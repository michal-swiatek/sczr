//
// Created by micho6 on 14.12.2020.
//

#include "CameraProcess.h"

//  C++ libraries
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

//  System libraries
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

using namespace std;

void CameraProcess::init()
{
    //  Open the device
    fd = open("/dev/video0", O_RDWR);
    if(fd < 0){
        perror("Failed to open device, OPEN");
        exit(1);
    }

    //  Ask the device if it can capture frames
    v4l2_capability capability{};
    if(ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0){
        // something went wrong... exit
        perror("Failed to get device capabilities, VIDIOC_QUERYCAP");
        exit(1);
    }
}

void CameraProcess::quit()
{
    close(fd);
    fd = 0;
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

void CameraProcess::readFrame(const char* file)
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

    outBuffer.resize(bufferInfo.bytesused);
    std::copy(buffer, buffer + bufferInfo.bytesused, outBuffer.begin());
    updateFrameData(file);
}

void CameraProcess::updateFrameData(const char* file)
{
    // Write the data out to file
    ofstream outFile;
    outFile.open(file, ios::binary | ios::app);

    outFile.write(reinterpret_cast<const char *>(outBuffer.data()), bufferInfo.bytesused);

    // Close the file
    outFile.close();
}

void CameraProcess::run()
{
    init();
    setup();

    openStream();

    std::string path = "webcam_output";

    for (int i = 0; i < 20; ++i)
        readFrame((path + std::to_string(i) + ".jpg").c_str());

    closeStream();

    quit();
}
