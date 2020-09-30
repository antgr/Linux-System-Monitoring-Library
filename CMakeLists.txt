cmake_minimum_required(VERSION 3.0)
project(LinuxMonitoring LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")





set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi -Wall -pedantic -Wextra -g -ggdb")

include_directories(thirdparty/cxxopts/include)
include_directories(thirdparty/Catch2/single_include)
include_directories(thirdparty/json/single_include/nlohmann)
include_directories(thirdparty/json/single_include)
include_directories(thirdparty/uSockets/src)
include_directories(thirdparty/uWebSockets/src)
link_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/uSockets)
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/uSockets)



include(src/CMakeLists.txt)

include(tests/CMakeLists.txt)

include(example/LinuxSystemMonitor/CMakeLists.txt)
include(example/simplemonitor/CMakeLists.txt)