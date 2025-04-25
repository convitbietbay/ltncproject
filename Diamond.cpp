#include "Diamond.h"
#include <SDL.h>
#include <tmxlite/TileLayer.hpp>
#include <vector>
#include <iostream>

bool DiamondHandler::loadDiamonds(const tmx::TileLayer* redDiamondLayer, const tmx::TileLayer* blueDiamondLayer, int tileSize) {
    diamonds.clear();

    if (redDiamondLayer) {
        const auto& tiles = redDiamondLayer->getTiles();
        int width = redDiamondLayer->getSize().x;
        for (size_t i = 0; i < tiles.size(); ++i) {
            if (tiles[i].ID != 0) {
                int x = (i % width) * tileSize;
                int y = (i / width) * tileSize;
                diamonds.push_back({{x, y, tileSize, tileSize}, DiamondType::RED, false, i});
            }
        }
    }

    if (blueDiamondLayer) {
        const auto& tiles = blueDiamondLayer->getTiles();
        int width = blueDiamondLayer->getSize().x;
        for (size_t i = 0; i < tiles.size(); ++i) {
            if (tiles[i].ID != 0) {
                int x = (i % width) * tileSize;
                int y = (i / width) * tileSize;
                diamonds.push_back({{x, y, tileSize, tileSize}, DiamondType::BLUE, false, i});
            }
        }
    }

    return !diamonds.empty();
}

bool DiamondHandler::checkCollision(Character& character, bool isFireboy, int& score, std::vector<tmx::TileLayer::Tile>& redDiamondTiles, std::vector<tmx::TileLayer::Tile>& blueDiamondTiles) {
    SDL_Rect charRect = character.getRect();
    std::vector<tmx::TileLayer::Tile>& tiles = isFireboy ? redDiamondTiles : blueDiamondTiles;
    DiamondType targetType = isFireboy ? DiamondType::RED : DiamondType::BLUE;

    bool collected = false;
    for (auto it = diamonds.begin(); it != diamonds.end();) {
        if (it->type == targetType && !it->collected && SDL_HasIntersection(&charRect, &it->rect)) {
            score += 10;
            it->collected = true;
            tiles[it->tileIndex].ID = 0;
            it = diamonds.erase(it);
            collected = true;
        } else {
            ++it;
        }
    }
    return collected;
}
