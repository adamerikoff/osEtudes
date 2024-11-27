#include "ray.h"

float normalizeAngle(float angle) {
    angle = remainder(angle, DOUBLE_PI);
    if (angle < 0) {
        angle = DOUBLE_PI + angle;
    }
    return angle;
}
float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void castRay(Player* player, Ray* ray, Map* map) {
    ray->rayAngle = normalizeAngle(ray->rayAngle);

    bool isRayDown = ray->rayAngle > 0 && ray->rayAngle < PI;
    bool isRayUp = !isRayDown; 
    bool isRayRight = (ray->rayAngle < 0.5 * PI) || (ray->rayAngle > 1.5 * PI);
    bool isRayLeft = !isRayRight;

    float interceptX, interceptY;
    float stepX, stepY;

    bool foundHorizontalWallHit = false;
    float horizontalWallHitX = 0;
    float horizontalWallHitY = 0;
    int horizontalWallContent = 0;

    interceptY = floor(player->positionY / TILE_SIZE) * TILE_SIZE;
    interceptY += isRayDown ? TILE_SIZE : 0;

    interceptX = player->positionX + (interceptY - player->positionY) / tan(ray->rayAngle);

    stepY = TILE_SIZE;
    stepY *= isRayUp ? -1 : 1;

    stepX = TILE_SIZE / tan(ray->rayAngle);
    stepX *= (isRayLeft && stepX > 0) ? -1 : 1;
    stepX *= (isRayRight && stepX < 0) ? -1 : 1;

    float nextHorizontalTouchX = interceptX;
    float nextHorizontalTouchY = interceptY;

    while (nextHorizontalTouchX >= 0 && nextHorizontalTouchX < WINDOW_WIDTH && nextHorizontalTouchY >= 0 && nextHorizontalTouchY < WINDOW_HEIGHT) {
        float toCheckX = nextHorizontalTouchX;
        float toCheckY = nextHorizontalTouchY + (isRayUp ? -1 : 0);
        if (isWall(map, (int)toCheckX, (int)toCheckY)) {
            horizontalWallHitX = nextHorizontalTouchX;
            horizontalWallHitY = nextHorizontalTouchY;
            horizontalWallContent = map->array[((int)floor(toCheckY) / TILE_SIZE)][((int)floor(toCheckX) / TILE_SIZE)];
            foundHorizontalWallHit = true;
            break;
        } else {
            nextHorizontalTouchX += stepX;
            nextHorizontalTouchY += stepY;
        }
    }

    bool foundVerticalWallHit = false;
    float verticalWallHitX = 0;
    float verticalWallHitY = 0;
    int verticalWallContent = 0;

    interceptX = floor(player->positionX / TILE_SIZE) * TILE_SIZE;
    interceptX += isRayRight ? TILE_SIZE : 0;

    interceptY = player->positionY + (interceptX - player->positionX) * tan(ray->rayAngle);

    stepX = TILE_SIZE;
    stepX *= isRayLeft ? -1 : 1;

    stepY = TILE_SIZE * tan(ray->rayAngle);
    stepY *= (isRayUp && stepY > 0) ? -1 : 1;
    stepY *= (isRayDown && stepY < 0) ? -1 : 1;

    float nextVerticalTouchX = interceptX;
    float nextVerticalTouchY = interceptY;

    while (nextVerticalTouchX >= 0 && nextVerticalTouchX < WINDOW_WIDTH && 
       nextVerticalTouchY >= 0 && nextVerticalTouchY < WINDOW_HEIGHT) {
        float toCheckX = nextVerticalTouchX + (isRayLeft ? -1 : 0);
        float toCheckY = nextVerticalTouchY;

        if (isWall(map, (int)toCheckX, (int)toCheckY)) {
            verticalWallHitX = nextVerticalTouchX;
            verticalWallHitY = nextVerticalTouchY;
            verticalWallContent = map->array[((int)floor(toCheckY) / TILE_SIZE)][((int)floor(toCheckX) / TILE_SIZE)];
            foundVerticalWallHit = true;
            break;
        } else {
            nextVerticalTouchX += stepX;
            nextVerticalTouchY += stepY;
        }
    }

    float horizontalHitDistance = foundHorizontalWallHit ? distanceBetweenPoints(player->positionX, player->positionY, horizontalWallHitX, horizontalWallHitY) : INT_MAX;
    float verticalHitDistance = foundVerticalWallHit ? distanceBetweenPoints(player->positionX, player->positionY, verticalWallHitX, verticalWallHitY) : INT_MAX;

    if (verticalHitDistance < horizontalHitDistance) {
        ray->distance = verticalHitDistance;
        ray->wallHitX = verticalWallHitX;
        ray->wallHitY = verticalWallHitY;
        ray->wallHitContent = verticalWallContent;
        ray->wasHitVertical = true;
    } else {
        ray->distance = horizontalHitDistance;
        ray->wallHitX = horizontalWallHitX;
        ray->wallHitY = horizontalWallHitY;
        ray->wallHitContent = horizontalWallContent;
        ray->wasHitVertical = false;
    }
    ray->isRayDown = isRayDown;
    ray->isRayUp = isRayUp;
    ray->isRayLeft = isRayLeft;
    ray->isRayRight = isRayRight;
}

void castAllRays(Player* player, Ray* rays, Map* map) {
    float rayAngle = player->rotationAngle - (FOV_ANGLE / 2);

    for (int i = 0; i < NUM_RAYS; i++) {
        (rays + i)->rayAngle = rayAngle;
        castRay(player, (rays + i), map);
        rayAngle += FOV_ANGLE / NUM_RAYS;
    }
}

void renderRays(Player* player, Ray* rays, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < NUM_RAYS; i++) {
        SDL_RenderDrawLine(
            renderer,
            MAP_SCALE_FACTOR * player->positionX,
            MAP_SCALE_FACTOR * player->positionY,
            MAP_SCALE_FACTOR * (rays + i)->wallHitX,
            MAP_SCALE_FACTOR * (rays + i)->wallHitY
        );
    }
    
}