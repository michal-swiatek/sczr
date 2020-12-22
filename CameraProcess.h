//
// Created by micho6 on 14.12.2020.
//

#ifndef SCZR_CAMERAPROCESS_H
#define SCZR_CAMERAPROCESS_H

//  C++/system libraries
#include <vector>
#include <chrono>
#include <linux/videodev2.h>

//  Project includes
#include "common.h"
#include "SharedMemory.h"

class CameraProcess
{
public:
    CameraProcess();
    ~CameraProcess();

    void setup();
    void openStream() const;
    void closeStream();

    void readFrame();
    void updateFrameData(std::chrono::system_clock::time_point& timestamp, int& frames);

    [[noreturn]] void run();

private:
    int fd{0};                      //  File descriptor of the video device (/dev/video0)

    byte* buffer{nullptr};          //  Local frame buffer
    v4l2_buffer bufferInfo{};       //  Holds info about read buffer

    SharedMemory shm;               //  Inter-process buffer to which rgb data of the frame will be copied
};

#endif //SCZR_CAMERAPROCESS_H
