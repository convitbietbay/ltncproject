#include "GameOver.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>

GameOver::GameOver() : font(nullptr), gameOverTexture(nullptr), instructionTexture(nullptr), mainMenuTexture(nullptr) {
}

GameOver::~GameOver() {
    cleanup();
}

bool GameOver::init(SDL_Renderer* renderer) {
    font = TTF_OpenFont("font/TrajanPro-Bold.otf", 32);
    if (!font) {
        std::cerr << "Failed to load GameOver font: " << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Color white = {205, 0, 0};

    // Tạo texture cho "GAME OVER"
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "GAME END", white);
    if (!gameOverSurface) {
        std::cerr << "Failed to create GameOver surface: " << TTF_GetError() << std::endl;
        return false;
    }
    gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_FreeSurface(gameOverSurface);
    SDL_QueryTexture(gameOverTexture, nullptr, nullptr, &gameOverRect.w, &gameOverRect.h);
    gameOverRect.x = (704 - gameOverRect.w) / 2;
    gameOverRect.y = 200;

    // Tạo texture cho "PRESS P TO REPLAY"
    SDL_Surface* instructionSurface = TTF_RenderText_Solid(font, "PRESS P TO REPLAY", white);
    if (!instructionSurface) {
        std::cerr << "Failed to create instruction surface: " << TTF_GetError() << std::endl;
        return false;
    }
    instructionTexture = SDL_CreateTextureFromSurface(renderer, instructionSurface);
    SDL_FreeSurface(instructionSurface);
    SDL_QueryTexture(instructionTexture, nullptr, nullptr, &instructionRect.w, &instructionRect.h);
    instructionRect.x = (704 - instructionRect.w) / 2; // Căn giữa ngang
    instructionRect.y = 264 - instructionRect.h; // Căn giữa dọc (528 / 2 = 264)

    // Tạo texture cho "ANY OTHER KEY TO MAIN MENU"
    SDL_Surface* mainMenuSurface = TTF_RenderText_Solid(font, "ANY OTHER KEY TO MAIN MENU", white);
    if (!mainMenuSurface) {
        std::cerr << "Failed to create Main Menu surface: " << TTF_GetError() << std::endl;
        return false;
    }
    mainMenuTexture = SDL_CreateTextureFromSurface(renderer, mainMenuSurface);
    SDL_FreeSurface(mainMenuSurface);
    SDL_QueryTexture(mainMenuTexture, nullptr, nullptr, &mainMenuRect.w, &mainMenuRect.h);
    mainMenuRect.x = (704 - mainMenuRect.w) / 2;
    mainMenuRect.y = 264;

    return true;
}

void GameOver::handleEvents(bool& gameOverRunning, bool& restart) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gameOverRunning = false;
            restart = false;
            return;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_p) {
                restart = true; // Replay game
                gameOverRunning = false;
            } else {
                restart = false; // Quay lại Main Menu
                gameOverRunning = false;
            }
        }
    }
}

void GameOver::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
    SDL_RenderCopy(renderer, instructionTexture, nullptr, &instructionRect);
    SDL_RenderCopy(renderer, mainMenuTexture, nullptr, &mainMenuRect);
    SDL_RenderPresent(renderer);
}

void GameOver::cleanup() {
    if (gameOverTexture) SDL_DestroyTexture(gameOverTexture);
    if (instructionTexture) SDL_DestroyTexture(instructionTexture);
    if (mainMenuTexture) SDL_DestroyTexture(mainMenuTexture);
    if (font) TTF_CloseFont(font);
    gameOverTexture = nullptr;
    instructionTexture = nullptr;
    mainMenuTexture = nullptr;
    font = nullptr;
}
