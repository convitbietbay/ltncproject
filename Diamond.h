#pragma once
#include <SDL.h>
#include <tmxlite/TileLayer.hpp>
#include <vector>
#include"character.h"
enum class DiamondType { RED, BLUE };

struct Diamond {
    SDL_Rect rect;
    DiamondType type;
    bool collected;
    size_t tileIndex; // Lưu chỉ số tile trong layer
};

class DiamondHandler {
public:
    bool loadDiamonds(const tmx::TileLayer* redDiamondLayer, const tmx::TileLayer* blueDiamondLayer, int tileSize);
    bool checkCollision(Character& character, bool isFireboy, int& score, std::vector<tmx::TileLayer::Tile>& redDiamondTiles, std::vector<tmx::TileLayer::Tile>& blueDiamondTiles);
private:
    std::vector<Diamond> diamonds;
};
