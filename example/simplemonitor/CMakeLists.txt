

cmake_minimum_required(VERSION 3.0)
project(simple_linuxsystemMonitor)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_linuxsystemMonitor
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        )

target_compile_options(simple_linuxsystemMonitor PRIVATE ${COMPILER_SETTINGS})
target_link_libraries(simple_linuxsystemMonitor linuxmonitoring)

