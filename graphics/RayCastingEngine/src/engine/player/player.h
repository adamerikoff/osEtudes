#ifndef PLAYER_H
#define PLAYER_H

#include "../constants.h"
#include "../map/map.h"

typedef struct Player{
    float positionX;
    float positionY;
    float width;
    float height;

    int turnDirection;
    int walkDirection;

    float rotationAngle;
    float walkSpeed;
    float turnSpeed;

} Player;

extern Player* initializePlayer(float positionX, float positionY, float width, float height);
extern void destroyPlayer(Player* player);
extern void renderPlayer(Player* player, SDL_Renderer* renderer);
extern void updatePlayer(Player* player, Map* map, float deltaTime);
extern void movePlayer(Player* player, Map* map, float deltaTime);
extern void processInputPlayer(Player* player, SDL_Event* event);



#endif