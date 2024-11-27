#ifndef MAP_H
#define MAP_H

#include "../constants.h"

typedef struct {
    unsigned int rows;
    unsigned int columns;

    int** array;
} Map;

extern Map* generateEmptyMap(const unsigned int rows, const unsigned int columns);
extern Map* generateCircularRoomMap(const unsigned int rows, const unsigned int columns, const unsigned int radius);
extern Map* generateRoomWithColumns(const unsigned int rows, const unsigned int columns, const unsigned int numColumns);
extern void renderMap(const Map* map, SDL_Renderer* renderer);
extern void destroyMap(Map* map);
extern bool isWall(Map* map, int x, int y);
#endif