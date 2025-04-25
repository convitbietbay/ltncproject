#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include"MenuState.h"
class Instructions {
public:
    Instructions();
    ~Instructions();

    bool init(SDL_Renderer* renderer, TTF_Font* font);
    void handleEvents(SDL_Event& event, MenuState& menuState);
    void render(SDL_Renderer* renderer);

private:
    TTF_Font* font; //title font
    TTF_Font* smallFont; //  instruction font
    SDL_Texture* instructionsTitleTexture;
    SDL_Rect instructionsTitleRect;
    SDL_Texture* instructionsTextures[5];
    SDL_Rect instructionsRects[5];
    SDL_Texture* backToMenuTexture;
    SDL_Rect backToMenuRect;
    SDL_Rect instructionsBorder;  // viền
    SDL_Rect innerRect; // Inner rectangle
};
#endif
