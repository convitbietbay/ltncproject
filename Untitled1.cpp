#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

const int TILE_SIZE = 16;  // 📝 Tileset của em là 16x16, không phải 32x32!
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE2=16;

// Hàm đọc CSV file
std::vector<std::vector<int>> loadCSVMap(const std::string& filename) {
    std::vector<std::vector<int>> mapData;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "❌ Không thể mở file: " << filename << std::endl;
        return mapData;
    }

    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string tileID;

        while (std::getline(ss, tileID, ',')) {
            if (!tileID.empty()) {
                row.push_back(std::stoi(tileID));
            }
        }

        mapData.push_back(row);
    }

    std::cout << "✅ Đã load map từ file: " << filename << " (rows: " << mapData.size() << ")" << std::endl;
    return mapData;
}

int main(int argc, char* argv[]) {
    // ======== Khởi tạo SDL ========
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "❌ SDL không khởi tạo được: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "❌ SDL_image không khởi tạo được: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Render 2 Layers with Tileset",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    if (!window) {
        std::cerr << "❌ Không tạo được cửa sổ: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "❌ Không tạo được renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // ======== Load tileset PNG ========
    SDL_Surface* tilesetSurface2 = IMG_Load("background_1a.png");
    SDL_Surface* tilesetSurface = IMG_Load("tileset.png");

    if (!tilesetSurface) {
        std::cerr << "❌ Không load được tileset PNG! " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* tilesetTexture = SDL_CreateTextureFromSurface(renderer, tilesetSurface);
    SDL_Texture* tilesetTexture2 = SDL_CreateTextureFromSurface(renderer, tilesetSurface2);
    if (!tilesetTexture) {
        std::cerr << "❌ Không tạo được texture từ surface! " << SDL_GetError() << std::endl;
        SDL_FreeSurface(tilesetSurface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    int tilesetColumns = tilesetSurface->w / TILE_SIZE;
    int tilesetRows = tilesetSurface->h / TILE_SIZE;  // làm nốt đoạn này, nhớ phải thay dổi tile size cho phù hợp với surface2
    int tilesetColumns2=tilesetSurface2->w/TILE_SIZE2;
    int tilesetRows2=tilesetSurface2->w/TILE_SIZE2;

    std::cout << "✅ Tileset loaded: " << tilesetSurface->w << "x" << tilesetSurface->h
              << " (" << tilesetColumns << " columns, " << tilesetRows << " rows)" << std::endl;

    SDL_FreeSurface(tilesetSurface);  // Không cần giữ surface sau khi tạo texture. // FRee surface2 sau khi dùng xong
    SDL_FreeSurface(tilesetSurface2);
    // ======== Load các layer từ CSV ========
    auto backgroundLayer = loadCSVMap("huhu1.csv");
    auto blockLayer = loadCSVMap("block.csv");

    if (backgroundLayer.empty() || blockLayer.empty()) {
        std::cerr << "❌ Một trong hai layer rỗng!" << std::endl;
        SDL_DestroyTexture(tilesetTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // ======== Vòng lặp game ========
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // ===== Render background layer ===== // Dùng các thông số tilesetColumns,Rows, Size của suface2
        for (int row = 0; row < backgroundLayer.size(); row++) {
            for (int col = 0; col < backgroundLayer[row].size(); col++) {
                int tileID = backgroundLayer[row][col];
                if (tileID <= 0) continue;  // Skip tile trống

                tileID -= 1;  // ID trong Tiled bắt đầu từ 1

                SDL_Rect srcRect;
        srcRect.x = (tileID % tilesetColumns2) * TILE_SIZE2;
        srcRect.y = (tileID / tilesetColumns2) * TILE_SIZE2;
        srcRect.w = TILE_SIZE2;
        srcRect.h = TILE_SIZE2;

        SDL_Rect destRect;
        destRect.x = col * TILE_SIZE2;
        destRect.y = row * TILE_SIZE2;
        destRect.w = TILE_SIZE2;
        destRect.h = TILE_SIZE2;

  SDL_RenderCopy(renderer, tilesetTexture2, &srcRect, &destRect);
            }
        }

        // ===== Render block layer =====
        for (int row = 0; row < blockLayer.size(); row++) {
            for (int col = 0; col < blockLayer[row].size(); col++) {
                int tileID = blockLayer[row][col];
                if (tileID <= 0) continue;

                tileID -= 1;

                SDL_Rect srcRect;
                srcRect.x = (tileID % tilesetColumns) * TILE_SIZE;
                srcRect.y = (tileID / tilesetColumns) * TILE_SIZE;
                srcRect.w = TILE_SIZE;
                srcRect.h = TILE_SIZE;

                SDL_Rect destRect;
                destRect.x = col * TILE_SIZE;
                destRect.y = row * TILE_SIZE;
                destRect.w = TILE_SIZE;
                destRect.h = TILE_SIZE;

                SDL_RenderCopy(renderer, tilesetTexture, &srcRect, &destRect);
            }
        }

        SDL_RenderPresent(renderer);
    }

    // ======== Giải phóng tài nguyên ========
    SDL_DestroyTexture(tilesetTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
