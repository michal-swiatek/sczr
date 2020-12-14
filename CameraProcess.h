//
// Created by micho6 on 14.12.2020.
//

#ifndef SCZR_CAMERAPROCESS_H
#define SCZR_CAMERAPROCESS_H

#include "common.h"

#include <vector>
#include <linux/videodev2.h>

class CameraProcess
{
public:
    void init();
    void quit();

    void setup();
    void openStream() const;
    void closeStream();

    void readFrame(const char* file);
    void updateFrameData(const char* file);

    void run();

private:
    int fd{0};                      //  File descriptor of the video device (/dev/video0)

    byte* buffer{nullptr};          //  Local frame buffer
    v4l2_buffer bufferInfo{};       //  Holds info about read buffer

    std::vector<byte> outBuffer;    //  Buffer used to send data to ImageProcess
};

#endif //SCZR_CAMERAPROCESS_H
