#include "MainMenu.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "MenuState.h"

MainMenu::MainMenu() : menuState(MenuState::MAIN_MENU),
                       font(nullptr),
                       fireboyFont(nullptr),
                       watergirlFont(nullptr),
                       fireboyTexture(nullptr),
                       watergirlTexture(nullptr),
                       playTexture(nullptr),
                       highscoreTexture(nullptr),
                       instructionsTexture(nullptr) {}

MainMenu::~MainMenu() {
    cleanup();
}

bool MainMenu::init(SDL_Renderer* renderer, TTF_Font* font, const std::vector<HighscoreEntry>& highscores) {
    this->renderer = renderer;
    this->font = font;

    // Tải font cho "Fireboy"
    fireboyFont = TTF_OpenFont("font/solemnity/solemnit.ttf", 80);
    if (!fireboyFont) {
        std::cerr << "[ERROR] Failed to load Fireboy font (solemnit.ttf): " << TTF_GetError() << "\n";
        return false;
    }

    // Tải font cho "Watergirl"
    watergirlFont = TTF_OpenFont("font/thayer-street-ndp-regular-font/thaysn.ttf", 80);
    if (!watergirlFont) {
        std::cerr << "[ERROR] Failed to load Watergirl font (thaysn.ttf): " << TTF_GetError() << "\n";
        TTF_CloseFont(fireboyFont);
        return false;
    }

    // Tạo texture cho "Fireboy" (màu đỏ)
    SDL_Color red = {255, 0, 0, 255};
    SDL_Surface* fireboySurface = TTF_RenderText_Solid(fireboyFont, "Fireboy", red);
    if (!fireboySurface) {
        std::cerr << "[ERROR] Failed to create Fireboy surface: " << TTF_GetError() << "\n";
        TTF_CloseFont(fireboyFont);
        TTF_CloseFont(watergirlFont);
        return false;
    }
    fireboyTexture = SDL_CreateTextureFromSurface(renderer, fireboySurface);
    SDL_FreeSurface(fireboySurface);
    if (!fireboyTexture) {
        std::cerr << "[ERROR] Failed to create Fireboy texture: " << SDL_GetError() << "\n";
        TTF_CloseFont(fireboyFont);
        TTF_CloseFont(watergirlFont);
        return false;
    }
    SDL_QueryTexture(fireboyTexture, nullptr, nullptr, &fireboyRect.w, &fireboyRect.h);
    fireboyRect.x = 85; // sát mép trái
    fireboyRect.y = 80;

    // Tạo texture cho "Watergirl"
    SDL_Color blue = {0,255,255};
    SDL_Surface* watergirlSurface = TTF_RenderText_Solid(watergirlFont, "Watergirl", blue);
    if (!watergirlSurface) {
        std::cerr << "[ERROR] Failed to create Watergirl surface: " << TTF_GetError() << "\n";
        TTF_CloseFont(fireboyFont);
        TTF_CloseFont(watergirlFont);
        SDL_DestroyTexture(fireboyTexture);
        return false;
    }
    watergirlTexture = SDL_CreateTextureFromSurface(renderer, watergirlSurface);
    SDL_FreeSurface(watergirlSurface);
    if (!watergirlTexture) {
        std::cerr << "[ERROR] Failed to create Watergirl texture: " << SDL_GetError() << "\n";
        TTF_CloseFont(fireboyFont);
        TTF_CloseFont(watergirlFont);
        SDL_DestroyTexture(fireboyTexture);
        return false;
    }
    SDL_QueryTexture(watergirlTexture, nullptr, nullptr, &watergirlRect.w, &watergirlRect.h);
    watergirlRect.x = 704 - watergirlRect.w-85; // Đặt "Watergirl" sát mép phải
    watergirlRect.y = 140; // Giữ "Watergirl" ở dưới "Fireboy"

    // Load play text
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Press Enter to Play", white);
    if (!playSurface) {
        std::cerr << "[ERROR] Failed to create play surface: " << TTF_GetError() << "\n";
        return false;
    }
    playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_FreeSurface(playSurface);
    if (!playTexture) {
        std::cerr << "[ERROR] Failed to create play texture: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_QueryTexture(playTexture, nullptr, nullptr, &playRect.w, &playRect.h);
    playRect.x = (704 - playRect.w) / 2;
    playRect.y = 280;

    // Load highscore text
    SDL_Surface* highscoreSurface = TTF_RenderText_Solid(font, "Press H for Highscore", white);
    if (!highscoreSurface) {
        std::cerr << "[ERROR] Failed to create highscore surface: " << TTF_GetError() << "\n";
        return false;
    }
    highscoreTexture = SDL_CreateTextureFromSurface(renderer, highscoreSurface);
    SDL_FreeSurface(highscoreSurface);
    if (!highscoreTexture) {
        std::cerr << "[ERROR] Failed to create highscore texture: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_QueryTexture(highscoreTexture, nullptr, nullptr, &highscoreRect.w, &highscoreRect.h);
    highscoreRect.x = (704 - highscoreRect.w) / 2;
    highscoreRect.y = 330;

    // Load instructions text
    SDL_Surface* instructionsSurface = TTF_RenderText_Solid(font, "Press I for Instructions", white);
    if (!instructionsSurface) {
        std::cerr << "[ERROR] Failed to create instructions surface: " << TTF_GetError() << "\n";
        return false;
    }
    instructionsTexture = SDL_CreateTextureFromSurface(renderer, instructionsSurface);
    SDL_FreeSurface(instructionsSurface);
    if (!instructionsTexture) {
        std::cerr << "[ERROR] Failed to create instructions texture: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_QueryTexture(instructionsTexture, nullptr, nullptr, &instructionsRect.w, &instructionsRect.h);
    instructionsRect.x = (704 - instructionsRect.w) / 2;
    instructionsRect.y = 380;

    // Khởi tạo Instructions
    if (!instructions.init(renderer, font)) {
        std::cerr << "[ERROR] Failed to initialize Instructions\n";
        return false;
    }

    // Khởi tạo Highscore
    if (!highscore.init(renderer, font, highscores)) {
        std::cerr << "[ERROR] Failed to initialize Highscore\n";
        return false;
    }

    return true;
}

void MainMenu::handleEvents(SDL_Event& event, bool& startGame) {
    if (event.type == SDL_KEYDOWN) {
        if (menuState == MenuState::MAIN_MENU) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                startGame = true;
            } else if (event.key.keysym.sym == SDLK_h) {
                menuState = MenuState::HIGHSCORE;
            } else if (event.key.keysym.sym == SDLK_i) {
                menuState = MenuState::INSTRUCTIONS;
            }
        } else if (menuState == MenuState::HIGHSCORE) {
            highscore.handleEvents(event, menuState);
        } else if (menuState == MenuState::INSTRUCTIONS) {
            instructions.handleEvents(event, menuState);
        }
    }

}

void MainMenu::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (menuState == MenuState::MAIN_MENU) {
        if (fireboyTexture) SDL_RenderCopy(renderer, fireboyTexture, nullptr, &fireboyRect);
        if (watergirlTexture) SDL_RenderCopy(renderer, watergirlTexture, nullptr, &watergirlRect);
        if (playTexture) SDL_RenderCopy(renderer, playTexture, nullptr, &playRect);
        if (highscoreTexture) SDL_RenderCopy(renderer, highscoreTexture, nullptr, &highscoreRect);
        if (instructionsTexture) SDL_RenderCopy(renderer, instructionsTexture, nullptr, &instructionsRect);
    } else if (menuState == MenuState::HIGHSCORE) {
        highscore.render(renderer);
    } else if (menuState == MenuState::INSTRUCTIONS) {
        instructions.render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void MainMenu::updateHighscore(SDL_Renderer* renderer, const std::vector<HighscoreEntry>& highscores) {
    highscore.updateScores(renderer, highscores);
}

void MainMenu::cleanup() {
    if (fireboyTexture) SDL_DestroyTexture(fireboyTexture);
    if (watergirlTexture) SDL_DestroyTexture(watergirlTexture);
    if (playTexture) SDL_DestroyTexture(playTexture);
    if (highscoreTexture) SDL_DestroyTexture(highscoreTexture);
    if (instructionsTexture) SDL_DestroyTexture(instructionsTexture);
    if (fireboyFont) TTF_CloseFont(fireboyFont);
    if (watergirlFont) TTF_CloseFont(watergirlFont);
}
