#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include <string>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include "Character.h"
#include "poison.h"
#include "diamond.h"
#include "MainMenu.h"
#include "GameOver.h"
#include "Timer.h"
#include "MenuState.h"
#include "highscore.h"

const int SCREEN_WIDTH = 704;
const int SCREEN_HEIGHT = 528;
const int TILE_SIZE = 16;

enum class GameState { MENU, PLAYING, GAME_OVER, INPUT_NAME };

struct Tileset {
    int firstGID;
    int columns;
    SDL_Texture* texture;
};

class Game {
public:
    Game();
    ~Game();
    bool init();
    void handleEvents(bool& running);
    void update(float deltaTime, bool& running);
    void render();
    void run();
    void cleanup();
    const std::vector<HighscoreEntry>& getHighscores() const { return highscores; }
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    tmx::Map map;
    std::vector<Tileset> tilesets;
    const tmx::TileLayer* collisionLayer;
    const tmx::TileLayer* redDiamondLayer;
    const tmx::TileLayer* blueDiamondLayer;
    std::vector<tmx::TileLayer::Tile> redDiamondTiles;
    std::vector<tmx::TileLayer::Tile> blueDiamondTiles;
    Character fireboy;
    Character watergirl;
    DiamondHandler diamondHandler;
    TTF_Font* scoreFont;
    SDL_Texture* fireboyScoreTexture;
    SDL_Texture* watergirlScoreTexture;
    SDL_Rect fireboyScoreRect;
    SDL_Rect watergirlScoreRect;
    int fireboyScore;
    int watergirlScore;
    std::vector<HighscoreEntry> highscores; // Danh sách highscore
    float gameTime; // Thời gian chơi
    Timer timer;
    GameState gameState;
    MainMenu mainMenu;
    GameOver gameover;
    bool isGameOver;
    bool restart;
    std::string playerName; // Tên người chơi nhập vào
    SDL_Texture* inputPromptTexture;
    SDL_Rect inputPromptRect;
    SDL_Texture* nameTexture;
    SDL_Rect nameRect;
    bool isWin; // Trạng thái thắng để kiểm soát lưu highscore
    SDL_Texture* winMessageTexture; // Texture cho thông báo "You Win"
    SDL_Rect winMessageRect; // Vị trí hiển thị "You Win"

    SDL_Texture* loadTexture(const std::string& path);
    bool loadMap(const std::string& mapPath);
    bool loadTilesets();
    bool loadCharacters();
    bool findCollisionLayer();
    bool findDiamondLayers();
    bool initScoreDisplay();
    void updateScoreDisplay();
    void renderMap();
    void saveHighscores();
    void loadHighscores();
    void handleNameInput(SDL_Event& event, bool& running);
    void updateNameTexture();
    void renderNameInput();
};

#endif
