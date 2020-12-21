cmake_minimum_required(VERSION 3.17)
project(sczr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

find_library(LIBRT rt)

add_executable(SynchroTest main.cpp SharedMemory.cpp SharedQueue.cpp CameraProcess.cpp ImageProcess.cpp ImageProcess.h)

if(LIBRT)
    target_link_libraries(SynchroTest ${LIBRT})
endif()