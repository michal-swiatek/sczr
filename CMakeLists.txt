cmake_minimum_required(VERSION 3.17)
project(sczr)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
add_executable(SynchroTest main.cpp SharedMemory.cpp SharedMemory.h SharedQueue.cpp SharedQueue.h CameraProcess.cpp CameraProcess.h common.h)
find_library(LIBRT rt)
if(LIBRT)
    target_link_libraries(SynchroTest ${LIBRT})
endif()