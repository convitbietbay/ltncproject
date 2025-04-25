#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Timer {
public:
    Timer();
    ~Timer();

    bool init(SDL_Renderer* renderer, TTF_Font* font);
    void start();
    void update();
    void render(SDL_Renderer* renderer);
    void reset();

private:
    Uint32 startTime; // Thời gian bắt đầu (ms)
    SDL_Texture* timerTexture; // Texture hiển thị timer
    SDL_Rect timerRect; // Vị trí hiển thị
    SDL_Renderer* renderer; // Renderer từ Game
    TTF_Font* font; // Font từ Game (TrajanPro-Bold.otf)
};
