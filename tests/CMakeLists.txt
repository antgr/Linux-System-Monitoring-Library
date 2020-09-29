cmake_minimum_required(VERSION 3.5)

project(baz LANGUAGES CXX VERSION 0.0.1)


add_subdirectory(thirdparty/Catch2)

add_executable(foo ${CMAKE_CURRENT_LIST_DIR}/test.cpp)
target_link_libraries(foo Catch2::Catch2)
