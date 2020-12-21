//
// Created by micho6 on 14.12.2020.
//

#include "CameraProcess.h"

//  C++ libraries
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

//  System libraries
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

using namespace std;

void YUVDataToRGBBuffer(int y, int u, int v, byte* buf);

CameraProcess::CameraProcess() : shm(true)
{
    //  Open the device
//    fd = open("/dev/video0", O_RDWR);
//    if(fd < 0){
//        perror("Failed to open device /dev/video0, OPEN");
//        exit(1);
//    }
//
//    //  Ask the device if it can capture frames
//    v4l2_capability capability{};
//    if(ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0){
//        // something went wrong... exit
//        perror("Failed to get device capabilities, VIDIOC_QUERYCAP");
//        exit(1);
//    }
}

CameraProcess::~CameraProcess() {
//    close(fd);
//    fd = 0;
}

void CameraProcess::setup()
{
    //  Set Image format
    v4l2_format imageFormat{};
    imageFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    imageFormat.fmt.pix.width = WIDTH;
    imageFormat.fmt.pix.height = HEIGHT;
    imageFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    imageFormat.fmt.pix.field = V4L2_FIELD_NONE;
    // tell the device you are using this format
    if(ioctl(fd, VIDIOC_S_FMT, &imageFormat) < 0){
        perror("Device could not set format, VIDIOC_S_FMT");
        exit(1);
    }

    //  Request Buffers from the device
    v4l2_requestbuffers requestBuffer = {0};
    requestBuffer.count = 1; // one request buffer
    requestBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // request a buffer which we can use for capturing frames
    requestBuffer.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_REQBUFS, &requestBuffer) < 0){
        perror("Could not request buffer from device, VIDIOC_REQBUFS");
        exit(1);
    }

    //  Query the buffer to get raw data ie. ask for the you requested buffer
    //  and allocate memory for it
    v4l2_buffer queryBuffer = {0};
    queryBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queryBuffer.memory = V4L2_MEMORY_MMAP;
    queryBuffer.index = 0;
    if(ioctl(fd, VIDIOC_QUERYBUF, &queryBuffer) < 0){
        perror("Device did not return the buffer information, VIDIOC_QUERYBUF");
        exit(1);
    }

    //  Use a pointer to point to the newly created buffer
    //  mmap() will map the memory address of the device to an address in memory
    buffer = (byte*)mmap(nullptr, queryBuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                               fd, queryBuffer.m.offset);
    memset(buffer, 0, queryBuffer.length);

    //  Prepare info buffer
    memset(&bufferInfo, 0, sizeof(bufferInfo));
    bufferInfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferInfo.memory = V4L2_MEMORY_MMAP;
    bufferInfo.index = 0;
}

void CameraProcess::openStream() const
{
    if(ioctl(fd, VIDIOC_STREAMON, &bufferInfo.type) < 0){
        perror("Could not start streaming, VIDIOC_STREAMON");
        exit(1);
    }
}

void CameraProcess::closeStream()
{
    if(ioctl(fd, VIDIOC_STREAMOFF, &bufferInfo.type) < 0){
        perror("Could not end streaming, VIDIOC_STREAMOFF");
        exit(1);
    }
}

void CameraProcess::readFrame()
{
    //  Queue the buffer
    if(ioctl(fd, VIDIOC_QBUF, &bufferInfo) < 0){
        perror("Could not queue buffer, VIDIOC_QBUF");
        exit(1);
    }

    //  Dequeue the buffer
    if(ioctl(fd, VIDIOC_DQBUF, &bufferInfo) < 0){
        perror("Could not dequeue the buffer, VIDIOC_DQBUF");
        exit(1);
    }

#ifndef NDEBUG
    //  Frames get written after dequeuing the buffer
    cout << "Buffer has: " << (double)bufferInfo.bytesused / 1024
         << " KBytes of data" << endl;
#endif
}

void YUVDataToRGBBuffer(int y, int u, int v, byte *buf)
{
    u -= 128;
    v -= 128;

    int r = static_cast<int>(y + 1.370705 * v);
    int g = static_cast<int>(y - 0.698001 * v - 0.337633 * u);
    int b = static_cast<int>(y + 1.732446 * u);

    // Clamp to 0..1
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    *buf = r;
    *(buf + 1) = g;
    *(buf + 2) = b;
}

void CameraProcess::updateFrameData()
{
    shm.prodOperation([&](){

        byte* shmBuf = shm.buffer;
        for (int i = 0; i < 640 * 480 * 2; i += 4, shmBuf += 6)
        {
            int y1 = buffer[i];
            int u = buffer[i + 1];
            int y2 = buffer[i + 2];
            int v = buffer[i + 3];

            YUVDataToRGBBuffer(y1, u, v, shmBuf);
            YUVDataToRGBBuffer(y2, u, v, shmBuf + 3);
        }
    });
}

[[noreturn]] void CameraProcess::run()
{
//    setup();
//    openStream();

    int frames = 0;
    buffer = new byte[640 * 480 * 2];

    while (true)
    {
        //  Start counting time
        auto start = std::chrono::system_clock::now();

//        readFrame();
        updateFrameData();

        auto difference = std::chrono::system_clock::now() - start;
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(difference).count();

#ifndef NDEBUG
        std::cout << "CameraProcess (running): " << ++frames << ' ' << FRAME_TIME - time << '\n';
#endif

        if (time < FRAME_TIME)
            usleep(1000 * (FRAME_TIME - time));
    }

    //  Upon exit OS will automatically close opened file descriptors
}
