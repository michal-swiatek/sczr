cmake_minimum_required(VERSION 3.17)
project(sczr)

set(CMAKE_CXX_STANDARD 14)

add_executable(example example.cpp CameraProcess.h common.h)
add_executable(main main.cpp CameraProcess.h common.h CameraProcess.cpp)