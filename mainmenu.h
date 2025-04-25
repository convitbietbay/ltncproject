#ifndef MAINMENU_H
#define MAINMENU_H
#include "Highscore.h"
#include "Instructions.h"
#include "MenuState.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

class MainMenu {
private:
    MenuState menuState;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* fireboyFont;
    TTF_Font* watergirlFont;
    Instructions instructions;
    Highscore highscore;

    SDL_Texture* fireboyTexture;
    SDL_Rect fireboyRect;
    SDL_Texture* watergirlTexture;
    SDL_Rect watergirlRect;
    SDL_Texture* playTexture;
    SDL_Rect playRect;
    SDL_Texture* highscoreTexture;
    SDL_Rect highscoreRect;
    SDL_Texture* instructionsTexture;
    SDL_Rect instructionsRect;

public:
    MainMenu();
    ~MainMenu();

    bool init(SDL_Renderer* renderer, TTF_Font* font, const std::vector<HighscoreEntry>& highscores);
    void handleEvents(SDL_Event& event, bool& startGame);
    void render(SDL_Renderer* renderer);
    void updateHighscore(SDL_Renderer* renderer, const std::vector<HighscoreEntry>& highscores);
    void cleanup();
};

#endif
