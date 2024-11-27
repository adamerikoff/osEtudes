#include "map.h"

// Function to generate an empty map with 1s on the borders and 0s inside
Map* generateEmptyMap(const unsigned int rows, const unsigned int columns) {
    // Allocate memory for the Map structure
    Map* map = (Map*)malloc(sizeof(Map));
    if (!map) {
        LOG_DEBUG("Failed to allocate memory for map structure.");
        return NULL;
    }

    // Set map dimensions
    map->rows = rows;
    map->columns = columns;

    // Allocate memory for the 2D array
    map->array = (int**)malloc(rows * sizeof(int*));
    if (!map->array) {
        LOG_DEBUG("Failed to allocate memory for map rows.");
        free(map);
        return NULL;
    }

    for (unsigned int i = 0; i < rows; i++) {
        map->array[i] = (int*)malloc(columns * sizeof(int));
        if (!map->array[i]) {
            LOG_DEBUG("Failed to allocate memory for map row %d.", i);

            // Free previously allocated rows if current allocation fails
            for (unsigned int j = 0; j < i; j++) {
                free(map->array[j]);
            }
            free(map->array);
            free(map);
            return NULL;
        }

        // Initialize cells in the row
        for (unsigned int j = 0; j < columns; j++) {
            if (i == 0 || i == rows - 1 || j == 0 || j == columns - 1) {
                map->array[i][j] = 1; // Border wall
            } else {
                map->array[i][j] = 0; // Open space inside
            }
        }
    }

    LOG_DEBUG("Empty map with borders created successfully.");
    return map;
}

// Function to generate a circular room in the middle of the map
Map* generateCircularRoomMap(const unsigned int rows, const unsigned int columns, const unsigned int radius) {
    Map* map = generateEmptyMap(rows, columns);
    if (!map) return NULL;

    unsigned int centerX = rows / 2;
    unsigned int centerY = columns / 2;

    // Create the circular room
    for (unsigned int i = 1; i < rows - 1; i++) {
        for (unsigned int j = 1; j < columns - 1; j++) {
            // Check if the point (i, j) lies within the circle using the circle equation
            if (pow(i - centerX, 2) + pow(j - centerY, 2) <= pow(radius, 2)) {
                map->array[i][j] = 1; // Inside the circle, make it a wall
            }
        }
    }

    LOG_DEBUG("Circular room generated successfully.\n");
    return map;
}

// Function to generate a room with columns (pillars inside the room)
Map* generateRoomWithColumns(const unsigned int rows, const unsigned int columns, const unsigned int numColumns) {
    Map* map = generateEmptyMap(rows, columns);
    if (!map) return NULL;

    unsigned int roomStartX = 1;
    unsigned int roomEndX = rows - 2;
    unsigned int roomStartY = 1;
    unsigned int roomEndY = columns - 2;

    // Randomly place columns inside the room
    for (unsigned int n = 0; n < numColumns; n++) {
        unsigned int columnX = rand() % (roomEndX - roomStartX + 1) + roomStartX;
        unsigned int columnY = rand() % (roomEndY - roomStartY + 1) + roomStartY;

        map->array[columnX][columnY] = 1; // Place a column (wall) at a random location inside the room
    }

    LOG_DEBUG("Room with columns generated successfully.\n");
    return map;
}

void renderMap(const Map* map, SDL_Renderer* renderer) {
    if (!map || !map->array) {
        fprintf(stderr, "Map or map data is NULL. Cannot render map.\n");
        return;
    }

    LOG_DEBUG("Rendering map with dimensions - Rows: %d, Columns: %d", map->rows, map->columns);

    for (unsigned int i = 0; i < map->rows; i++) {
        for (unsigned int j = 0; j < map->columns; j++) {
            // Calculate tile position in the renderer
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;

            // Set tile color: white (255) for walls, black (0) for open spaces
            int tileColor = (map->array[i][j] == 1) ? 255 : 0;

            // Set the color for the tile based on the map array
            SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);


            // Define the rectangle for the tile, with scaling applied
            SDL_Rect mapTileRect = {
                tileX * MAP_SCALE_FACTOR,
                tileY * MAP_SCALE_FACTOR,
                TILE_SIZE * MAP_SCALE_FACTOR,
                TILE_SIZE * MAP_SCALE_FACTOR,
            };

            // Render the tile
            SDL_RenderFillRect(renderer, &mapTileRect);

            // Optional debug log for each tile rendered
            // LOG_DEBUG("Rendered tile at (%d, %d) with color %d", i, j, tileColor);
        }
    }
}

// Function to free the memory allocated for the map
void destroyMap(Map* map) {
    if (!map) {
        LOG_DEBUG("Map is already NULL, nothing to destroy.");
        return;
    }

    // Free each row in the 2D array
    for (unsigned int i = 0; i < map->rows; i++) {
        free(map->array[i]);
    }

    // Free the array of row pointers and the Map structure
    free(map->array);
    free(map);

    LOG_DEBUG("Map memory successfully deallocated.");
}

bool isWall(Map* map, int x, int y) {
    if (map == NULL) {
        LOG_DEBUG("Map is NULL. Cannot check for wall.");
        return true;
    }
    int indexX = x / TILE_SIZE;
    int indexY = y / TILE_SIZE;
    // Check if the provided coordinates are within the map boundaries
    if (indexX < 0 || indexX > map->columns || indexY < 0 || indexY > map->rows) {
        LOG_DEBUG("Coordinates (%d, %d) are out of bounds. Map size is %d x %d.", x, y, map->columns, map->rows);
        return true;
    }

    // Return true if the tile is a wall, false if it is open space
    bool isWallTile = map->array[indexY][indexX] == 1;
    LOG_DEBUG("Tile at (%d, %d) is %s.", indexX, indexY, isWallTile ? "WALL" : "OPEN SPACE");
    return isWallTile;
}