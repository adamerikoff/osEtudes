#include <stdio.h>

#include "engine/engine.h"

int main() {
    printf("Starting....\n");
    Ray rays[NUM_RAYS];

    Map* map = generateRoomWithColumns(MAP_ROWS, MAP_COLS, 25);
    Player* player = initializePlayer(WINDOW_HEIGHT/2, WINDOW_WIDTH/2, 5, 5);
    
    Engine* engine = initializeEngine("testing engine");

    engine->player = player;
    engine->map = map;
    engine->rays = rays;
    
    loopEngine(engine);    
    printf("Exiting....\n");
    return 0;
}
