//
// Created by miki on 21.12.2020.
//

#include "ImageProcess.h"

#ifndef NDEBUG
#include <iostream>
#endif

[[noreturn]] void ImageProcess::run()
{
    while(true)
    {
        GameMes result{};
        shm.consOperation(findCenter, shm, &result);
        send_q.sendMes(&result);

#ifndef NDEBUG
        std::cout << "ImageProcess (running): " << result.id << ' ' << result.x << ' ' << result.y << '\n';
#endif
    }
}

void ImageProcess::findCenter(SharedMemory& shm, GameMes* result)
{
    // get border coordinates before searching for the color center
    int x_max=-1, x_min=WIDTH, y_max=-1, y_min=HEIGHT;

    auto ts = shm.data->timestamp;
    auto fid = shm.data->id;

    // +=3 because one pixel consists of 3 bytes
    for(int i = 0; i < sizeof(shm.data->buffer); i+=3)
    {
        int r = (unsigned char)shm.data->buffer[i];
        int g = (unsigned char)shm.data->buffer[i+1];
        int b = (unsigned char)shm.data->buffer[i+2];

        // find current position on screen from 1d array of subpixels
        int x_curr = (i/3)%WIDTH;
        int y_curr = (i/3)/WIDTH;

        // checking if not far enough from the given color (RL,BL,GL)
         if((r-RL)*(r-RL) + (g-GL)*(g-GL) + (b-BL)*(b-BL) <= DIST * DIST)
//        if(g > 200)
        {
            // we are looking for min and max coordinates that we can approximate to being the desired color
            x_min = std::min(x_curr, x_min);
            y_min = std::min(y_curr, y_min);
            x_max = std::max(x_curr, x_max);
            y_max = std::max(y_curr, y_max);
        }
    }

    // the center of color would be the average of min and max coordinates - which is the center of a rectangle
    result->x = WIDTH-(x_max+x_min)/2;
    result->y = (y_max+y_min)/2;

    result->scale = (y_max - y_min) * (x_max - x_min);

    result->timestamp = ts;
    result->id = fid;
}