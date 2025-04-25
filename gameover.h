#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

class GameOver {
public:
    GameOver();
    ~GameOver();

    bool init(SDL_Renderer* renderer);
    void handleEvents(bool& gameOverRunning, bool& restart);
    void render(SDL_Renderer* renderer);
    void cleanup();

private:
    TTF_Font* font;
    SDL_Texture* gameOverTexture;
    SDL_Rect gameOverRect;
    SDL_Texture* instructionTexture;
    SDL_Rect instructionRect;
    SDL_Texture* mainMenuTexture;
    SDL_Rect mainMenuRect;
};
