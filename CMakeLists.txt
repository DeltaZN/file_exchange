cmake_minimum_required(VERSION 3.17)
project(spo_lab_3_file_exchange C)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_C_STANDARD 11)

add_executable(spo_lab_3_file_exchange main.c file_reader.c file_reader.h list.c list.h cmd_handler.c cmd_handler.h util.c util.h app_context.c app_context.h net/udp_server.c net/udp_server.h net/udp_search.c net/udp_search.h shell_module.c shell_module.h net/tcp_server.c net/tcp_server.h net/tcp_client.c net/tcp_client.h)
target_link_libraries(spo_lab_3_file_exchange ${CURSES_LIBRARIES} OpenSSL::SSL Threads::Threads)