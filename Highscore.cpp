#include "Highscore.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <iomanip>

Highscore::Highscore() : renderer(nullptr), font(nullptr), titleTexture(nullptr) {
    for (int i = 0; i < 5; ++i) {
        scoreTextures[i] = nullptr;
    }
}

Highscore::~Highscore() {
    cleanup();
}

// Hàm chuyển đổi thời gian từ giây sang định dạng MM:SS
std::string Highscore::formatTime(float time) {
    int minutes = static_cast<int>(time / 60);
    int seconds = static_cast<int>(time - (minutes * 60)); // Bỏ phần thập phân

    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << minutes << ":"
       << std::setw(2) << std::setfill('0') << seconds;
    return ss.str();
}

bool Highscore::init(SDL_Renderer* renderer, TTF_Font* font, const std::vector<HighscoreEntry>& highscores) {
    this->renderer = renderer;
    this->font = font;
    this->scores = highscores;

    SDL_Color white = {255, 255, 255, 255};

    // Load title
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Highscore", white);
    if (!titleSurface) {
        std::cerr << "[ERROR] Failed to create highscore title surface: " << TTF_GetError() << "\n";
        return false;
    }
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_FreeSurface(titleSurface);
    if (!titleTexture) {
        std::cerr << "[ERROR] Failed to create highscore title texture: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_QueryTexture(titleTexture, nullptr, nullptr, &titleRect.w, &titleRect.h);
    titleRect.x = (704 - titleRect.w) / 2;
    titleRect.y = 150;

    // Load back to menu text
    SDL_Surface* backSurface = TTF_RenderText_Solid(font, "Press M to back to menu", white);
    if (!backSurface) {
        std::cerr << "[ERROR] Failed to create back to menu surface: " << TTF_GetError() << "\n";
        return false;
    }
    backToMenuTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
    SDL_FreeSurface(backSurface);
    if (!backToMenuTexture) {
        std::cerr << "[ERROR] Failed to create back to menu texture: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_QueryTexture(backToMenuTexture, nullptr, nullptr, &backToMenuRect.w, &backToMenuRect.h);
    backToMenuRect.x = (704 - backToMenuRect.w) / 2;
    backToMenuRect.y = 350; // Đặt y = 350 để nằm trong ô xám (trong khoảng 102 -> 402)

    updateScores(renderer, highscores);
    return true;
}

void Highscore::handleEvents(SDL_Event& event, MenuState& menuState) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m) {
        std::cout << "[INFO] Quay lại Main Menu từ Highscore.\n";
        menuState = MenuState::MAIN_MENU;
    }
}

void Highscore::render(SDL_Renderer* renderer) {
    // Vẽ viền vàng
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Vàng
    SDL_Rect borderRect = {100, 100, 504, 304};
    SDL_RenderDrawRect(renderer, &borderRect);

    // Vẽ nền xám đậm
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_Rect innerRect = {102, 102, 500, 300};
    SDL_RenderFillRect(renderer, &innerRect);

    // Vẽ tiêu đề
    if (titleTexture) {
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    } else {
        std::cerr << "[ERROR] titleTexture is null\n";
    }

    // Vẽ danh sách highscore
    for (int i = 0; i < 5; ++i) {
        if (scoreTextures[i]) {
            SDL_RenderCopy(renderer, scoreTextures[i], nullptr, &scoreRects[i]);
        }
    }

    // Vẽ dòng "Press M to back to menu"
    if (backToMenuTexture) {
        SDL_RenderCopy(renderer, backToMenuTexture, nullptr, &backToMenuRect);
    } else {
        std::cerr << "[ERROR] backToMenuTexture is null\n";
    }
}

void Highscore::updateScores(SDL_Renderer* renderer, const std::vector<HighscoreEntry>& highscores) {
    this->scores = highscores;

    SDL_Color white = {255, 255, 255, 255};
    for (int i = 0; i < 5; ++i) {
        if (scoreTextures[i]) {
            SDL_DestroyTexture(scoreTextures[i]);
            scoreTextures[i] = nullptr;
        }

        if (i < static_cast<int>(scores.size())) {
            // Chuyển đổi thời gian sang định dạng MM:SS
            std::string formattedTime = formatTime(scores[i].time);
            std::string scoreText = std::to_string(i + 1) + ". " + scores[i].name + " - " + formattedTime;

            SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
            if (!scoreSurface) {
                std::cerr << "[ERROR] Failed to create score surface for entry " << i << ": " << TTF_GetError() << "\n";
                continue;
            }
            scoreTextures[i] = SDL_CreateTextureFromSurface(renderer, scoreSurface);
            SDL_FreeSurface(scoreSurface);
            if (!scoreTextures[i]) {
                std::cerr << "[ERROR] Failed to create score texture for entry " << i << ": " << SDL_GetError() << "\n";
                continue;
            }
            SDL_QueryTexture(scoreTextures[i], nullptr, nullptr, &scoreRects[i].w, &scoreRects[i].h);
            scoreRects[i].x = (704 - scoreRects[i].w) / 2;
            scoreRects[i].y = 200 + i * 30;
        }
    }
}

void Highscore::cleanup() {
    if (titleTexture) SDL_DestroyTexture(titleTexture);
    if (backToMenuTexture) SDL_DestroyTexture(backToMenuTexture);
    for (int i = 0; i < 5; ++i) {
        if (scoreTextures[i]) SDL_DestroyTexture(scoreTextures[i]);
    }
}
