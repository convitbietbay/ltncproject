#define SDL_MAIN_HANDLED
#include "game.h"

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init()) return -1;

    game.run();
    game.cleanup();
    return 0;
}
