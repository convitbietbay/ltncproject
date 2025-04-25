#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL.h>
#include <vector>
#include <string>
#include <tmxlite/TileLayer.hpp>

struct JumpZone {
    SDL_Point from;
    SDL_Point to;
    float jumpForce;
    float velocityX;
};

class Character {
public:
    Character();
    ~Character();
    bool loadTexture(SDL_Renderer* renderer, const std::string& path);
    void setPosition(float x, float y);
    void setSize(int w, int h);
    void setMoveDirection(int dx);
    void setAlpha(Uint8 alpha);
    void setVelocity(float vx, float vy);
    void jump(bool force = false, bool checkGround = true);
    void update(float deltaTime, const tmx::TileLayer* solidLayer);
    void render(SDL_Renderer* renderer);
    std::vector<std::vector<int>> loadCSVLayer(const std::string& filename, int rows, int cols);
    bool isOnGroundFromCSV(float x, float y, int width, int height, const std::vector<std::vector<int>>& canStandLayer);
    int getCurrentStandingTileY();
    SDL_Point getCurrentStandingTile();
    void applyJumpProfileFromTile();
    SDL_Rect getRect() const;
    float findGroundLevel(float testX, const tmx::TileLayer* solidLayer);
    SDL_Texture* getTexture() const;

private:
    SDL_Texture* texture;
    float x, y;
    int width, height;
    float velocityX, velocityY;
    float velocityX_jump;
    bool onGround;
    float speed;
    float jumpForce;
    float gravity;
    float maxFallSpeed;
    bool movingLeft;
    bool movingRight;
    bool checkCollision(float testX, float testY, const tmx::TileLayer* solidLayer);
};

#endif
