#include "player.h"

Player* initializePlayer(float positionX, float positionY, float width, float height) {
    LOG_DEBUG("Initializing Player with position (%f, %f) and rectangle size w:%f, h:%f", positionX, positionY, width, height);

    Player* player = (Player*)malloc(sizeof(Player));
    if (!player) {
        fprintf(stderr, "Error allocating memory for Player struct.\n");
        return NULL;
    }
    LOG_DEBUG("Allocated memory for Player at %p", (void*)player);
    
    player->positionX = positionX;
    player->positionY = positionY;
    player->width = width;
    player->height = height;
    player->turnDirection = 0;
    player->walkDirection = 0;
    player->rotationAngle = - PI / 2;
    player->walkSpeed = 50;
    player->turnSpeed = 45 * (PI / 180);

    LOG_DEBUG("Player initialized successfully with position (%f, %f) and rectangle size w:%f, h:%f", positionX, positionY, width, height);

    return player;
}

void destroyPlayer(Player* player) {
    if (!player) {
        LOG_DEBUG("Attempted to destroy a NULL Player pointer.");
        return;
    }
    LOG_DEBUG("Freeing memory for Player at %p", (void*)player);
    free(player);
    LOG_DEBUG("Player memory freed.");
}

void renderPlayer(Player* player, SDL_Renderer* renderer) {
    if (!player || !renderer) {
        fprintf(stderr, "Error: Player or renderer is NULL in renderPlayer.\n");
        LOG_DEBUG("renderPlayer called with NULL parameter(s): player: %p, renderer: %p", (void*)player, (void*)renderer);
        return;
    }

    LOG_DEBUG("Rendering player at position (%f, %f) with rectangle size w:%f, h:%f", player->positionX, player->positionY, player->width, player->height);
    LOG_DEBUG("Rendering player at position (%f, %f) with angle %f", player->positionX, player->positionY, player->rotationAngle);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_Rect rect = {
        .x = player->positionX * MAP_SCALE_FACTOR,
        .y = player->positionY * MAP_SCALE_FACTOR,
        .w = player->width * MAP_SCALE_FACTOR,
        .h = player->height * MAP_SCALE_FACTOR,
    };

    if (SDL_RenderFillRect(renderer, &rect) != 0) {
        fprintf(stderr, "Error rendering player: %s\n", SDL_GetError());
        LOG_DEBUG("Failed to render player at position (%f, %f)", player->positionX, player->positionY);
        return;
    }

    LOG_DEBUG("Player rendered successfully at position (%f, %f)", player->positionX, player->positionY);
    
}

void processInputPlayer(Player* player, SDL_Event* event) {
    switch (event->type) 
    {
        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_UP) {
                player->walkDirection = +1;
            }
            if (event->key.keysym.sym == SDLK_DOWN) {
                player->walkDirection = -1;
            }
            if (event->key.keysym.sym == SDLK_RIGHT) {
                player->turnDirection = +1;
            }
            if (event->key.keysym.sym == SDLK_LEFT) {
                player->turnDirection = -1;
            }
            if (event->key.keysym.sym == SDLK_LSHIFT) {
                player->walkSpeed = 100;
            }
            break;
        case SDL_KEYUP:
            if (event->key.keysym.sym == SDLK_UP) {
                player->walkDirection = 0;
            }
            if (event->key.keysym.sym == SDLK_DOWN) {
                player->walkDirection = 0;
            }
            if (event->key.keysym.sym == SDLK_RIGHT) {
                player->turnDirection = 0;
            }
            if (event->key.keysym.sym == SDLK_LEFT) {
                player->turnDirection = 0;
            }
            if (event->key.keysym.sym == SDLK_LSHIFT) {
                player->walkSpeed = 50;
            }
            break;
        default:
            break;
    }
}

void updatePlayer(Player* player, Map* map, float deltaTime) {
    if (!player) {
        LOG_DEBUG("Attempted to update a NULL Player pointer.");
        return;
    }
    movePlayer(player, map, deltaTime);
}

void movePlayer(Player* player, Map* map, float deltaTime) {
    LOG_DEBUG("Updating player position. Current position: (%f, %f), deltaTime: %f", player->positionX, player->positionY, deltaTime);

    // Update the rotation angle based on the current turn direction and speed
    player->rotationAngle += player->turnDirection * player->turnSpeed * deltaTime;
    LOG_DEBUG("Updated rotation angle: %f radians", player->rotationAngle);

    // Calculate the step distance based on the walk direction and speed
    float moveStep = player->walkDirection * player->walkSpeed * deltaTime;
    LOG_DEBUG("Calculated move step: %f", moveStep);

    // Calculate the new position based on the rotation and movement
    float newX = player->positionX + cos(player->rotationAngle) * moveStep;
    float newY = player->positionY + sin(player->rotationAngle) * moveStep;
    LOG_DEBUG("Calculated new position: (%f, %f)", newX, newY);

    // Check for wall collision at the new position
    if (!isWall(map, (int)newX, (int)newY)) {
        // If there is no wall, update the player's position
        player->positionX = newX;
        player->positionY = newY;
        LOG_DEBUG("Player moved to new position: (%f, %f)", player->positionX, player->positionY);
    } else {
        // If there is a wall, log the collision event
        LOG_DEBUG("Collision detected at position (%f, %f). Player position remains at (%f, %f)", newX, newY, player->positionX, player->positionY);
    }
}