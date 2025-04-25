#include "Character.h"
#include <SDL_image.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <tmxlite/TileLayer.hpp>
#include <cmath>

static std::vector<JumpZone> jumpZones = {
    {{7, 30},  {15, 30},  200.0f,  100.0f},
    {{15, 30}, {7, 30},   200.0f, -100.0f},

    {{18, 30}, {27, 30},  200.0f,  120.0f},
    {{27, 30}, {18, 30},  200.0f, -120.0f},

    {{17, 30}, {27, 30},  200.0f,  120.0f},
    {{27, 30}, {17, 30},  200.0f, -120.0f},

    {{34, 30}, {38, 27},  450.0f,  140.0f},
    {{38, 27}, {34, 30},  450.0f, -140.0f},

    {{35, 30}, {38, 27},  450.0f,  140.0f},
    {{38, 27}, {35, 30},  450.0f, -140.0f},

    {{38, 27}, {40, 27},  350.0f,  90.0f},
    {{40, 27}, {38, 27},  350.0f, -90.0f},

    {{36, 22}, {34, 22},  300.0f, -90.0f},
    {{34, 22}, {36, 22},  300.0f,  90.0f},

    {{39, 22}, {35, 22},  300.0f, -100.0f},
    {{35, 22}, {39, 22},  300.0f,  100.0f},

    {{40, 22}, {36, 22},  300.0f, -100.0f},
    {{36, 22}, {40, 22},  300.0f,  100.0f},

    {{15, 22}, {11, 20},  400.0f, -120.0f},
    {{11, 20}, {15, 22},  400.0f,  120.0f},

    {{13, 22}, {10, 20},  400.0f, -120.0f},
    {{10, 20}, {13, 22},  400.0f,  120.0f},

    {{5, 20},  {9, 14},   300.0f,  140.0f},
    {{9, 14},  {5, 20},   300.0f, -140.0f},

    {{13, 14}, {23, 14},  250.0f,  170.0f},
    {{23, 14}, {13, 14},  250.0f, -170.0f},

    {{24, 14}, {7, 12},   500.0f, -150.0f},
    {{7, 12},  {24, 14},  500.0f,  150.0f}
};

Character::Character() {
    x = y = 0;
    width = height = 0;
    velocityX = velocityY = 0;
    onGround = false;
    texture = nullptr;

    speed = 100.0f;
    jumpForce = 150.0f;
    gravity = 500.0f;
    maxFallSpeed = 400.0f;

    movingLeft = false;
    movingRight = false;
    velocityX_jump = 0; // Khởi tạo velocityX_jump
}

Character::~Character() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

bool Character::loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load character image: " << path << " Error: " << IMG_GetError() << std::endl;
        return false;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture != nullptr;
}

void Character::setPosition(float px, float py) {
    x = px;
    y = py;
}

void Character::setSize(int w, int h) {
    width = w;
    height = h;
}

void Character::setMoveDirection(int dx) {
    movingLeft = (dx < 0);
    movingRight = (dx > 0);
}

bool Character::checkCollision(float testX, float testY, const tmx::TileLayer* solidLayer) {
    const int tileSize = 16;
    const auto& tiles = solidLayer->getTiles();
    int mapWidth = solidLayer->getSize().x;
    int mapHeight = solidLayer->getSize().y;

    int left = static_cast<int>(testX) / tileSize;
    int right = static_cast<int>(testX + width - 1) / tileSize;
    int top = static_cast<int>(testY) / tileSize;
    int bottom = static_cast<int>(testY + height - 1) / tileSize;

    if (left < 0 || right >= mapWidth || top < 0 || bottom >= mapHeight) {
        return true;
    }

    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            std::size_t index = y * mapWidth + x;
            if (index < tiles.size() && tiles[index].ID != 0) {
                return true;
            }
        }
    }

    return false;
}

float Character::findGroundLevel(float testX, const tmx::TileLayer* solidLayer) {
    const auto& tiles = solidLayer->getTiles();
    int mapWidth = solidLayer->getSize().x;
    int mapHeight = solidLayer->getSize().y;

    int leftTile = static_cast<int>(testX) / 16;
    int rightTile = static_cast<int>(testX + width - 1) / 16;
    float lowestGround = static_cast<float>(mapHeight * 16);

    for (int col = leftTile; col <= rightTile; col++) {
        if (col < 0 || col >= mapWidth) continue;

        for (int row = static_cast<int>(y + height) / 16; row < mapHeight; row++) {
            std::size_t index = row * mapWidth + col;
            if (index < tiles.size()) {
                if (tiles[index].ID != 0) {
                    float groundAtThisPoint = row * 16;
                    lowestGround = std::min(lowestGround, groundAtThisPoint);
                    break;
                }
            }
        }
    }
    return lowestGround - height;
}

void Character::update(float deltaTime, const tmx::TileLayer* solidLayer) {
    velocityX = 0;
    if (movingLeft) velocityX = -speed;
    else if (movingRight) velocityX = speed;

    velocityY += gravity * deltaTime;
    if (velocityY > maxFallSpeed) velocityY = maxFallSpeed;
    float totalVelocityX = velocityX + velocityX_jump;
    float newX = x + totalVelocityX * deltaTime;

    if (!checkCollision(newX, y, solidLayer)) {
        x = newX;
    }

    float newY = y + velocityY * deltaTime;
    if (!checkCollision(x, newY, solidLayer)) {
        y = newY;
        onGround = false;
    } else {
        if (velocityY > 0) {
            onGround = true;
            velocityX_jump = 0;
        }
        velocityY = 0;
    }
    SDL_Point tile = getCurrentStandingTile();
}

void Character::render(SDL_Renderer* renderer) {
    SDL_Rect destRect = {
        static_cast<int>(x), static_cast<int>(y),
        width, height
    };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}

std::vector<std::vector<int>> Character::loadCSVLayer(const std::string& filename, int rows, int cols) {
    std::vector<std::vector<int>> layer(rows, std::vector<int>(cols));
    std::ifstream file(filename);
    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < rows) {
        std::stringstream ss(line);
        std::string cell;
        int col = 0;
        while (std::getline(ss, cell, ',') && col < cols) {
            layer[row][col] = std::stoi(cell);
            ++col;
        }
        ++row;
    }
    return layer;
}

bool Character::isOnGroundFromCSV(float x, float y, int width, int height, const std::vector<std::vector<int>>& canStandLayer) {
    int tileSize = 16;
    int left = static_cast<int>(x) / tileSize;
    int right = static_cast<int>(x + width - 1) / tileSize;
    int bottom = static_cast<int>(y + height) / tileSize;

    if (bottom >= static_cast<int>(canStandLayer.size())) return false;

    for (int i = left; i <= right; ++i) {
        if (i >= 0 && i < static_cast<int>(canStandLayer[0].size())) {
            if (canStandLayer[bottom][i] != 0) {
                return true;
            }
        }
    }
    return false;
}

void Character::jump(bool force, bool checkGround) {
    if (onGround || !checkGround || force) {
        applyJumpProfileFromTile();
        velocityY = -jumpForce;
        velocityX_jump = velocityX;
        onGround = false;
    }
}

int Character::getCurrentStandingTileY() {
    float bottomY = y + height;
    int tileY = static_cast<int>(bottomY / 16.0f);

    float offset = fmod(bottomY, 16.0f);
    if (offset <= 2.0f) {
        tileY -= 1;
    }

    return tileY;
}

SDL_Point Character::getCurrentStandingTile() {
    float bottomY = y + height;
    float centerX = x + width / 2.0f;

    int tileX = static_cast<int>(centerX / 16.0f);
    int tileY = static_cast<int>(bottomY / 16.0f);

    float offsetY = fmod(bottomY, 16.0f);
    if (offsetY <= 2.0f) {
        tileY -= 1;
    }

    return SDL_Point{ tileX, tileY };
}

void Character::applyJumpProfileFromTile() {
    SDL_Point current = getCurrentStandingTile();

    jumpForce = 320.0f;
    velocityX = 0.0f;

    if (movingLeft) velocityX = -100.0f;
    else if (movingRight) velocityX = 100.0f;

    for (const auto& zone : jumpZones) {
        if (current.x == zone.from.x && current.y == zone.from.y) {
            if (!movingLeft && !movingRight && zone.velocityX != 0) continue;
            if (movingLeft && zone.velocityX > 0) continue;
            if (movingRight && zone.velocityX < 0) continue;

            jumpForce = zone.jumpForce;
            velocityX = zone.velocityX;
            return;
        }
    }
}

void Character::setAlpha(Uint8 alpha) {
    if (texture) {
        SDL_SetTextureAlphaMod(texture, alpha);
    }
}

void Character::setVelocity(float vx, float vy) {
    velocityX = vx;
    velocityY = vy;
    velocityX_jump = 0; // Đặt lại velocityX_jump để đảm bảo không có lực ngang từ jump zone
}

SDL_Rect Character::getRect() const {
    return {static_cast<int>(x), static_cast<int>(y), width, height};
}

SDL_Texture* Character::getTexture() const {
    return texture;
}
