#include "Instructions.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "MenuState.h"

Instructions::Instructions() : font(nullptr), smallFont(nullptr), instructionsTitleTexture(nullptr), backToMenuTexture(nullptr) {
    for (auto& texture : instructionsTextures) {
        texture = nullptr;
    }
}

Instructions::~Instructions() {
    if (instructionsTitleTexture) SDL_DestroyTexture(instructionsTitleTexture);
    if (backToMenuTexture) SDL_DestroyTexture(backToMenuTexture);
    for (auto texture : instructionsTextures) {
        if (texture) SDL_DestroyTexture(texture);
    }
    if (smallFont) TTF_CloseFont(smallFont);
}

bool Instructions::init(SDL_Renderer* renderer, TTF_Font* mainFont) {
    this->font = mainFont;

    // Font cho instruction
    smallFont = TTF_OpenFont("font/TrajanPro-Bold.otf", 20);
    if (!smallFont) {
        std::cerr << "Failed to load small font for Instructions: " << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Color white = {255, 255, 255, 255};

    // Texture cho tiêu đề "INSTRUCTION"
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "INSTRUCTION", white);
    if (!titleSurface) {
        std::cerr << "Failed to create Instructions title surface: " << TTF_GetError() << std::endl;
        return false;
    }
    instructionsTitleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_FreeSurface(titleSurface);
    if (!instructionsTitleTexture) {
        std::cerr << "Failed to create Instructions title texture: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_QueryTexture(instructionsTitleTexture, nullptr, nullptr, &instructionsTitleRect.w, &instructionsTitleRect.h);
    instructionsTitleRect.x = (704 - instructionsTitleRect.w) / 2;
    instructionsTitleRect.y = 84;

    // Tạo viền
    instructionsBorder = {52, 64, 600, 400};

    // Vùng ghi hướng dẫn
    int innerWidth = static_cast<int>(instructionsBorder.w * 0.8);
    int innerHeight = static_cast<int>(instructionsBorder.h * 0.8);
    innerRect = {
        instructionsBorder.x + (instructionsBorder.w - innerWidth) / 2,
        instructionsBorder.y + (instructionsBorder.h - innerHeight) / 2,
        innerWidth,
        innerHeight
    };

    // Hướng dẫn
    const char* instructions[] = {
        "Fireboy moves with WAD keys.",
        "Watergirl moves with arrow keys.",
        "Collect diamonds of your color.",
        "Avoid hazards and opposite pools.",
        "Work together to reach the doors!"
    };
    int totalHeight = 5 * 20 + 4 * 10; // 5 dòng, mỗi dòng cao ~20, cách nhau 10 pixel
    int yPos = innerRect.y + (innerRect.h - totalHeight) / 2; // Căn giữa dọc trong innerRect
    for (int i = 0; i < 5; ++i) {
        SDL_Surface* surface = TTF_RenderText_Solid(smallFont, instructions[i], white);
        if (!surface) {
            std::cerr << "Failed to create instruction surface: " << TTF_GetError() << std::endl;
            return false;
        }
        instructionsTextures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!instructionsTextures[i]) {
            return false;
        }

        SDL_QueryTexture(instructionsTextures[i], nullptr, nullptr, &instructionsRects[i].w, &instructionsRects[i].h);
        instructionsRects[i].x = innerRect.x + (innerRect.w - instructionsRects[i].w) / 2; // Căn giữa ngang trong innerRect
        instructionsRects[i].y = yPos;
        yPos += instructionsRects[i].h + 10; // Khoảng cách dòng
    }

    // "Press M to back to menu"
    SDL_Surface* backSurface = TTF_RenderText_Solid(smallFont, "Press M to back to menu", white);
    if (!backSurface) {
        std::cerr << "Failed to create back to menu surface: " << TTF_GetError() << std::endl;
        return false;
    }
    backToMenuTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
    SDL_FreeSurface(backSurface);
    if (!backToMenuTexture) {
        return false;
    }
    SDL_QueryTexture(backToMenuTexture, nullptr, nullptr, &backToMenuRect.w, &backToMenuRect.h);
    backToMenuRect.x = (704 - backToMenuRect.w) / 2;
    backToMenuRect.y = instructionsBorder.y + instructionsBorder.h - backToMenuRect.h - 40; // Cách đáy khung 40 pixel

    return true;
}

void Instructions::handleEvents(SDL_Event& event, MenuState& menuState) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_m:
                menuState = MenuState::MAIN_MENU; // Updated to match enum
                break;
        }
    }
}

void Instructions::render(SDL_Renderer* renderer) {
    // Viền
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Vàng
    SDL_RenderDrawRect(renderer, &instructionsBorder);
    // Nền trong
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Xám đậm
    SDL_RenderFillRect(renderer, &innerRect);
    // Tiêu đề
    SDL_RenderCopy(renderer, instructionsTitleTexture, nullptr, &instructionsTitleRect);
    // Hướng dẫn
    for (int i = 0; i < 5; ++i) {
        SDL_RenderCopy(renderer, instructionsTextures[i], nullptr, &instructionsRects[i]);
    }
    // "Press M to back to menu"
    SDL_RenderCopy(renderer, backToMenuTexture, nullptr, &backToMenuRect);
}
