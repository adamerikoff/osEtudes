#ifndef RAY_H
#define RAY_H

#include "../constants.h"
#include "../player/player.h"
#include "../map/map.h"

typedef struct Ray {
    float rayAngle;
    float wallHitY;
    float wallHitX;
    float distance;

    int wasHitVertical;
    bool isRayUp;
    bool isRayDown;
    bool isRayLeft;
    bool isRayRight;
    int wallHitContent;
} Ray;

extern float normalizeAngle(float angle);
extern float distanceBetweenPoints(float x1, float y1, float x2, float y2);

extern void castRay(Player* player, Ray* ray, Map* map);
extern void castAllRays(Player* player, Ray* rays, Map* map);

extern void renderRays(Player* player, Ray* rays, SDL_Renderer* renderer);
#endif