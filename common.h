//
// Created by micho6 on 14.12.2020.
//

#ifndef SCZR_COMMON_H
#define SCZR_COMMON_H

#include <cinttypes>

/* red, green, blue color limits ( color we are looking for ) */
const int RL = 230;
const int GL = 255;
const int BL = 230;

const int DIST = 20; // distance allowed from the chosen color


const char MY_Q[] = "/my_q";
const char LOG_Q[] = "/log_q";
const char SEM_CONS_NAME[] = "/consumer";
const char SEM_PROD_NAME[] = "/producer";
const char FILE_NAME[] = "MEMFILE";
const char LOG_FILE_PATH[] = "data.txt";

using byte = uint8_t;

const int LOG_COUNT = 1000;

const int WIDTH = 640;
const int HEIGHT = 480;

const int FPS = 60;
const int FRAME_TIME = 1000 / FPS;

#endif //SCZR_COMMON_H
