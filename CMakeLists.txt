cmake_minimum_required(VERSION 3.0)
project(LinuxMonitoring LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(COMPILER_SETTINGS
    -Wall
    -pedantic
    -Wextra
    -g
    -ggdb
    -Werror
    -Wshadow
    -Wold-style-cast
    -Wsign-conversion
    -Woverloaded-virtual
    -Wuseless-cast
    -Wsign-promo
    -Wstrict-null-sentinel
    -Wno-unused
    -Wstrict-overflow=1
    -Wlogical-op
    -Winit-self
    -Wformat=2
    -Wcast-align
    -Wcast-qual
    -Wdisabled-optimization
    -Wmissing-include-dirs
    -Wno-format-nonliteral
    )




set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

include_directories(thirdparty/cxxopts/include)
include_directories(thirdparty/Catch2/single_include)
include_directories(thirdparty/json/single_include/nlohmann)
include_directories(thirdparty/json/single_include)
include_directories(thirdparty/uWebSockets/uSockets/src)
include_directories(thirdparty/uWebSockets/src)
link_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/uWebSockets/uSockets)
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/uWebSockets/uSockets)



include(lib/CMakeLists.txt)
include(tests/CMakeLists.txt)
include(example/LinuxSystemMonitor_WebsocketService/CMakeLists.txt)
include(example/simplemonitor/CMakeLists.txt)