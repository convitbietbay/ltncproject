#include "game.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>

PoisonHandler poisonHandler;

// Constructor
Game::Game() : window(nullptr), renderer(nullptr), collisionLayer(nullptr),
               redDiamondLayer(nullptr), blueDiamondLayer(nullptr),
               scoreFont(nullptr), fireboyScoreTexture(nullptr),
               watergirlScoreTexture(nullptr), fireboyScore(0),
               watergirlScore(0), gameTime(0.0f), gameState(GameState::MENU),
               isGameOver(false), restart(false), inputPromptTexture(nullptr),
               nameTexture(nullptr), isWin(false), winMessageTexture(nullptr) {}

// Destructor
Game::~Game() {
    cleanup();
}

SDL_Texture* Game::loadTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "[IMG Load Error] " << path << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool Game::loadMap(const std::string& mapPath) {
    try {
        map.load(mapPath);
    } catch (const std::exception& e) {
        std::cerr << "Lỗi load TMX: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Game::loadTilesets() {
    for (const auto& ts : map.getTilesets()) {
        Tileset t;
        t.firstGID = ts.getFirstGID();
        t.columns = ts.getImageSize().x / map.getTileSize().x;

        std::string imagePath = ts.getImagePath();
        std::string filename = imagePath.substr(imagePath.find_last_of("/\\") + 1);
        std::string fullPath = "img/" + filename;

        t.texture = loadTexture(fullPath);
        if (!t.texture) return false;
        tilesets.push_back(t);
    }
    return true;
}

bool Game::loadCharacters() {
    if (!fireboy.loadTexture(renderer, "img/fireboy.png") ||
        !watergirl.loadTexture(renderer, "img/watergirl.png")) {
        return false;
    }
    fireboy.setPosition(6 * TILE_SIZE, 27 * TILE_SIZE);
    watergirl.setPosition(3 * TILE_SIZE, 27 * TILE_SIZE);
    fireboy.setSize(48, 64);
    watergirl.setSize(48, 64);
    fireboy.setMoveDirection(0);
    watergirl.setMoveDirection(0);
    fireboy.setAlpha(255);
    watergirl.setAlpha(255);
    fireboy.setVelocity(0, 0);
    watergirl.setVelocity(0, 0);
    return true;
}

bool Game::findCollisionLayer() {
    for (const auto& layer : map.getLayers()) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
            const auto* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
            if (tileLayer && tileLayer->getName() == "Tile Layer 2: area") {
                collisionLayer = tileLayer;
                return true;
            }
        }
    }
    return false;
}

bool Game::findDiamondLayers() {
    bool foundRed = false, foundBlue = false;
    for (const auto& layer : map.getLayers()) {
        std::cout << "[DEBUG] Layer: " << layer->getName() << "\n";
    }

    for (const auto& layer : map.getLayers()) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
            auto* tileLayer = dynamic_cast<tmx::TileLayer*>(layer.get());
            if (tileLayer) {
                std::string layerName = tileLayer->getName();
                std::string lowerName = layerName;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

                if (lowerName == "tile layer 4: diamond fire" ||
                    lowerName == "tile layer 4: diamondfire" ||
                    lowerName == "diamond fire" ||
                    (lowerName.find("diamond") != std::string::npos && lowerName.find("fire") != std::string::npos)) {
                    redDiamondLayer = tileLayer;
                    redDiamondTiles = tileLayer->getTiles();
                    foundRed = true;
                    std::cout << "[INFO] Found red diamond layer: " << layerName << "\n";
                }
                else if (lowerName == "tile layer 9: diamond water" ||
                         lowerName == "tile layer 9: diamondwater" ||
                         lowerName == "diamond water" ||
                         (lowerName.find("diamond") != std::string::npos && lowerName.find("water") != std::string::npos)) {
                    blueDiamondLayer = tileLayer;
                    blueDiamondTiles = tileLayer->getTiles();
                    foundBlue = true;
                }
            }
        }
    }

    if (!foundRed || !foundBlue) {
        std::cerr << "[ERROR] Could not find diamond layers: "
                  << (!foundRed ? "diamond fire " : "")
                  << (!foundBlue ? "diamond water" : "") << "\n";
        return false;
    }
    return true;
}

bool Game::initScoreDisplay() {
    scoreFont = TTF_OpenFont("font/TrajanPro-Bold.otf", 24);
    if (!scoreFont) {
        std::cerr << "Failed to load score font: " << TTF_GetError() << std::endl;
        return false;
    }
    fireboyScore = 0;
    watergirlScore = 0;
    updateScoreDisplay();
    if (!timer.init(renderer, scoreFont)) {
        std::cerr << "Failed to initialize timer.\n";
        return false;
    }
    return true;
}

void Game::updateScoreDisplay() {
    if (fireboyScoreTexture) SDL_DestroyTexture(fireboyScoreTexture);
    std::string fbScoreText = "FIREBOY: " + std::to_string(fireboyScore);
    SDL_Color red = {255, 102, 102, 255};
    SDL_Surface* fbSurface = TTF_RenderText_Solid(scoreFont, fbScoreText.c_str(), red);
    if (!fbSurface) {
        std::cerr << "Failed to create Fireboy score surface: " << TTF_GetError() << std::endl;
        return;
    }
    fireboyScoreTexture = SDL_CreateTextureFromSurface(renderer, fbSurface);
    SDL_FreeSurface(fbSurface);
    SDL_QueryTexture(fireboyScoreTexture, nullptr, nullptr, &fireboyScoreRect.w, &fireboyScoreRect.h);
    fireboyScoreRect.x = 20;
    fireboyScoreRect.y = 20;

    if (watergirlScoreTexture) SDL_DestroyTexture(watergirlScoreTexture);
    std::string wgScoreText = "WATERGIRL: " + std::to_string(watergirlScore);
    SDL_Color blue = {102, 204, 255, 255};
    SDL_Surface* wgSurface = TTF_RenderText_Solid(scoreFont, wgScoreText.c_str(), blue);
    if (!wgSurface) {
        std::cerr << "Failed to create Watergirl score surface: " << TTF_GetError() << std::endl;
        return;
    }
    watergirlScoreTexture = SDL_CreateTextureFromSurface(renderer, wgSurface);
    SDL_FreeSurface(wgSurface);
    SDL_QueryTexture(watergirlScoreTexture, nullptr, nullptr, &watergirlScoreRect.w, &watergirlScoreRect.h);
    watergirlScoreRect.x = 20;
    watergirlScoreRect.y = 50;
}

void Game::saveHighscores() {
    std::ofstream file("highscore/highscore.txt");
    if (file.is_open()) {
        for (const auto& entry : highscores) {
            file << entry.name << " " << entry.time << "\n";
        }
        file.close();
    } else {
        std::cerr << "[ERROR] Failed to save highscores to file\n";
    }
}

void Game::loadHighscores() {
    highscores.clear();
    std::ifstream file("highscore/highscore.txt");
    if (file.is_open()) {
        std::string name;
        float time;
        while (file >> name >> time) {
            highscores.push_back({name, time});
        }
        file.close();
        std::sort(highscores.begin(), highscores.end(), [](const HighscoreEntry& a, const HighscoreEntry& b) {
            return a.time < b.time;
        });}
}

void Game::handleNameInput(SDL_Event& event, bool& running) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN && !playerName.empty()) {
            if (isWin) {
                highscores.push_back({playerName, gameTime});
                std::sort(highscores.begin(), highscores.end(), [](const HighscoreEntry& a, const HighscoreEntry& b) {
                    return a.time < b.time;
                });
                saveHighscores();
                mainMenu.updateHighscore(renderer, highscores);
            }
            gameState = GameState::GAME_OVER;
            playerName.clear();
            updateNameTexture();
        } else if (event.key.keysym.sym == SDLK_BACKSPACE && !playerName.empty()) {
            playerName.pop_back();
            updateNameTexture();
        }
    } else if (event.type == SDL_TEXTINPUT) {
        if (playerName.length() < 10) {
            playerName += event.text.text;
            updateNameTexture();
        }
    }
}

void Game::updateNameTexture() {
    if (nameTexture) SDL_DestroyTexture(nameTexture);
    std::string displayText = playerName.empty() ? " " : playerName;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* nameSurface = TTF_RenderText_Solid(scoreFont, displayText.c_str(), white);
    if (!nameSurface) {
        std::cerr << "Failed to create name surface: " << TTF_GetError() << std::endl;
        return;
    }
    nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
    SDL_FreeSurface(nameSurface);
    SDL_QueryTexture(nameTexture, nullptr, nullptr, &nameRect.w, &nameRect.h);
    nameRect.x = (704 - nameRect.w) / 2;
    nameRect.y = 300;
}

void Game::renderNameInput() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (winMessageTexture) SDL_RenderCopy(renderer, winMessageTexture, nullptr, &winMessageRect);
    SDL_RenderCopy(renderer, inputPromptTexture, nullptr, &inputPromptRect);
    if (nameTexture) SDL_RenderCopy(renderer, nameTexture, nullptr, &nameRect);

    SDL_RenderPresent(renderer);
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Fireboy & Watergirl",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    scoreFont = TTF_OpenFont("font/TrajanPro-Bold.otf", 24);
    if (!scoreFont) {
        std::cerr << "Failed to load score font: " << TTF_GetError() << std::endl;
        return false;
    }

    loadHighscores();
    if (!mainMenu.init(renderer, scoreFont, highscores)) {
        std::cerr << "MainMenu init failed.\n";
        return false;
    }

    if (!gameover.init(renderer)) {
        std::cerr << "GameOver menu init failed.\n";
        return false;
    }

    if (!loadMap("tsx/level1.tmx")) {
        std::cerr << "Failed to load map.\n";
        return false;
    }

    if (!loadTilesets()) {
        std::cerr << "Failed to load tilesets.\n";
        return false;
    }

    if (!loadCharacters()) {
        std::cerr << "Failed to load characters.\n";
        return false;
    }

    if (!findCollisionLayer()) {
        std::cerr << "Failed to find collision layer.\n";
        return false;
    }

    if (!poisonHandler.loadHazardLayers(map.getLayers())) {
        std::cerr << "Failed to load hazard layers.\n";
        return false;
    }

    if (!findDiamondLayers()) {
        std::cerr << "Failed to load diamond layers.\n";
        return false;
    }

    if (!diamondHandler.loadDiamonds(redDiamondLayer, blueDiamondLayer, TILE_SIZE)) {
        std::cerr << "Failed to load diamonds.\n";
        return false;
    }

    if (!initScoreDisplay()) {
        std::cerr << "Failed to initialize score display.\n";
        return false;
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* promptSurface = TTF_RenderText_Solid(scoreFont, "Enter your name:", white);
    if (!promptSurface) {
        std::cerr << "Failed to create input prompt surface: " << TTF_GetError() << std::endl;
        return false;
    }
    inputPromptTexture = SDL_CreateTextureFromSurface(renderer, promptSurface);
    SDL_FreeSurface(promptSurface);
    SDL_QueryTexture(inputPromptTexture, nullptr, nullptr, &inputPromptRect.w, &inputPromptRect.h);
    inputPromptRect.x = (704 - inputPromptRect.w) / 2;
    inputPromptRect.y = 250;

    SDL_Surface* winSurface = TTF_RenderText_Solid(scoreFont, "You Win", white);
    if (!winSurface) {
        std::cerr << "Failed to create win message surface: " << TTF_GetError() << std::endl;
        return false;
    }
    winMessageTexture = SDL_CreateTextureFromSurface(renderer, winSurface);
    SDL_FreeSurface(winSurface);
    SDL_QueryTexture(winMessageTexture, nullptr, nullptr, &winMessageRect.w, &winMessageRect.h);
    winMessageRect.x = (704 - winMessageRect.w) / 2;
    winMessageRect.y = 200;
    return true;
}

void Game::handleEvents(bool& running) {
    SDL_Event event;
    static bool startGame = false;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            return;
        }

        if (gameState == GameState::MENU) {
            mainMenu.handleEvents(event, startGame);
            if (startGame) {
                std::cout << "[INFO] Start game triggered, switching to PLAYING state\n";
                gameState = GameState::PLAYING;
                startGame = false;
                timer.start();
                break; // Đảm bảo thoát vòng lặp sự kiện để chuyển trạng thái ngay lập tức
            }
        } else if (gameState == GameState::PLAYING) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w: fireboy.jump(); break;
                    case SDLK_UP: watergirl.jump(); break;
                    case SDLK_a: fireboy.setMoveDirection(-1); break;
                    case SDLK_d: fireboy.setMoveDirection(1); break;
                    case SDLK_LEFT: watergirl.setMoveDirection(-1); break;
                    case SDLK_RIGHT: watergirl.setMoveDirection(1); break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_a:
                    case SDLK_d:
                        fireboy.setMoveDirection(0); break;
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        watergirl.setMoveDirection(0); break;
                }
            }
        } else if (gameState == GameState::INPUT_NAME) {
            handleNameInput(event, running);
        }
    }
}
void Game::update(float deltaTime, bool& running) {
    fireboy.update(deltaTime, collisionLayer);
    watergirl.update(deltaTime, collisionLayer);

    SDL_Rect fireboyRect = fireboy.getRect();
    SDL_Rect watergirlRect = watergirl.getRect();
    SDL_Rect fireboyWinArea = {527, 144, 32, 32};
    SDL_Rect watergirlWinArea = {640, 144, 32, 32};

    float fireboyFootX = fireboyRect.x + fireboyRect.w / 2.0f;
    float fireboyFootY = fireboyRect.y + fireboyRect.h;
    float watergirlFootX = watergirlRect.x + watergirlRect.w / 2.0f;
    float watergirlFootY = watergirlRect.y + watergirlRect.h;

    bool fireboyInWinArea = fireboyFootX >= fireboyWinArea.x &&
                            fireboyFootX <= fireboyWinArea.x + fireboyWinArea.w &&
                            fireboyFootY >= fireboyWinArea.y &&
                            fireboyFootY <= fireboyWinArea.y + fireboyWinArea.h;
    bool watergirlInWinArea = watergirlFootX >= watergirlWinArea.x &&
                              watergirlFootX <= watergirlWinArea.x + watergirlWinArea.w &&
                              watergirlFootY >= watergirlWinArea.y &&
                              watergirlFootY <= watergirlWinArea.y + watergirlWinArea.h;

    if (fireboyInWinArea && watergirlInWinArea) {
        gameTime = SDL_GetTicks() / 1000.0f;
        isWin = true;
        gameState = GameState::INPUT_NAME;
        SDL_StartTextInput();
        return;
    }
    bool fbDead = poisonHandler.handle(fireboy, true, renderer);
    bool wgDead = poisonHandler.handle(watergirl, false, renderer);
    if (fbDead || wgDead) {
        gameTime = SDL_GetTicks() / 1000.0f;
        isGameOver = true;
        isWin = false;
        gameState = GameState::GAME_OVER;
        return;
    }

    diamondHandler.checkCollision(fireboy, true, fireboyScore, redDiamondTiles, blueDiamondTiles);
    diamondHandler.checkCollision(watergirl, false, watergirlScore, redDiamondTiles, blueDiamondTiles);
    updateScoreDisplay();
    timer.update();
}

void Game::renderMap() {
    for (const auto& layer : map.getLayers()) {
        if (layer->getType() != tmx::Layer::Type::Tile) continue;
        const auto* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
        if (!tileLayer) continue;

        std::string layerName = tileLayer->getName();

        std::string lowerName = layerName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        const auto& tiles = (lowerName == "tile layer 4: diamond fire" ||
                             lowerName == "tile layer 4: diamondfire" ||
                             lowerName == "diamond fire" ||
                             (lowerName.find("diamond") != std::string::npos && lowerName.find("fire") != std::string::npos)) ? redDiamondTiles :
                            (lowerName == "tile layer 9: diamond water" ||
                             lowerName == "tile layer 9: diamondwater" ||
                             lowerName == "diamond water" ||
                             (lowerName.find("diamond") != std::string::npos && lowerName.find("water") != std::string::npos)) ? blueDiamondTiles :
                            tileLayer->getTiles();
        int width = map.getTileCount().x;
        for (size_t i = 0; i < tiles.size(); ++i) {
            int gid = tiles[i].ID;
            if (gid == 0) continue;

            const Tileset* selected = nullptr;
            for (int j = tilesets.size() - 1; j >= 0; --j) {
                if (gid >= tilesets[j].firstGID) {
                    selected = &tilesets[j];
                    break;
                }
            }
            if (!selected) continue;

            int localID = gid - selected->firstGID;
            int col = (i % width), row = (i / width);

            SDL_Rect src = {
                (localID % selected->columns) * TILE_SIZE,
                (localID / selected->columns) * TILE_SIZE,
                TILE_SIZE, TILE_SIZE
            };
            SDL_Rect dst = {
                col * TILE_SIZE,
                row * TILE_SIZE,
                TILE_SIZE, TILE_SIZE
            };

            SDL_RenderCopy(renderer, selected->texture, &src, &dst);
        }
    }
}

void Game::render() {
    if (gameState == GameState::MENU) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        mainMenu.render(renderer);
        SDL_RenderPresent(renderer);
    } else if (gameState == GameState::PLAYING) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderMap();

        if (isGameOver) {
            gameover.render(renderer);
        } else {
            fireboy.render(renderer);
            watergirl.render(renderer);
            if (fireboyScoreTexture) SDL_RenderCopy(renderer, fireboyScoreTexture, nullptr, &fireboyScoreRect);
            if (watergirlScoreTexture) SDL_RenderCopy(renderer, watergirlScoreTexture, nullptr, &watergirlScoreRect);
            timer.render(renderer);
        }

        SDL_RenderPresent(renderer);
    } else if (gameState == GameState::GAME_OVER) {
        gameover.render(renderer);
    } else if (gameState == GameState::INPUT_NAME) {
        renderNameInput();
    }
}

void Game::run() {
    bool running = true;
    Uint32 restartDelay = 0;
    while (running) {
        if (gameState == GameState::MENU) {
            handleEvents(running);
            render();
            SDL_Delay(10);
        } else if (gameState == GameState::PLAYING) {
            restart = false;
            isGameOver = false;
            fireboyScore = 0;
            watergirlScore = 0;
            gameTime = 0.0f;
            timer.reset();
            restartDelay = SDL_GetTicks() + 200; // Giảm delay từ 500ms xuống 200ms để phản hồi nhanh hơn
            loadCharacters();
            poisonHandler.loadHazardLayers(map.getLayers());
            if (redDiamondLayer) redDiamondTiles = redDiamondLayer->getTiles();
            if (blueDiamondLayer) blueDiamondTiles = blueDiamondLayer->getTiles();
            diamondHandler.loadDiamonds(redDiamondLayer, blueDiamondLayer, TILE_SIZE);
            Uint32 lastTime = SDL_GetTicks();
            while (running && gameState == GameState::PLAYING) {
                Uint32 currentTime = SDL_GetTicks();
                float deltaTime = (currentTime - lastTime) / 1000.0f;
                lastTime = currentTime;
                handleEvents(running);
                if (currentTime > restartDelay) {
                    update(deltaTime, running);
                }

                render();
                SDL_Delay(10);
            }
        } else if (gameState == GameState::INPUT_NAME) {
            while (running && gameState == GameState::INPUT_NAME) {
                handleEvents(running);
                render();
                SDL_Delay(10);
            }
        } else if (gameState == GameState::GAME_OVER) {
            SDL_PumpEvents();
            SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

            bool gameOverRunning = true;
            while (gameOverRunning && running) {
                gameover.handleEvents(gameOverRunning, restart);
                if (!gameOverRunning) {
                    restart ? "true" : "false";
                    break;
                }

                render();
                SDL_Delay(10);
            }

            if (running) {
                if (restart) {
                    gameState = GameState::PLAYING;
                } else {
                    gameState = GameState::MENU;
                }
            }
        }
    }
}
void Game::cleanup() {
    for (auto& ts : tilesets) SDL_DestroyTexture(ts.texture);
    if (fireboyScoreTexture) SDL_DestroyTexture(fireboyScoreTexture);
    if (watergirlScoreTexture) SDL_DestroyTexture(watergirlScoreTexture);
    if (inputPromptTexture) SDL_DestroyTexture(inputPromptTexture);
    if (nameTexture) SDL_DestroyTexture(nameTexture);
    if (winMessageTexture) SDL_DestroyTexture(winMessageTexture);
    if (scoreFont) TTF_CloseFont(scoreFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    gameover.cleanup();
    TTF_Quit();
}
