cmake_minimum_required(VERSION 3.10)

project(SerialCommTest)

add_executable(SerialCommTest 
serial_comm.cpp
pico_comm.cpp
test_serial_comm.cpp)