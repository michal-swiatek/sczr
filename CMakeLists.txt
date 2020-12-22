cmake_minimum_required(VERSION 3.17)
project(sczr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

find_library(LIBRT rt)
find_package(SFML 2 REQUIRED network audio graphics window system)

if(SFML_FOUND)
    message("cokolwiek")
    include_directories(${SFML_INCLUDE_DIR})
    set(SFML_FLAGS -lsfml-graphics -lsfml-window -lsfml-system)
endif()

add_executable(SynchroTest main.cpp SharedMemory.cpp SharedQueue.cpp CameraProcess.cpp ImageProcess.cpp ImageProcess.h LoggerProcess.cpp LoggerProcess.h GameProcess.cpp GameProcess.h BMP.h)

target_link_libraries(SynchroTest ${LIBRT} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${SFML_FLAGS})
