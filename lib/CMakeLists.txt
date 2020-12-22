
cmake_minimum_required(VERSION 3.0)

SET(BUILD_SHARED_LIBS ON)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
add_library(linuxmonitoring SHARED
            ${CMAKE_CURRENT_LIST_DIR}/linux_memoryload.cpp
            ${CMAKE_CURRENT_LIST_DIR}/linux_networkload.cpp
            ${CMAKE_CURRENT_LIST_DIR}/linux_systemutil.cpp
            ${CMAKE_CURRENT_LIST_DIR}/linux_cpuload.cpp)
target_compile_options(linuxmonitoring PRIVATE ${COMPILER_SETTINGS})

target_link_libraries(linuxmonitoring
        pthread
        rt
        )
