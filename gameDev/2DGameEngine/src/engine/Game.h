#ifndef GAME_H
#define GAME_H

#include "includes.h"

class Game {
private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
public:
    Game();
    ~Game();
    
    void Initialize();
    void Setup();

    void ProcessInput();
    void Update();
    void Render();

    void Destroy();

    void Run();

    int windowHeight;
    int windowWidth;
};

#endif
