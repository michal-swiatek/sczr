//
// Created by micho6 on 14.12.2020.
//

#ifndef SCZR_COMMON_H
#define SCZR_COMMON_H

#include <cinttypes>

/* red, green, blue color limits ( color we are looking for ) */
const int RL = 240;
const int GL = 250;
const int BL = 240;

const int DIST = 30; // distance allowed from the chosen color

// displaying depth of the cursor
const int LOWER_SCALE_BOUND = 5;
const int UPPER_SCALE_BOUND = 64;

const char MY_Q[] = "/my_q";
const char LOG_Q[] = "/log_q";
const char SEM_CONS_NAME[] = "/consumer";
const char SEM_PROD_NAME[] = "/producer";
const char FILE_NAME[] = "MEMFILE";
const char LOG_FILE_PATH[] = "data.txt";
const char LOG_FILE_PATH2[] = "data2.txt";

using byte = uint8_t;

const int LOG_COUNT = 100;

const int WIDTH = 640;
const int HEIGHT = 480;

const int FPS = 30;
const int FRAME_TIME = 1000 / FPS;

#endif //SCZR_COMMON_H
