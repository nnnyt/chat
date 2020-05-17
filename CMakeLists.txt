cmake_minimum_required(VERSION 3.14)
project(chat)

set(CMAKE_CXX_STANDARD 11)

add_executable(chat_server server.cpp)
add_executable(chat_client client.cpp)