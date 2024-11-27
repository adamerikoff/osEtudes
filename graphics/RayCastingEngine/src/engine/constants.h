#ifndef CONSTANTS_H
#define CONSTANTS_H
// CONSTANTS
#define FPS 30
#define FRAME_TIME_LENGTH (1000 / FPS)

#define NUM_TEXTURES 8

#define PI 3.14159265
#define DOUBLE_PI 6.28318530

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FOV_ANGLE (60 * (PI / 180))
#define NUM_RAYS WINDOW_WIDTH

#define TILE_SIZE 32

#define MAP_ROWS (WINDOW_HEIGHT / TILE_SIZE)
#define MAP_COLS (WINDOW_WIDTH / TILE_SIZE)
#define MAP_SCALE_FACTOR 0.3

#define INT_MAX 2147483647

#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#endif