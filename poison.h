#ifndef POISON_H
#define POISON_H

#include <SDL.h>
#include <tmxlite/TileLayer.hpp>
#include "Character.h"

class PoisonHandler {
public:
    bool loadHazardLayers(const std::vector<std::unique_ptr<tmx::Layer>>& layers);
    bool handle(Character& character, bool isFireboy, SDL_Renderer* renderer); // 3 tham số
private:
    const tmx::TileLayer* fireLayer = nullptr;
    const tmx::TileLayer* waterLayer = nullptr;
    const tmx::TileLayer* poisonLayer = nullptr;

    void fadeOut(Character& character, SDL_Renderer* renderer);
    bool isInHazardTile(Character& character, const tmx::TileLayer* hazardLayer);
};

#endif
