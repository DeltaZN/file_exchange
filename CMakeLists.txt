cmake_minimum_required(VERSION 3.17)
project(spo_lab_3_file_exchange C)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)

set(CMAKE_C_STANDARD 11)

add_executable(spo_lab_3_file_exchange main.c file_reader.c file_reader.h list.c list.h)
target_link_libraries(spo_lab_3_file_exchange ${CURSES_LIBRARIES} OpenSSL::SSL)