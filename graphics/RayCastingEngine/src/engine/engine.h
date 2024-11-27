#ifndef ENGINE_H
#define ENGINE_H

#include "constants.h"
#include "map/map.h"
#include "player/player.h"
#include "enemy/enemy.h"
#include "ray/ray.h"

#include <SDL2/SDL.h>

typedef struct Engine{
    unsigned int ticksLastTime;

    SDL_Window* window;
    SDL_Renderer* renderer;

    bool isRunning;

    char* title;
    int   width;
    int   height;

    Map* map;
    Player* player;
    Ray* rays;

    Uint32* colourBuffer;
    SDL_Texture* colourBufferTexture;
} Engine;

extern Engine* initializeEngine(const char* title);
extern void destroyEngine(Engine* engine);

extern void generate3Dprojection(Engine* engine);
extern void renderColourBuffer(Engine* engine);
extern void clearColourBuffer(Engine* engine, Uint32 colour);
extern void renderEngine(Engine* engine);
extern void updateEngine(Engine* engine);

extern void processInputEngine(Engine* engine);

extern void loopEngine(Engine* engine);


#endif