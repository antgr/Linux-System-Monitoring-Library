
cmake_minimum_required(VERSION 3.0)

SET(BUILD_SHARED_LIBS ON)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
add_library(libutil SHARED
        ${CMAKE_CURRENT_LIST_DIR}/linuxMemory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/linuxEthernet.cpp
        ${CMAKE_CURRENT_LIST_DIR}/linuxSys.cpp
        ${CMAKE_CURRENT_LIST_DIR}/linuxcpu.cpp)
target_link_libraries(libutil
        pthread
        rt
        )