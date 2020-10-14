/*
  _ _                                _                                       _
 | (_)                              | |                                     (_)
 | |_ _ __  _   ___  _____ _   _ ___| |_ ___ _ __ ___  _ __ ___   ___  _ __  _  ___  _ __
 | | | '_ \| | | \ \/ / __| | | / __| __/ _ \ '_ ` _ \| '_ ` _ \ / _ \| '_ \| |/ _ \| '__|
 | | | | | | |_| |>  <\__ \ |_| \__ \ ||  __/ | | | | | | | | | | (_) | | | | | (_) | |
 |_|_|_| |_|\__,_/_/\_\___/\__, |___/\__\___|_| |_| |_|_| |_| |_|\___/|_| |_|_|\___/|_|
                            __/ |
                           |___/

 */



#include <memory>
#include <iostream>
#include <json.hpp>
#include <string>
#include <streambuf>
#include <thread>
#include <atomic>
#include <chrono>

#include <cxxopts.hpp>
#include "linuxsysmonitor.hpp"
#include <functional>
#include <utility>

#include <App.h>

using json = nlohmann::json;


class PerSocketData : public IObserver {
public:

    void shutdown() {
        this->linuxMon->Detach(this);
    }

    void
    setConnectedWebsocket(uWS::WebSocket<true, true> *socket, std::shared_ptr<linuxsysmonitor> linuxsysmonitorinst) {
         this->connectedWS = socket;
         this->linuxMon = std::move(linuxsysmonitorinst);
         this->linuxMon->Attach(this);
         std::cout << "send to ws " << this->connectedWS << std::endl;
         this->connectedWS->send("send json dump", uWS::OpCode::TEXT);
    }
    void Update(const json &message_from_subject) override {
        this->connectedWS->send(message_from_subject.dump(), uWS::OpCode::TEXT);
    }

private:
    uWS::WebSocket<true, true> *connectedWS;
    std::shared_ptr<linuxsysmonitor> linuxMon;
};

class LinuxMonitorWebSocketBridge {

private:
    uint16_t portToListen;
    std::string publicCerts;
    std::string privateKey;
    std::string passPhrase;
    std::unique_ptr<std::thread> spawnThread;
    std::vector<uWS::AsyncSocket<true>> connectedSockets;
    uWS::TemplatedApp<true> inst;
    std::shared_ptr<linuxsysmonitor> linuxMon;


    void startServer(std::string route) {

        uWS::SSLApp({
                            .key_file_name = this->privateKey.c_str(),
                            .cert_file_name = this->publicCerts.c_str(),
                            .passphrase = this->passPhrase.c_str()
                    }).ws<PerSocketData>(route, {
                .compression = uWS::SHARED_COMPRESSOR,
                .maxPayloadLength = 16 * 1024,
                .idleTimeout = 100,
                .maxBackpressure = 1 * 1024 * 1024,
                .open = [&](auto *ws) {
                    std::cout << "open ws adr: " << ws << std::endl;
                    static_cast<PerSocketData *>(ws->getUserData())->setConnectedWebsocket(ws, this->linuxMon);
                    //connectedSockets.push_back(*ws);

                },
                .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                    std::cout << "message ws adr: " << ws << std::endl;
                    ws->send(message, opCode, true);
                },
                .drain = [](auto *ws) {
                    std::cout << "drain ws adr: " << ws << std::endl;
                },
                .ping = [](auto *ws) {
                    std::cout << "ping ws adr: " << ws << std::endl;
                },
                .pong = [](auto *ws) {
                    std::cout << "pong ws adr: " << ws << std::endl;
                },
                .close = [](auto *ws, int code, std::string_view message) {
                    std::cout << "close ws adr:" << ws << std::endl;
                    static_cast<PerSocketData *>(ws->getUserData())->shutdown();
                }
        }).listen(this->portToListen, [&](us_listen_socket_t *token) {
            if (token) {
                std::cout   << "WebsocketServer Listening on port "
                            << this->portToListen
                            << " with route "
                            << route << std::endl;
            }
        }).run();
        std::cerr << "WebsocketServer undefined behavior!"<< std::endl;
    }


public:

    explicit LinuxMonitorWebSocketBridge(std::string privateKeyPath,
                                std::string publicCertPath,
                                std::string passphrase = "",
                                uint16_t portNum = 4002) {
        this->portToListen = portNum;
        this->publicCerts = std::move(publicCertPath);
        this->privateKey = std::move(privateKeyPath);
        this->passPhrase = std::move(passphrase);
        linuxMon = std::make_shared<linuxsysmonitor>(std::chrono::milliseconds(1000));
    }

    void runServer(std::string route, bool detach = false) {
        spawnThread = std::make_unique<std::thread>([&]() { this->startServer(route); });
        if (detach) {
            spawnThread->detach();
        } else {
            spawnThread->join();
        }
    }

};


int main(int argc, const char* argv[]) {

    cxxopts::Options options("BME680 Monitoring Testing",
                             "generates sensor values and tests secure webinterface and mqtt pushing");
    options.add_options()
            ("d,debug", "Enable debugging")
            ("c,public_cert", "path to public cert", cxxopts::value<std::string>()->default_value(""))
            ("k,private_key", "path to private key", cxxopts::value<std::string>()->default_value(""))
            ("p,websocketport_num", "Port to listen on", cxxopts::value<int>()->default_value("4002"))
            ("f,json", "File name", cxxopts::value<std::string>());
    auto result = options.parse(argc, argv);

    auto webSocketLinuxMonitor = std::make_unique<LinuxMonitorWebSocketBridge>(
                std::string(result["private_key"].as<std::string>()),
                std::string(result["public_cert"].as<std::string>()),
                "",
                result["websocketport_num"].as<int>()
        );

    webSocketLinuxMonitor->runServer("/linuxmonitor", false);
    return 0;
}

