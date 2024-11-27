#include "engine/Game.h"

int main() {
    std::cout << "Started..." << std::endl;
    Game game;

    game.Initialize();
    game.Run();
    game.Destroy();

    std::cout << "Exited..." << std::endl;
    return 0;
}
