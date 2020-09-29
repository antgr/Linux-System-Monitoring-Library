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
#include "logfile.hpp"

#include "jsonConfig.hpp"
#include "webinterface.hpp"
#include "cxxopts.hpp"
#include <linuxmonitoring/linuxsysmonitor.hpp>
#include <boost/signals2.hpp>
#include "mqttClient/securemqttclient.hpp"
#include <functional>

#include <App.h>

using json = nlohmann::json;
using namespace boost::signals2;


class PerSocketData {
public:

    void shutdown() {
        this->con.disconnect();
    }

    void
    setConnectedWebsocket(uWS::WebSocket<true, true> *socket, std::shared_ptr<linuxsysmonitor> linuxsysmonitorinst) {
        this->connectedWS = socket;
        this->linuxMon = linuxsysmonitorinst;

        this->con = this->linuxMon->dataReady.connect([this](json values) {
            if (!values.empty()) {
                CLOGINFO() << "send to ws " << this->connectedWS;
                this->connectedWS->send(values.dump(), uWS::OpCode::TEXT);

            }
        });

    }

private:
    uWS::WebSocket<true, true> *connectedWS;
    std::shared_ptr<linuxsysmonitor> linuxMon;
    connection con;
};

class LinuxMQTTWebSocket {

private:
    uint16_t portToListen;
    std::string publicCerts;
    std::string privateKey;
    std::string passPhrase;
    std::unique_ptr<std::thread> spawnThread;
    std::vector<uWS::AsyncSocket<true>> connectedSockets;
    uWS::TemplatedApp<true> inst;
    std::shared_ptr<linuxsysmonitor> linuxMon;
    std::unique_ptr<securemqttclient> securemqttclient_p;
    uint32_t cnt;


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
                    CLOGINFO() << "open ws adr: " << ws;
                    static_cast<PerSocketData *>(ws->getUserData())->setConnectedWebsocket(ws, this->linuxMon);
                    //connectedSockets.push_back(*ws);

                },
                .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                    CLOGINFO() << "message ws adr: " << ws;
                    ws->send(message, opCode, true);
                },
                .drain = [](auto *ws) {
                    CLOGINFO() << "drain ws adr: " << ws;
                },
                .ping = [](auto *ws) {
                    CLOGINFO() << "ping ws adr: " << ws;
                },
                .pong = [](auto *ws) {
                    CLOGINFO() << "pong ws adr: " << ws;
                },
                .close = [](auto *ws, int code, std::string_view message) {
                    CLOGINFO() << "close ws adr:" << ws;
                    static_cast<PerSocketData *>(ws->getUserData())->shutdown();
                }
        }).listen(this->portToListen, [&](us_listen_socket_t *token) {
            if (token) {
                CLOGINFO() << "WebsocketServer Listening on port " << this->portToListen << " with route " << route;
            }
        }).run();
        CLOGINFO() << "WebsocketServer undefined behavior!";
    }


public:

    explicit LinuxMQTTWebSocket(std::string privateKeyPath,
                                std::string publicCertPath,
                                std::string passphrase = "",
                                uint16_t portNum = 4002) {
        this->portToListen = portNum;
        this->publicCerts = publicCertPath;
        this->privateKey = privateKeyPath;
        this->passPhrase = passphrase;
        cnt = 0;
        linuxMon = std::make_shared<linuxsysmonitor>(std::chrono::milliseconds(1000));
        securemqttclient_p = std::make_unique<securemqttclient>(JSONConfig::getconfig().get_mqttbroker().get_mqtt_id());
    }

    void runServer(std::string route, bool detach = false, std::string mqttTopic = "/linuxMonitor") {

        auto fut = securemqttclient_p->connectToBroker();
        fut.wait_for(std::chrono::milliseconds(3000));

        linuxMon->dataReady.connect([=](json values) {
            if (!values.empty()) {
                this->securemqttclient_p->publish(
                        mqtt::make_message(mqttTopic.c_str(),
                                           values.dump()));
                if (!(cnt++ % 7)) {
                    CLOGINFO() << values.dump();
                }
            }
        });


        spawnThread = std::make_unique<std::thread>([&]() { this->startServer(route); });
        if (detach) {
            spawnThread->detach();
        } else {
            spawnThread->join();
        }
    }

};


int main(int argc, char *argv[]) {

    cxxopts::Options options("BME680 Monitoring Testing",
                             "generates sensor values and tests secure webinterface and mqtt pushing");
    options.add_options()
            ("d,debug", "Enable debugging")
            ("f,json", "File name", cxxopts::value<std::string>());
    auto result = options.parse(argc, argv);


    if (result["debug"].as<bool>()) {
        logging::initLogging(vrlvl_debug, false);
    } else {
        logging::initLogging(vrlvl_info, false);
    }
    JSONConfig(result["json"].as<std::string>());

    if (JSONConfig::getconfig().get_misc().get_start_as_daemon()) {
        linuxsystem::setAppAsDaemon();
    }

    verbositylevels verbositylvl = vrlvl_debug;
    if (JSONConfig::getconfig().get_logging().get_verbositylevel() == "Debug") {
        verbositylvl = verbositylevels::vrlvl_debug;
    } else if (JSONConfig::getconfig().get_logging().get_verbositylevel() == "Info") {
        verbositylvl = verbositylevels::vrlvl_info;
    } else if (JSONConfig::getconfig().get_logging().get_verbositylevel() == "Warning") {
        verbositylvl = verbositylevels::vrlvl_warning;
    } else {
        verbositylvl = vrlvl_debug;
    }
    logging::setVerbosityLevel(verbositylvl);

    auto mqttWebsocketLinuxMonitor = std::make_unique<LinuxMQTTWebSocket>(
            JSONConfig::getconfig().get_http_server().get_private_key(),
            JSONConfig::getconfig().get_http_server().get_public_certs()
    );
    mqttWebsocketLinuxMonitor->runServer("/linuxmonitor", false,"/linuxMonitor");
    return 0;
}

