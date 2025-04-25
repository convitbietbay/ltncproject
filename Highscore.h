#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "MenuState.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

struct HighscoreEntry {
    std::string name;
    float time;
};

class Highscore {
private:
    SDL_Renderer* renderer; // Thêm renderer
    TTF_Font* font;         // Thêm font
    std::vector<HighscoreEntry> scores; // Thêm scores

    SDL_Texture* titleTexture; // Thêm titleTexture
    SDL_Rect titleRect;        // Thêm titleRect
    SDL_Texture* backToMenuTexture; // Thêm backToMenuTexture
    SDL_Rect backToMenuRect;   // Thêm backToMenuRect
    SDL_Texture* scoreTextures[5]; // Thêm scoreTextures
    SDL_Rect scoreRects[5];    // Thêm scoreRects

    // Hàm chuyển đổi thời gian
    std::string formatTime(float time);

public:
    Highscore();
    ~Highscore();

    bool init(SDL_Renderer* renderer, TTF_Font* font, const std::vector<HighscoreEntry>& highscores);
    void handleEvents(SDL_Event& event, MenuState& menuState);
    void render(SDL_Renderer* renderer);
    void updateScores(SDL_Renderer* renderer, const std::vector<HighscoreEntry>& highscores);
    void cleanup(); // Thêm khai báo cleanup
};

#endif
