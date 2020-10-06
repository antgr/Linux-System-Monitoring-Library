

cmake_minimum_required(VERSION 3.0)
project(linuxsystemMonitor)

set(CMAKE_CXX_STANDARD 17)

add_executable(linuxsystemMonitor
        ${CMAKE_CURRENT_LIST_DIR}/linuxsysmonitor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        )

target_compile_options(linuxsystemMonitor PRIVATE ${COMPILER_SETTINGS})

target_link_libraries(linuxsystemMonitor
        pthread
        crypto
        ssl
        )

target_link_libraries(linuxsystemMonitor uSockets z)