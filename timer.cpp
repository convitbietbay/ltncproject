#include "Timer.h"
#include <iostream>

Timer::Timer() : startTime(0), timerTexture(nullptr), renderer(nullptr), font(nullptr) {
    timerRect = {540, 20, 0, 0}; // Vị trí (10, 70)
}

Timer::~Timer() {
    if (timerTexture) SDL_DestroyTexture(timerTexture);
}

bool Timer::init(SDL_Renderer* renderer_, TTF_Font* font_) {
    renderer = renderer_;
    font = font_;
    if (!font) {
        std::cerr << "Timer init failed: Font is null\n";
        return false;
    }
    if (!renderer) {
        std::cerr << "Timer init failed: Renderer is null\n";
        return false;
    }
    start();
    update(); // Tạo texture ban đầu
    return true;
}

void Timer::start() {
    startTime = SDL_GetTicks();
}

void Timer::update() {
    if (timerTexture) SDL_DestroyTexture(timerTexture);

    Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000; // Giây
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    char timerText[16];
    snprintf(timerText, sizeof(timerText), "TIME: %02d:%02d", minutes, seconds);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* timerSurface = TTF_RenderText_Solid(font, timerText, white);
    if (!timerSurface) {
        std::cerr << "Failed to create timer surface: " << TTF_GetError() << "\n";
        return;
    }
    timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
    SDL_FreeSurface(timerSurface);
    SDL_QueryTexture(timerTexture, nullptr, nullptr, &timerRect.w, &timerRect.h);
}

void Timer::render(SDL_Renderer* renderer) {
    if (timerTexture) {
        SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);
    }
}

void Timer::reset() {
    startTime = SDL_GetTicks();
    update();
}
