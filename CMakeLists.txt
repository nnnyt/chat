cmake_minimum_required(VERSION 3.4)
project(chat)

set(CMAKE_CXX_STANDARD 11)

add_executable(chat_server server.cpp)
add_executable(chat_client client.cpp)
target_link_libraries(chat_server pthread)
target_link_libraries(chat_client pthread)