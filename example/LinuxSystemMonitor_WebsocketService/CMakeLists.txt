

cmake_minimum_required(VERSION 3.0)
project(linuxsystemMonitor)

set(CMAKE_CXX_STANDARD 17)

add_executable(linuxsystemMonitor
        ${CMAKE_CURRENT_LIST_DIR}/linuxsysmonitor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        )


target_link_libraries(linuxsystemMonitor
        linuxmonitoring
        pthread
        )


target_link_libraries(linuxsystemMonitor z)
target_link_libraries(linuxsystemMonitor ${CMAKE_HOME_DIRECTORY}/thirdparty/uWebSockets/uSockets/uSockets.a)
target_link_libraries(linuxsystemMonitor
        crypto
        ssl
        )