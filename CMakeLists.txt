cmake_minimum_required(VERSION 3.4)
project(chat)

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cpp)
add_executable(client client.cpp)
target_link_libraries(server pthread)
target_link_libraries(client pthread)