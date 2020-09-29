

cmake_minimum_required(VERSION 3.0)
project(linuxsystemMonitor)

set(CMAKE_CXX_STANDARD 17)

if (NOT CROSSCOMPILE)
add_executable(linuxsystemMonitor
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        )


target_link_libraries(linuxsystemMonitor
        paho-mqttpp3
        paho-mqtt3as
        pthread
        libutil
        crypto
        ssl
        libwebinterface
        cfglib
        )

target_link_libraries(linuxsystemMonitor uSockets z)
endif (NOT CROSSCOMPILE)