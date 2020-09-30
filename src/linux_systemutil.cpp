#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <sys/types.h>
#include <csignal>
#include <cstring>
#include <netinet/in.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <pwd.h>
#include <thread>

#include "linux_systemutil.hpp"


long linuxUtil::getCPUtemp(void) {
    FILE *fp;
    char buffer[4000];
    size_t bytes_read;
    long temp;
    if ((fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r")) == NULL) {
        perror("fopen()");
        return 0;
    }
    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (bytes_read == 0 || bytes_read == sizeof(buffer))
        return 0;
    buffer[bytes_read] = '\0';
    sscanf(buffer, "%ld", &temp);
    return (temp);
}

int linuxUtil::getProcIdByName(std::string procName) {
    int pid = -1;
    DIR *dp = opendir("/proc");
    if (dp != nullptr) {
        struct dirent *dirp;
        while (pid < 0 && (dirp = readdir(dp))) {
            int id = atoi(dirp->d_name);
            if (id > 0) {
                std::string cmdPath{"/proc/"};
                cmdPath.append(dirp->d_name);
                cmdPath.append("/cmdline");
                std::ifstream cmdFile(cmdPath.c_str());
                std::string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty()) {
                    size_t pos = cmdLine.find('\0');
                    if (pos != std::string::npos)
                        cmdLine = cmdLine.substr(0, pos);
                    pos = cmdLine.rfind('/');
                    if (pos != std::string::npos)
                        cmdLine = cmdLine.substr(pos + 1);
                    if (strcmp(procName.c_str(), cmdLine.c_str()) == 0) {
                        pid = id;
                    }
                }
            }
        }
    }
    closedir(dp);
    return pid;
}

int linuxUtil::killProcessById(int pid, std::string procName) {
    if (pid == -1) {
        throw std::runtime_error(
                "Nothing to Kill, no Process " + procName + " PID " + std::to_string(pid));
    }
    int ret = kill(pid, 9);
    if (ret == -1) {
        throw std::runtime_error("killing " + procName + " was not successful!");
    }
    return ret;
}

uint64_t linuxUtil::getSysUpTime() {
    ///proc/uptime
    /// 325679.29 320697.33
    std::ifstream upTimeFile;
    upTimeFile.open("/proc/uptime");

    if (!upTimeFile.is_open()) {
        return 0;
    }

    uint64_t beforeBootTime;
    uint64_t SysUptime = 0;
    std::string line;
    while (std::getline(upTimeFile, line)) {
        sscanf(line.c_str(), "%lu %lu", static_cast<uint64_t *>(&SysUptime), static_cast<uint64_t *>(&beforeBootTime));
    }
    upTimeFile.close();
    return SysUptime;
}


bool linuxUtil::setAppAsDaemon() {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        return false;
        exit(EXIT_FAILURE);
    }


    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);
    auto retval = chdir("/test");
    if (retval < 0) {
        return false;
    }

    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }
    return true;
}

long linuxUtil::getFreeDiskSpace(std::string absoluteFilePath) {
    struct statvfs buf;

    if (!statvfs(absoluteFilePath.c_str(), &buf)) {
        unsigned long blksize, blocks, freeblks, disk_size, used, free;
        printf("blksize :  %ld\n", buf.f_bsize);
        printf("blocks :  %ld\n", buf.f_blocks);
        printf("bfree :  %ld\n", buf.f_bfree);
        printf("bavail: %ld\n", buf.f_bavail);
        printf("f_frsize:%ld\n", buf.f_frsize);
        blksize = buf.f_bsize;
        blocks = buf.f_blocks;
        freeblks = buf.f_bfree;
        disk_size = blocks * blksize;
        free = freeblks * blksize;
        used = disk_size - free;

        printf("disk %s disksize: %ld free %ld used %ld\n", absoluteFilePath.c_str(), disk_size, free, used);
        return free;
    } else {
        return -1;
    }
    return -1;
}

long long linuxUtil::userAvailableFreeSpace() {
    struct statvfs stat;
    struct passwd *pw = getpwuid(getuid());
    if (NULL != pw && 0 == statvfs(pw->pw_dir, &stat)) {
        printf("path %s\n", pw->pw_dir);
        long long freeBytes = (uint64_t) stat.f_bavail * stat.f_frsize;
        return freeBytes;
    }
    return 0ULL;
}

std::string linuxUtil::getOSVersion_Signature(void) {
    std::ifstream versionFile;
    versionFile.open("/proc/version_signature");

    if (!versionFile.is_open()) {
        return std::string();
    }
    std::string line;
    std::getline(versionFile, line);

    versionFile.close();
    return line;
}

std::string linuxUtil::getOsVersionString(void) {
    std::ifstream versionFile;
    versionFile.open("/proc/version");

    if (!versionFile.is_open()) {
        return std::string();
    }
    std::string line;
    std::getline(versionFile, line);

    versionFile.close();
    return line;
}

bool linuxUtil::isDeviceOnline(std::string address) {
    const std::string processPrefix = {"ping -c 1 -w 1 "};
    const std::string processPostfix = {" 2>&1"};
    auto fd = popen((processPrefix + address + processPostfix).c_str(), "r");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if (fd == nullptr) {
        return false;
    }
    char buff[1000];
    char *ptr = buff;
    size_t sz = sizeof(buff);
    while (getline(&ptr, &sz, fd) != -1) {
        std::string line(buff);
        if (line.find(" 1 received") != std::string::npos) {
            pclose(fd);
            return true;
        }
        if (line.find("100% packet loss") != std::string::npos) {
            pclose(fd);
            return false;
        }
    }
    return false;
}

uint32_t linuxUtil::getNumOfThreadsByThisProcess() {
    uint32_t Threads = 0;
    std::ifstream memoryFile;
    memoryFile.open("/proc/self/status");
    std::string line;
    while (std::getline(memoryFile, line)) {
        sscanf(line.c_str(), "Threads: %u", static_cast<uint32_t *>(&Threads));
    }
    return Threads;
}

uint32_t linuxUtil::getNumOfThreadsByPID(int Pid) {
    uint32_t Threads = 0;
    std::ifstream memoryFile;
    memoryFile.open("/proc/self/" + std::to_string(Pid));
    std::string line;
    while (std::getline(memoryFile, line)) {
        sscanf(line.c_str(), "Threads: %u", static_cast<uint32_t *>(&Threads));
    }
    return Threads;
}
