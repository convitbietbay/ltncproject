#include "poison.h"
#include <SDL.h>
#include <iostream>

bool PoisonHandler::loadHazardLayers(const std::vector<std::unique_ptr<tmx::Layer>>& layers) {
    for (const auto& layer : layers) {
        if (layer->getType() != tmx::Layer::Type::Tile) continue;
        const auto* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
        if (!tileLayer) continue;

        std::string name = tileLayer->getName();
        if (name == "Tile Layer 3:fire area") fireLayer = tileLayer;
        else if (name == "Tile Layer 7:water area") waterLayer = tileLayer;
        else if (name == "Tile Layer 8: hazard area") poisonLayer = tileLayer;
    }

    if (!fireLayer || !waterLayer || !poisonLayer) {
        std::cerr << "[ERROR] Layer không được load đúng! "
                  << (fireLayer ? "" : "fireLayer NULL, ")
                  << (waterLayer ? "" : "waterLayer NULL, ")
                  << (poisonLayer ? "" : "poisonLayer NULL")
                  << std::endl;
        return false;
    }

    return true;
}

bool PoisonHandler::isInHazardTile(Character& character, const tmx::TileLayer* layer) {
    if (!layer) {
        std::cerr << "[ERROR] Layer truyền vào bị null!\n";
        return false;
    }

    SDL_Point tile = character.getCurrentStandingTile();
    const auto& tiles = layer->getTiles();
    int width = layer->getSize().x;
    int height = layer->getSize().y;
    std::size_t index = tile.y * width + tile.x;

    if (tile.x < 0 || tile.y < 0 || tile.x >= width || tile.y >= height || index >= tiles.size()) {
        return false;
    }

    return tiles[index].ID != 0;
}

void PoisonHandler::fadeOut(Character& character, SDL_Renderer* renderer) {
    for (int alpha = 255; alpha >= 0; alpha -= 10) {
        character.setAlpha(static_cast<Uint8>(alpha));
        SDL_Delay(40);
        SDL_RenderClear(renderer);
        character.render(renderer);
        SDL_RenderPresent(renderer);
    }
}

bool PoisonHandler::handle(Character& character, bool isFireboy, SDL_Renderer* renderer) {
    if (!fireLayer || !waterLayer || !poisonLayer) {
        std::cerr << "[ERROR] Layer chưa được khởi tạo đúng!\n";
        return false;
    }

    SDL_Point tile = character.getCurrentStandingTile();

    if ((!isFireboy && isInHazardTile(character, fireLayer)) ||
        (isFireboy && isInHazardTile(character, waterLayer)) ||
        isInHazardTile(character, poisonLayer)) {
        fadeOut(character, renderer);
        SDL_Delay(1000);
        return true; // Nhân vật đã chết
    }

    return false;
}
