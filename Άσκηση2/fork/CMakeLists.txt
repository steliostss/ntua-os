cmake_minimum_required(VERSION 3.10)
project(fork C)

set(CMAKE_C_STANDARD 11)

add_executable(fork main.c proc-common.h proc-common.c)