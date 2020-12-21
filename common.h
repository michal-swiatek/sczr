//
// Created by micho6 on 14.12.2020.
//

#ifndef SCZR_COMMON_H
#define SCZR_COMMON_H

#include <cinttypes>

/* red, green, blue color limits ( color we are looking for ) */
#define RL 150
#define GL 255
#define BL 150

#define DIST 4 // distance allowed from the chosen color

using byte = uint8_t;

const int WIDTH = 640;
const int HEIGHT = 480;

const int FPS = 1;
const int FRAME_TIME = 1000 / FPS;

#endif //SCZR_COMMON_H
