#include "types.h"
#include "menu_render.h"
#include "menu_config.h"
#include "piece_utils.h"
#include "texture_utils.h"
#include "difficulty_config.h"
#include "achievements.h"
#include "audio_manager.h"
#include "save_system.h"
#include "game_ui.h"
#include "game_mode_theme.h"
#include "game_logic.h"
#include "game_state.h"
#include "input_handler.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif







void explodeMenuBomb(const BackgroundPiece& bomb, 
                     std::vector<ExplosionEffect>& explosionEffects,
                     std::vector<GlowEffect>& glowEffects,
                     AudioManager& audioManager,
                     float& shakeIntensity,
                     float& shakeDuration,
                     float& shakeTimer,
                     SaveData& saveData,
                     std::vector<AchievementPopup>& achievementPopups) {
    
    float bombRotation = bomb.rotation;
    float bombCenterX = bomb.x;
    float bombCenterY = bomb.y;
    float bombZDepth = bomb.y;
    

    for (int dy = -2; dy <= 2; ++dy) {
        for (int dx = -2; dx <= 2; ++dx) {

            float localOffsetX = dx * 32.0f;
            float localOffsetY = dy * 32.0f;
            

            float angleRad = bombRotation * 3.14159265f / 180.0f;
            float cosAngle = std::cos(angleRad);
            float sinAngle = std::sin(angleRad);
            
            float rotatedOffsetX = localOffsetX * cosAngle - localOffsetY * sinAngle;
            float rotatedOffsetY = localOffsetX * sinAngle + localOffsetY * cosAngle;
            

            float explosionX = bombCenterX + rotatedOffsetX;
            float explosionY = bombCenterY + rotatedOffsetY;
            

            explosionEffects.emplace_back(explosionX, explosionY, bombRotation, bombZDepth);
            

            float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
            float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
            float glowRotation = static_cast<float>(rand() % 360);
            sf::Color glowColor = sf::Color(
                150 + rand() % 106, 
                150 + rand() % 106, 
                150 + rand() % 106
            );
            glowEffects.emplace_back(explosionX + offsetX, explosionY + offsetY, glowColor, glowRotation);
        }
    }
    

    audioManager.playBombSound();
    shakeIntensity = 15.0f;
    shakeDuration = 0.4f;
    shakeTimer = 0.0f;
    

    unlockAchievement(saveData, Achievement::MenuBombClicker, &achievementPopups, &audioManager);
    saveGameData(saveData);
    
    std::cout << "Bomb clicked and exploded in menu!" << std::endl;
}

std::string getGameModeText(GameModeOption mode, ClassicDifficulty difficulty, SprintLines lines, ChallengeMode challenge) {
    std::string modeText = "";
    
    if (mode == GameModeOption::Classic) {
        modeText = "CLASSIC - ";
        switch (difficulty) {
            case ClassicDifficulty::Normal:
                modeText += "NORMAL";
                break;
            case ClassicDifficulty::Hard:
                modeText += "HARD";
                break;
        }
    } else if (mode == GameModeOption::Sprint) {
        modeText = "BLITZ - ";
        switch (lines) {
            case SprintLines::Lines24:
                modeText += "24 LINES";
                break;
            case SprintLines::Lines48:
                modeText += "48 LINES";
                break;
            case SprintLines::Lines96:
                modeText += "96 LINES";
                break;
            default:
                modeText += "24 LINES";
                break;
        }
    } else if (mode == GameModeOption::Challenge) {
        modeText = "CHALLENGE - ";
        switch (challenge) {
            case ChallengeMode::Debug:
                modeText += "DEBUG";
                break;
            case ChallengeMode::TheForest:
                modeText += "THE FOREST";
                break;
            case ChallengeMode::Randomness:
                modeText += "RANDOMNESS";
                break;
            case ChallengeMode::NonStraight:
                modeText += "NON-STRAIGHT";
                break;
            case ChallengeMode::OneRot:
                modeText += "ONE ROTATION";
                break;
            case ChallengeMode::ChristopherCurse:
                modeText += "THE CURSE";
                break;
            case ChallengeMode::Vanishing:
                modeText += "VANISHING";
                break;
            case ChallengeMode::AutoDrop:
                modeText += "AUTO DROP";
                break;
        }
    }
    
    return modeText;
}




AbilityType getAbilityType(PieceType type) {
    switch(type) {
        case PieceType::A_Bomb: return AbilityType::Bomb;
        case PieceType::A_Stomp: return AbilityType::Stomp;
        default: return AbilityType::None;
    }
}


std::vector<PieceType> PieceBag::createNewBag(int level) {
    std::cout << "Creating bag for level: " << level << std::endl;
    std::vector<PieceType> newBag;
    

    std::vector<PieceType> basicTypes = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic,
    };
    

    if (difficultyConfig && difficultyConfig->useCustomPieceFilter && !difficultyConfig->allowedBasicPieces.empty()) {
        basicTypes = difficultyConfig->allowedBasicPieces;
        std::cout << "Using custom basic pieces filter (" << basicTypes.size() << " pieces)" << std::endl;
    }
    

    if (!difficultyConfig) {
        std::cout << "WARNING: No difficulty config set, using fallback" << std::endl;

        if (level < 1) {
            newBag = basicTypes;
        } else if (level <= 5) {
            newBag = basicTypes;
            for (int i = 0; i < std::min(level, 2); i++) {
                if (mediumBagIndex >= mediumBag.size()) refillMediumBag();
                newBag.push_back(mediumBag[mediumBagIndex++]);
            }
            if (level >= 2) {
                for (int i = 0; i < std::min(level - 2, 2); i++) {
                    if (hardBagIndex >= hardBag.size()) refillHardBag();
                    newBag.push_back(hardBag[hardBagIndex++]);
                }
            }
        }
        std::shuffle(newBag.begin(), newBag.end(), rng);
        return newBag;
    }
    

    BagLevelConfig bagConfig = {7, 0, 0};
    
    for (const auto& threshold : difficultyConfig->levelThresholds) {
        if (level >= threshold.lines / 10) {
            bagConfig = threshold.bagConfig;
        }
    }
    

    int bestThreshold = 0;
    for (size_t i = 0; i < difficultyConfig->levelThresholds.size(); i++) {
        if (level >= static_cast<int>(i)) {
            bagConfig = difficultyConfig->levelThresholds[i].bagConfig;
        }
    }
    
    std::cout << "Bag config for level " << level << ": " 
              << bagConfig.basicPieces << " basic, " 
              << bagConfig.mediumPieces << " medium, " 
              << bagConfig.hardPieces << " hard" << std::endl;
    

    for (int i = 0; i < bagConfig.basicPieces; i++) {
        newBag.push_back(basicTypes[i % basicTypes.size()]);
    }
    

    for (int i = 0; i < bagConfig.mediumPieces; i++) {
        if (mediumBagIndex >= mediumBag.size()) refillMediumBag();
        newBag.push_back(mediumBag[mediumBagIndex++]);
    }
    

    for (int i = 0; i < bagConfig.hardPieces; i++) {
        if (hardBagIndex >= hardBag.size()) refillHardBag();
        newBag.push_back(hardBag[hardBagIndex++]);
    }
    
    std::shuffle(newBag.begin(), newBag.end(), rng);
    return newBag;
}

void PieceBag::ensureNextBagReady() {
    if (!nextBagReady) {
        nextBag = createNewBag(currentLevel);
        nextBagReady = true;
    }
}

void PieceBag::fillNextQueue() {
    nextQueue.clear();
    int tempBagIndex = bagIndex;
    
    for (int i = 0; i < 3; ++i) {
        if (tempBagIndex >= currentBag.size()) {
            ensureNextBagReady();
            int nextBagIdx = tempBagIndex - currentBag.size();
            nextQueue.push_back(nextBag[nextBagIdx]);
            tempBagIndex++;
        } else {
            nextQueue.push_back(currentBag[tempBagIndex++]);
        }
    }
}

void PieceBag::refillMediumBag() {

    std::vector<PieceType> defaultMediumTypes = {
        PieceType::I_Medium, PieceType::T_Medium, PieceType::L_Medium, 
        PieceType::J_Medium, PieceType::O_Medium, PieceType::S_Medium, PieceType::Z_Medium
    };
    

    if (difficultyConfig && difficultyConfig->useCustomPieceFilter && !difficultyConfig->allowedMediumPieces.empty()) {
        mediumBag = difficultyConfig->allowedMediumPieces;
    } else {
        mediumBag = defaultMediumTypes;
    }
    
    std::shuffle(mediumBag.begin(), mediumBag.end(), rng);
    mediumBagIndex = 0;
    std::cout << "Medium bag refilled and shuffled (" << mediumBag.size() << " pieces)" << std::endl;
}

void PieceBag::refillHardBag() {

    std::vector<PieceType> defaultHardTypes = {
        PieceType::I_Hard, PieceType::T_Hard, PieceType::L_Hard, 
        PieceType::J_Hard, PieceType::O_Hard, PieceType::S_Hard, PieceType::Z_Hard
    };
    

    if (difficultyConfig && difficultyConfig->useCustomPieceFilter && !difficultyConfig->allowedHardPieces.empty()) {
        hardBag = difficultyConfig->allowedHardPieces;
    } else {
        hardBag = defaultHardTypes;
    }
    
    std::shuffle(hardBag.begin(), hardBag.end(), rng);
    hardBagIndex = 0;
    std::cout << "Hard bag refilled and shuffled (" << hardBag.size() << " pieces)" << std::endl;
}

PieceBag::PieceBag() : rng(std::random_device{}()), currentLevel(0) {
    std::cout << "PieceBag constructor: currentLevel = " << currentLevel << std::endl;
    refillMediumBag();
    refillHardBag();
    currentBag = createNewBag(currentLevel);
    bagIndex = 0;
    fillNextQueue();
}

PieceType PieceBag::getNextPiece() {
    if (bagIndex >= currentBag.size()) {
        ensureNextBagReady();
        currentBag = nextBag;
        bagIndex = 0;
        nextBagReady = false;
    }
    
    PieceType piece = currentBag[bagIndex++];
    fillNextQueue();
    return piece;
}

void PieceBag::updateLevel(int newLevel) {
    if (newLevel != currentLevel) {
        currentLevel = newLevel;
        std::cout << "Bag difficulty updated to level " << currentLevel << "!" << std::endl;
    }
}

const std::vector<PieceType>& PieceBag::getNextQueue() const {
    return nextQueue;
}

const std::vector<PieceType>& PieceBag::getCurrentBag() const {
    return currentBag;
}

const std::vector<PieceType>& PieceBag::getNextBag() const {
    return nextBag;
}

int PieceBag::getBagIndex() const {
    return bagIndex;
}

bool PieceBag::isNextBagReady() const {
    return nextBagReady;
}

void PieceBag::reset() {
    reset(0);
}

void PieceBag::reset(int startLevel) {
    currentLevel = startLevel;
    bagIndex = 0;
    nextBagReady = false;
    refillMediumBag();
    refillHardBag();
    currentBag = createNewBag(currentLevel);
    nextBag.clear();
    nextQueue.clear();
    fillNextQueue();
    std::cout << "Bag system reset to level " << currentLevel << "!" << std::endl;
}

void PieceBag::setDifficultyConfig(const DifficultyConfig* config) {
    difficultyConfig = config;
    std::cout << "Difficulty config set to: " << (config ? config->modeName : "nullptr") << std::endl;
    if (config) {
        std::cout << "  Max Levels: " << config->maxLevels << std::endl;
        std::cout << "  Line Goal: " << (config->hasLineGoal ? std::to_string(config->lineGoal) : "None") << std::endl;
        std::cout << "  Bomb: " << (config->bombEnabled ? "Yes" : "No") << std::endl;
        std::cout << "  Hold: " << (config->holdEnabled ? "Yes" : "No") << std::endl;
    }
}

void PieceBag::returnPieceToBag(PieceType piece) {
    std::cout << "[DEBUG] Returning piece to bag at position " << bagIndex << " (will be next piece)" << std::endl;
    currentBag.insert(currentBag.begin() + bagIndex, piece);
    fillNextQueue();
    std::cout << "[DEBUG] Bag size after return: " << currentBag.size() << std::endl;
}

void PieceBag::insertPiecesToQueue(PieceType piece, int count) {
    std::cout << "[DEBUG] Inserting " << count << " pieces into queue at position " << bagIndex << std::endl;
    for (int i = 0; i < count; i++) {
        currentBag.insert(currentBag.begin() + bagIndex + i, piece);
    }
    fillNextQueue();
    std::cout << "[DEBUG] Bag size after insert: " << currentBag.size() << std::endl;
}


void StompEffect(int stompX, int stompY, std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, float& shakeIntensity, float& shakeDuration, float& shakeTimer);

class Piece {
private:
    PieceShape shape;
    PieceType type;
    int x, y;
    bool isStatic = false;
    float fallTimer = 0.0f;
    bool touchingGround = false;
    float lockDelayTimer = 0.0f;
    int lockResetCount = 0;
    int lowestY = 0;
    int highestY = GRID_HEIGHT;
    static constexpr float LOCK_DELAY_TIME = 2.0f;
    static constexpr int MAX_LOCK_RESETS = 15;
    AbilityType ability;
    void updatePosition() {
        float worldX = GRID_OFFSET_X + x * CELL_SIZE;
        float worldY = GRID_OFFSET_Y + y * CELL_SIZE;
    }
public:
    Piece(int x, int y, PieceType pieceType, bool isStatic = false)
    : isStatic(isStatic), x(x), y(y), type(pieceType), touchingGround(false), lockDelayTimer(0.0f), lockResetCount(0), lowestY(y), highestY(y)
    {
        shape = getPieceShape(pieceType);
        ability = getAbilityType(pieceType);
    }
    PieceType getType() const { return type; }
    AbilityType getAbility() const { return ability; }
    int getX() const { return x; }
    int getY() const { return y; }
    void setY(int newY) { y = newY; }
    void setColor(const sf::Color& newColor) { shape.color = newColor; }
    const PieceShape& getShape() const { return shape; }
    bool collidesAt(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, int testX, int testY) const {
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (!shape.blocks[i][j]) continue;
                int gx = testX + j;
                int gy = testY + i;
                if (gx < 0 || gx >= GRID_WIDTH) return true;
                if (gy >= GRID_HEIGHT) return true;
                if (gy < 0) continue;
                if (grid[gy][gx].occupied) return true;
            }
        }
        return false;
    }

    bool collidesAtWithCeiling(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, int testX, int testY) const {
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (!shape.blocks[i][j]) continue;
                int gx = testX + j;
                int gy = testY + i;
                if (gx < 0 || gx >= GRID_WIDTH) return true;
                if (gy >= GRID_HEIGHT) return true;
                if (gy < 0) return true;
                if (grid[gy][gx].occupied) return true;
            }
        }
        return false;
    }
    void update(float deltaTime, bool fastFall, std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, int currentLevel, float gravityValue, bool gravityFlipped = false) {
        if (isStatic) return;

        fallTimer += deltaTime;
        

        bool isGrounded;
        if (gravityFlipped) {
            isGrounded = collidesAtWithCeiling(grid, x, y - 1);
        } else {
            isGrounded = collidesAt(grid, x, y + 1);
        }


        if (gravityFlipped) {
            if (y < highestY) {
                highestY = y;
                lockResetCount = 0;
            }
        } else {
            if (y > lowestY) {
                lowestY = y;
                lockResetCount = 0;
            }
        }

        if (isGrounded) {
            if (!touchingGround) {
                touchingGround = true;
                lockDelayTimer = 0.0f;
            }
            
            lockDelayTimer += deltaTime;
            

            float timeRemaining = LOCK_DELAY_TIME - lockDelayTimer;
            if (timeRemaining > 0) {
                std::cout << "[LOCK DELAY] Time: " << std::fixed << std::setprecision(2) << timeRemaining 
                          << "s | Resets: " << lockResetCount << "/" << MAX_LOCK_RESETS 
                          << " | Y: " << y << " | LowestY: " << lowestY << std::endl;
            }

            if (lockDelayTimer >= LOCK_DELAY_TIME || lockResetCount >= MAX_LOCK_RESETS) {
                isStatic = true;
                if (lockResetCount >= MAX_LOCK_RESETS) {
                    std::cout << "[LOCK DELAY] Piece locked! (max resets reached)" << std::endl;
                } else {
                    std::cout << "[LOCK DELAY] Piece locked! (timeout)" << std::endl;
                }
            }
        } else {
            touchingGround = false;
            lockDelayTimer = 0.0f;



            float fallInterval = 1.0f / gravityValue;
            
            if (fallTimer >= (fastFall ? 0.03f : fallInterval)) {
                if (gravityFlipped) {
                    y--;
                } else {
                    y++;
                }
                fallTimer = 0.0f;
                updatePosition();
            }
        }
    }
    void ChangeToStatic(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, AbilityType ability = AbilityType::None, AudioManager* audioManager = nullptr, std::vector<ExplosionEffect>* explosions = nullptr, std::vector<GlowEffect>* glowEffects = nullptr, float* shakeIntensity = nullptr, float* shakeDuration = nullptr, float* shakeTimer = nullptr, int* consecutiveBombsUsed = nullptr, SaveData* saveData = nullptr, std::vector<AchievementPopup>* achievementPopups = nullptr, bool isVanishingMode = false) {

        if (ability != AbilityType::Stomp) {
            for (int i = 0; i < shape.height; ++i) {
                for (int j = 0; j < shape.width; ++j) {
                    if (shape.blocks[i][j]) {
                        int gx = x + j;
                        int gy = y + i;
                        if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT) {
                            grid[gy][gx] = Cell(shape.color, getTextureType(type));

                            if (isVanishingMode) {
                                grid[gy][gx].isVanishing = true;
                                grid[gy][gx].vanishTimer = 0.0f;
                            }
                        }
                    }
                }
            }
        }
        
        if (ability != AbilityType::None) {
            AbilityEffect(grid, audioManager, explosions, glowEffects, shakeIntensity, shakeDuration, shakeTimer, consecutiveBombsUsed, saveData, achievementPopups);
        }
    }
    void BombEffect(int centerX, int centerY, std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, AudioManager& audioManager, std::vector<ExplosionEffect>& explosions, std::vector<GlowEffect>& glowEffects, float& shakeIntensity, float& shakeDuration, float& shakeTimer, int* consecutiveBombsUsed = nullptr, SaveData* saveData = nullptr, std::vector<AchievementPopup>* achievementPopups = nullptr) {
        audioManager.playBombSound();
        

        if (consecutiveBombsUsed && saveData && achievementPopups) {
            (*consecutiveBombsUsed)++;
            std::cout << "[BOMB] Consecutive explosions: " << *consecutiveBombsUsed << "/3" << std::endl;
            
            if (*consecutiveBombsUsed >= 3) {
                std::cout << "[ACHIEVEMENT] UNLOCKING EXPLOSION!" << std::endl;
                unlockAchievement(*saveData, Achievement::Explosion, achievementPopups, &audioManager);
            }
        }
        

        shakeIntensity = 15.0f;
        shakeDuration = 0.4f;
        shakeTimer = 0.0f;
        
        int minX = std::max(0, centerX - 2);
        int maxX = std::min(GRID_WIDTH - 1, centerX + 2);
        int minY = std::max(0, centerY - 2);
        int maxY = std::min(GRID_HEIGHT - 1, centerY + 2);
        
        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -2; dx <= 2; ++dx) {
                int x = centerX + dx;
                int y = centerY + dy;
                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {

                    sf::Color blockColor = grid[y][x].occupied ? grid[y][x].color : sf::Color(200, 200, 200);
                    
                    grid[y][x] = Cell();
                    

                    float explosionX = GRID_OFFSET_X + x * CELL_SIZE;
                    float explosionY = GRID_OFFSET_Y + y * CELL_SIZE;
                    float explosionRotation = static_cast<float>(rand() % 360);
                    
                    explosions.push_back(ExplosionEffect(explosionX, explosionY, explosionRotation, 0.0f));
                    
                    float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                    float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                    float glowRotation = static_cast<float>(rand() % 360);
                    
                    glowEffects.push_back(GlowEffect(explosionX + offsetX, explosionY + offsetY, blockColor, glowRotation));
                }
            }
        }
        
        for (int col = minX; col <= maxX; ++col) {
            for (int row = minY - 1; row >= 0; --row) {
                if (grid[row][col].occupied) {
                    int fallRow = row;
                    
                    while (fallRow + 1 < GRID_HEIGHT && !grid[fallRow + 1][col].occupied) {
                        fallRow++;
                    }
                    
                    if (fallRow != row) {
                        grid[fallRow][col] = grid[row][col];
                        grid[row][col] = Cell();
                    }
                }
            }
        }
    }
    
    void AbilityEffect(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, AudioManager* audioManager = nullptr, std::vector<ExplosionEffect>* explosions = nullptr, std::vector<GlowEffect>* glowEffects = nullptr, float* shakeIntensity = nullptr, float* shakeDuration = nullptr, float* shakeTimer = nullptr, int* consecutiveBombsUsed = nullptr, SaveData* saveData = nullptr, std::vector<AchievementPopup>* achievementPopups = nullptr)
    {
        switch(ability)
        {
            case AbilityType::Bomb:
                if (audioManager && explosions && glowEffects && shakeIntensity && shakeDuration && shakeTimer) {
                    BombEffect(x, y, grid, *audioManager, *explosions, *glowEffects, *shakeIntensity, *shakeDuration, *shakeTimer, consecutiveBombsUsed, saveData, achievementPopups);
                } else {
                    std::cout << "Bomb ability activated (no sound/explosions)!" << std::endl;
                }
                break;
            case AbilityType::Stomp:
                if (shakeIntensity && shakeDuration && shakeTimer) {
                    StompEffect(x, y, grid, *shakeIntensity, *shakeDuration, *shakeTimer);
                } else {
                    std::cout << "Stomp ability activated (no shake)!" << std::endl;
                }
                break;
            default:
                break;
        }
    }
    bool hasStopped() const { return isStatic; }
    void makeStatic() { isStatic = true; }
    void draw(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, bool useTextures) const {
        if (ability == AbilityType::Bomb) {
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    float worldX = GRID_OFFSET_X + (x + dx) * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + (y + dy) * CELL_SIZE;
                    
                    if (x + dx >= 0 && x + dx < GRID_WIDTH && y + dy >= 0 && y + dy < GRID_HEIGHT) {
                        sf::RectangleShape explosionPreview;
                        explosionPreview.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                        explosionPreview.setPosition(sf::Vector2f(worldX, worldY));
                        explosionPreview.setFillColor(sf::Color(255, 0, 0, 60));
                        explosionPreview.setOutlineColor(sf::Color(255, 0, 0, 120));
                        explosionPreview.setOutlineThickness(1);
                        window.draw(explosionPreview);
                    }
                }
            }
        } else if (ability == AbilityType::Stomp) {

            for (int dx = 0; dx < 3; ++dx) {
                if (x + dx >= 0 && x + dx < GRID_WIDTH) {

                    for (int dy = 0; dy < GRID_HEIGHT; ++dy) {
                        float worldX = GRID_OFFSET_X + (x + dx) * CELL_SIZE;
                        float worldY = GRID_OFFSET_Y + dy * CELL_SIZE;
                        
                        sf::RectangleShape columnPreview;
                        columnPreview.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                        columnPreview.setPosition(sf::Vector2f(worldX, worldY));
                        columnPreview.setFillColor(sf::Color(160, 82, 45, 30));
                        columnPreview.setOutlineColor(sf::Color(160, 82, 45, 80));
                        columnPreview.setOutlineThickness(1);
                        window.draw(columnPreview);
                    }
                }
            }
        }
        
        TextureType texType = getTextureType(type);
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float worldX = GRID_OFFSET_X + (x + j) * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + (y + i) * CELL_SIZE;
                    drawCell(window, worldX, worldY, CELL_SIZE, shape.color, texType, textures, useTextures);
                }
            }
        }
    }
    PieceShape rotateShapeRight(const PieceShape& original) const {
        PieceShape rotated = original;
        int oldHeight = original.height;
        int oldWidth = original.width;
        rotated.width = oldHeight;
        rotated.height = oldWidth;
        rotated.blocks.clear();
        rotated.blocks.resize(rotated.height, std::vector<bool>(rotated.width, false));
        for (int i = 0; i < oldHeight; ++i) {
            for (int j = 0; j < oldWidth; ++j) {
                if (original.blocks[i][j]) {
                    rotated.blocks[j][oldHeight - 1 - i] = true;
                }
            }
        }
        return rotated;
    }
    
    PieceShape rotateShapeLeft(const PieceShape& original) const {
        PieceShape rotated = original;
        int oldHeight = original.height;
        int oldWidth = original.width;
        rotated.width = oldHeight;
        rotated.height = oldWidth;
        rotated.blocks.clear();
        rotated.blocks.resize(rotated.height, std::vector<bool>(rotated.width, false));
        for (int i = 0; i < oldHeight; ++i) {
            for (int j = 0; j < oldWidth; ++j) {
                if (original.blocks[i][j]) {
                    rotated.blocks[oldWidth - 1 - j][i] = true;
                }
            }
        }
        return rotated;
    }
    void rotateRight(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) {
        if (isStatic) return;
        PieceShape rotatedShape = rotateShapeRight(shape);
        std::vector<std::pair<int, int>> kickOffsets = {{0, 0}, {-1, 0}, {1, 0}, {0, -1}, {-2, 0}, {2, 0}};
        PieceShape originalShape = shape;
        for (const auto& offset : kickOffsets) {
            int testX = x + offset.first;
            int testY = y + offset.second;
            shape = rotatedShape;
            if (!collidesAt(grid, testX, testY)) {
                x = testX;
                y = testY;
                if (touchingGround && lockResetCount < MAX_LOCK_RESETS) {
                    lockDelayTimer = 0.0f;
                    lockResetCount++;
                }
                return;
            }
            shape = originalShape;
        }
    }
    
    void rotateLeft(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) {
        if (isStatic) return;
        PieceShape rotatedShape = rotateShapeLeft(shape);
        std::vector<std::pair<int, int>> kickOffsets = {{0, 0}, {-1, 0}, {1, 0}, {0, -1}, {-2, 0}, {2, 0}};
        PieceShape originalShape = shape;
        for (const auto& offset : kickOffsets) {
            int testX = x + offset.first;
            int testY = y + offset.second;
            shape = rotatedShape;
            if (!collidesAt(grid, testX, testY)) {
                x = testX;
                y = testY;
                if (touchingGround && lockResetCount < MAX_LOCK_RESETS) {
                    lockDelayTimer = 0.0f;
                    lockResetCount++;
                }
                return;
            }
            shape = originalShape;
        }
    }
    
    void moveLeft(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) { 
        if (!isStatic && !collidesAt(grid, x - 1, y)) {
            --x;
            if (touchingGround && lockResetCount < MAX_LOCK_RESETS) {
                lockDelayTimer = 0.0f;
                lockResetCount++;
            }
        }
    }
    void moveRight(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) { 
        if (!isStatic && !collidesAt(grid, x + 1, y)) {
            ++x;
            if (touchingGround && lockResetCount < MAX_LOCK_RESETS) {
                lockDelayTimer = 0.0f;
                lockResetCount++;
            }
        }
    }
    int moveGround(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, bool gravityFlipped = false) {
        if (isStatic) return 0;
        int startY = y;
        if (gravityFlipped) {

            while (!collidesAtWithCeiling(grid, x, y - 1)) {
                --y;
            }
        } else {

            while (!collidesAt(grid, x, y + 1)) {
                ++y;
            }
        }
        isStatic = true;
        ChangeToStatic(grid, ability);
        return std::abs(y - startY);
    }
    

    int getGhostY(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, bool gravityFlipped = false) const {
        if (isStatic) return y;
        int ghostY = y;
        if (gravityFlipped) {

            while (!collidesAtWithCeiling(grid, x, ghostY - 1)) {
                --ghostY;
            }
        } else {

            while (!collidesAt(grid, x, ghostY + 1)) {
                ++ghostY;
            }
        }
        return ghostY;
    }
    

    void drawGhost(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, bool gravityFlipped = false) const {
        if (isStatic) return;
        

        if (ability == AbilityType::Bomb) return;
        
        int ghostY = getGhostY(grid, gravityFlipped);
        if (ghostY == y) return;
        
        TextureType texType = getTextureType(type);
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float worldX = GRID_OFFSET_X + (x + j) * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + (ghostY + i) * CELL_SIZE;
                    

                    sf::Color ghostColor = shape.color;
                    ghostColor.a = 80;
                    
                    drawCell(window, worldX, worldY, CELL_SIZE, ghostColor, texType, textures, useTextures);
                }
            }
        }
    }
};

void StompEffect(int stompX, int stompY, std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, float& shakeIntensity, float& shakeDuration, float& shakeTimer)
{
    std::cout << "STOMP EFFECT ACTIVATED at (" << stompX << ", " << stompY << ")!" << std::endl;
    

    shakeIntensity = 8.0f;
    shakeDuration = 0.4f;
    shakeTimer = 0.0f;
    

    int stompWidth = 3;
    

    for (int col = stompX; col < stompX + stompWidth && col < GRID_WIDTH; ++col) {
        if (col < 0) continue;
        
        std::cout << "  Compacting column " << col << std::endl;
        


        for (int targetRow = GRID_HEIGHT - 1; targetRow >= 0; --targetRow) {
            if (!grid[targetRow][col].occupied) {

                for (int sourceRow = targetRow - 1; sourceRow >= 0; --sourceRow) {
                    if (grid[sourceRow][col].occupied) {

                        grid[targetRow][col] = grid[sourceRow][col];
                        grid[sourceRow][col] = Cell();
                        std::cout << "    Moved block from row " << sourceRow << " to row " << targetRow << std::endl;
                        break;
                    }
                }
            }
        }
    }
    
    std::cout << "STOMP EFFECT COMPLETE!" << std::endl;
}

bool hasBlocksInTopRows(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, int topRows = 5) {
    for (int row = 0; row < topRows; ++row) {
        for (int col = 0; col < GRID_WIDTH; ++col) {
            if (grid[row][col].occupied) {
                return true;
            }
        }
    }
    return false;
}


void drawSpawnPreview(sf::RenderWindow& window, PieceType nextType, 
                      const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid,
                      const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    PieceShape shape = getPieceShape(nextType);
    int spawnX = (GRID_WIDTH - shape.width) / 2;
    int firstFilledRow = findFirstFilledRow(shape);
    int spawnY = -firstFilledRow;
    
    TextureType texType = getTextureType(nextType);
    

    for (int i = 0; i < shape.height; ++i) {
        for (int j = 0; j < shape.width; ++j) {
            if (shape.blocks[i][j]) {
                int gridRow = spawnY + i;
                int gridCol = spawnX + j;
                

                if (gridRow >= 0) {
                    float worldX = GRID_OFFSET_X + gridCol * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + gridRow * CELL_SIZE;
                    

                    sf::Color ghostColor = sf::Color(255, 100, 100, 120);
                    
                    drawCell(window, worldX, worldY, CELL_SIZE, ghostColor, texType, textures, useTextures);
                }
            }
        }
    }
}


void drawCollisionPiece(sf::RenderWindow& window, PieceType pieceType, int pieceX, int pieceY,
                        const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid,
                        const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    PieceShape shape = getPieceShape(pieceType);
    int firstFilledRow = findFirstFilledRow(shape);
    int spawnY = -firstFilledRow;
    
    TextureType texType = getTextureType(pieceType);
    

    for (int i = 0; i < shape.height; ++i) {
        for (int j = 0; j < shape.width; ++j) {
            if (shape.blocks[i][j]) {
                int gridRow = spawnY + i;
                int gridCol = pieceX + j;
                

                if (gridRow >= 0 && gridCol >= 0 && gridCol < GRID_WIDTH) {
                    float worldX = GRID_OFFSET_X + gridCol * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + gridRow * CELL_SIZE;
                    

                    sf::Color collisionColor = sf::Color(255, 50, 50, 255);
                    
                    drawCell(window, worldX, worldY, CELL_SIZE, collisionColor, texType, textures, useTextures);
                }
            }
        }
    }
}




int clearFullLinesGravityFlip(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, AudioManager& audioManager, std::vector<GlowEffect>* glowEffects = nullptr, float* shakeIntensity = nullptr, float* shakeDuration = nullptr, float* shakeTimer = nullptr) {
    int linesCleared = 0;
    int midPoint = GRID_HEIGHT / 2;
    

    std::vector<int> fullLineRowsBottom;
    std::vector<int> fullLineRowsTop;
    
    for (int row = 0; row < GRID_HEIGHT; row++) {
        bool isFullLine = true;
        for (int col = 0; col < GRID_WIDTH; col++) {
            if (!grid[row][col].occupied) {
                isFullLine = false;
                break;
            }
        }
        if (isFullLine) {
            if (row >= midPoint) {
                fullLineRowsBottom.push_back(row);
            } else {
                fullLineRowsTop.push_back(row);
            }
            

            if (glowEffects) {
                for (int col = 0; col < GRID_WIDTH; col++) {
                    sf::Color blockColor = grid[row][col].color;
                    float baseWorldX = GRID_OFFSET_X + col * CELL_SIZE;
                    float baseWorldY = GRID_OFFSET_Y + row * CELL_SIZE;
                    float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                    float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                    float glowRotation = static_cast<float>(rand() % 360);
                    
                    glowEffects->push_back(GlowEffect(baseWorldX + offsetX, baseWorldY + offsetY, blockColor, glowRotation));
                }
            }
            linesCleared++;
        }
    }
    


    for (int idx = fullLineRowsBottom.size() - 1; idx >= 0; idx--) {
        int row = fullLineRowsBottom[idx];

        for (int moveRow = row; moveRow > midPoint; moveRow--) {
            for (int col = 0; col < GRID_WIDTH; col++) {
                grid[moveRow][col] = grid[moveRow - 1][col];
            }
        }

        for (int col = 0; col < GRID_WIDTH; col++) {
            grid[midPoint][col] = Cell();
        }

        for (int j = idx - 1; j >= 0; j--) {
            if (fullLineRowsBottom[j] < row) {
                fullLineRowsBottom[j]++;
            }
        }
    }
    


    for (int idx = 0; idx < (int)fullLineRowsTop.size(); idx++) {
        int row = fullLineRowsTop[idx];

        for (int moveRow = row; moveRow < midPoint - 1; moveRow++) {
            for (int col = 0; col < GRID_WIDTH; col++) {
                grid[moveRow][col] = grid[moveRow + 1][col];
            }
        }

        for (int col = 0; col < GRID_WIDTH; col++) {
            grid[midPoint - 1][col] = Cell();
        }

        for (int j = idx + 1; j < (int)fullLineRowsTop.size(); j++) {
            if (fullLineRowsTop[j] > row) {
                fullLineRowsTop[j]--;
            }
        }
    }
    
    if (linesCleared > 0) { 
        audioManager.playLineClearSound(); 
    }
    
    if (linesCleared > 0 && shakeIntensity && shakeDuration && shakeTimer) {
        float intensity = 5.0f + (linesCleared * 2.5f);
        *shakeIntensity = intensity;
        *shakeDuration = 0.3f;
        *shakeTimer = 0.0f;
        std::cout << "GravityFlip line clear shake! Lines: " << linesCleared << ", Intensity: " << intensity << std::endl;
    }
    
    if (linesCleared >= 4) {
        int randomWow = rand() % 3;
        audioManager.playWowSound(randomWow);
        std::cout << "AMAZING! " << linesCleared << " lines cleared! Playing wow_0" << (randomWow + 1) << ".ogg" << std::endl;
    }
    
    return linesCleared;
}

void updateAllVolumes(sf::Music& menuMusic, sf::Music& gameplayMusic, std::unique_ptr<sf::Sound>& spaceSound, std::unique_ptr<sf::Sound>& laserSound, std::unique_ptr<sf::Sound>& bombSound, std::unique_ptr<sf::Sound>& achievementSound, std::unique_ptr<sf::Sound>& gameOverSound, std::vector<std::unique_ptr<sf::Sound>>& wowSounds, float masterVolume, float menuMusicVolume, float gameplayMusicVolume, float spaceVolume, float laserVolume, float bombVolume, float achievementVolume, float gameOverVolume, float wowVolume) {
    menuMusic.setVolume((menuMusicVolume * masterVolume) / 100.0f);
    gameplayMusic.setVolume((gameplayMusicVolume * masterVolume) / 100.0f);
    
    if (spaceSound) {
        spaceSound->setVolume((spaceVolume * masterVolume) / 100.0f);
    }
    
    if (laserSound) {
        laserSound->setVolume((laserVolume * masterVolume) / 100.0f);
    }
    
    if (bombSound) {
        bombSound->setVolume((bombVolume * masterVolume) / 100.0f);
    }
    
    if (achievementSound) {
        achievementSound->setVolume((achievementVolume * masterVolume) / 100.0f);
    }
    
    if (gameOverSound) {
        gameOverSound->setVolume((gameOverVolume * masterVolume) / 100.0f);
    }
    
    for (auto& wowSound : wowSounds) {
        if (wowSound) {
            wowSound->setVolume((wowVolume * masterVolume) / 100.0f);
        }
    }
}

void switchMusic(sf::Music& fromMusic, sf::Music& toMusic, sf::Music*& currentMusic) {
    if (currentMusic == &toMusic) {
        return;
    }
    
    fromMusic.stop();
    toMusic.play();
    currentMusic = &toMusic;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int argc = __argc;
    char** argv = __argv;
#else
int main(int argc, char* argv[]) {
#endif
    bool consoleMode = false;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-debugMode" || arg == "--debug" || arg == "-d") {
            consoleMode = true;
            break;
        }
    }
    
#ifdef _WIN32
    if (consoleMode) {
        AllocConsole();
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        freopen_s(&dummy, "CONIN$", "r", stdin);
        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();
    }
#endif
    
    srand(static_cast<unsigned int>(time(nullptr)));
    
    SaveData saveData = loadGameData();
    
    const unsigned int WINDOW_WIDTH = 1920;
    const unsigned int WINDOW_HEIGHT = 1080;
    sf::RenderWindow window;
    bool isFullscreen = true;
    if (isFullscreen) {
        auto desktopMode = sf::VideoMode::getDesktopMode();
        window.create(desktopMode, "Tessera", sf::Style::None);
        window.setMouseCursorVisible(false);
    } else {
        window.create(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Tessera", sf::Style::Default);
        window.setMouseCursorVisible(false);
    }
    window.setMouseCursorGrabbed(false);
    window.setFramerateLimit(144);
    window.requestFocus();
    

    sf::View mainView(sf::Vector2f(static_cast<float>(WINDOW_WIDTH) / 2.0f, static_cast<float>(WINDOW_HEIGHT) / 2.0f), 
                      sf::Vector2f(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)));
    window.setView(mainView);
    std::cout << "Window created with resolution: " << window.getSize().x << "x" << window.getSize().y << std::endl;
    std::cout << "View size: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << " (will be scaled to fit)" << std::endl;
    

    sf::Texture splashTexture;
    bool splashLoaded = false;
    if (splashTexture.loadFromFile("Assets/Texture/SplashScreen/Kilonia Studios.png")) {
        splashLoaded = true;
        std::cout << "Splash screen texture loaded successfully" << std::endl;
    } else {
        std::cout << "Unable to load splash screen texture" << std::endl;
    }
    
    std::map<TextureType, sf::Texture> textures;
    std::vector<TextureInfo> textureList = {
        {TextureType::Empty, "Assets/Texture/Cells/cell_background.png", sf::Color(50, 50, 60)},
        {TextureType::GenericBlock, "Assets/Texture/Cells/cell_normal_block_default.png", sf::Color::White},
        {TextureType::MediumBlock, "Assets/Texture/Cells/cell_medium_block_default.png", sf::Color::White},
        {TextureType::HardBlock, "Assets/Texture/Cells/cell_hard_block_default.png", sf::Color::White},
        {TextureType::A_Bomb, "Assets/Texture/Cells/cell_bomb_block_default.png", sf::Color(255, 100, 100)},
        {TextureType::MuteIcon, "Assets/Texture/Icon/Mute.png", sf::Color::White},
        {TextureType::TesseraLogo, "Assets/Texture/Logo/TesseraLogo.png", sf::Color::White},
        {TextureType::Button, "Assets/Texture/Menu/Button.png", sf::Color::White},
        {TextureType::ButtonActive, "Assets/Texture/Menu/ButtonActive.png", sf::Color::White}
    };
    bool useTextures = false;
    for (const auto& info : textureList) {
        sf::Texture texture;
        if (texture.loadFromFile(info.filename)) {
            textures[info.type] = texture;
            useTextures = true;
            std::cout << "Loaded texture: " << info.filename << std::endl;
        } else {
            std::cout << "Unable to load texture: " << info.filename << " - using fallback for this type" << std::endl;
        }
    }
    
    sf::Font titleFont;
    bool titleFontLoaded = false;
    for (int attempt = 0; attempt < 3 && !titleFontLoaded; ++attempt) {
        if (titleFont.openFromFile("Assets/Fonts/Righteous-Regular.ttf")) {
            titleFontLoaded = true;
            std::cout << "Font loaded successfully: Righteous-Regular.ttf" << std::endl;
        } else {
            std::cout << "Unable to load font: Assets/Fonts/Righteous-Regular.ttf (attempt " << (attempt + 1) << "/3)" << std::endl;
            if (attempt < 2) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    
    sf::Font menuFont;
    bool menuFontLoaded = false;
    for (int attempt = 0; attempt < 3 && !menuFontLoaded; ++attempt) {
        if (menuFont.openFromFile("Assets/Fonts/Righteous-Regular.ttf")) {
            menuFontLoaded = true;
            std::cout << "Font loaded successfully: Righteous-Regular.ttf" << std::endl;
        } else {
            std::cout << "Unable to load font: Assets/Fonts/Righteous-Regular.ttf (attempt " << (attempt + 1) << "/3)" << std::endl;
            if (attempt < 2) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    
    bool fontLoaded = titleFontLoaded && menuFontLoaded;
    

    AudioManager audioManager;
    audioManager.setMasterVolume(saveData.masterVolume);
    if (saveData.isMuted) {
        audioManager.toggleMute();
    }
    audioManager.loadAllAudio();

    
    std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> grid;
    PieceBag TesseraBag;
    
    std::vector<ExplosionEffect> explosionEffects;
    std::vector<GlowEffect> glowEffects;
    std::vector<BackgroundPiece> backgroundPieces;
    std::vector<BackgroundPiece> gameBackgroundPieces;
    std::vector<AchievementPopup> achievementPopups;
    std::vector<ThermometerParticle> thermometerParticles;
    std::vector<FallingCell> fallingCells;
    

    float displayedThermometerFill = 0.0f;
    
    float shakeIntensity = 0.0f;
    float shakeDuration = 0.0f;
    float shakeTimer = 0.0f;
    
    float backgroundSpawnTimer = 0.0f;
    const float BACKGROUND_SPAWN_INTERVAL = 0.02f;
    float gameBackgroundSpawnTimer = 0.0f;
    const float GAME_BACKGROUND_SPAWN_INTERVAL = 0.25f;

    GameState gameState = GameState::SplashScreen;
    MenuOption selectedMenuOption = MenuOption::Start;
    GameModeOption selectedGameModeOption = GameModeOption::Classic;
    ClassicDifficulty selectedClassicDifficulty = ClassicDifficulty::Hard;
    SprintLines selectedSprintLines = SprintLines::Lines24;
    ChallengeMode selectedChallengeMode = ChallengeMode::Randomness;
    PracticeDifficulty selectedPracticeDifficulty = PracticeDifficulty::Easy;
    PracticeLineGoal selectedPracticeLineGoal = PracticeLineGoal::Infinite;
    bool practiceInfiniteBombs = false;
    PracticeStartLevel selectedPracticeStartLevel = PracticeStartLevel::Level0;
    int selectedPracticeOption = 0;
    ExtrasOption selectedExtrasOption = ExtrasOption::Achievements;
    OptionsMenuOption selectedOptionsOption = OptionsMenuOption::Audio;
    AudioOption selectedAudioOption = AudioOption::MainVolume;
    int draggingAudioSlider = -1;
    PauseOption selectedPauseOption = PauseOption::Resume;
    ConfirmOption selectedConfirmOption = ConfirmOption::No;
    int hoveredAchievement = -1;
    

    GameThemeChoice selectedThemeChoice = static_cast<GameThemeChoice>(saveData.selectedTheme);
    GameThemeChoice hoveredThemeChoice = selectedThemeChoice;
    

    AbilityChoice selectedAbilityChoice = AbilityChoice::Bomb;
    AbilityChoice hoveredAbilityChoice = selectedAbilityChoice;
    bool isAbilitySelectorHovered = false;
    

    GameModeTheme currentTheme = GameThemes::getDefaultTheme();
    
    bool showCustomCursor = false;
    

    bool useKeyboardNavigation = false;
    sf::Vector2f lastMousePos(-1.0f, -1.0f);
    

    bool isThemeSelectorHovered = false;
    

    int splashSequenceStep = 0;
    float splashElapsedTime = 0.0f;
    const float FADE_DURATION = 1.0f;
    float blackScreenAlpha = 1.0f;
    
    ControlScheme selectedControlScheme = ControlScheme::Alternative;
    ControlScheme hoveredControlScheme = ControlScheme::Alternative;
    
    KeyBindings keyBindings;

    keyBindings.moveLeft = static_cast<sf::Keyboard::Key>(saveData.moveLeft);
    keyBindings.moveRight = static_cast<sf::Keyboard::Key>(saveData.moveRight);
    keyBindings.rotateLeft = static_cast<sf::Keyboard::Key>(saveData.rotateLeft);
    keyBindings.rotateRight = static_cast<sf::Keyboard::Key>(saveData.rotateRight);
    keyBindings.quickFall = static_cast<sf::Keyboard::Key>(saveData.quickFall);
    keyBindings.drop = static_cast<sf::Keyboard::Key>(saveData.drop);
    keyBindings.hold = static_cast<sf::Keyboard::Key>(saveData.hold);
    keyBindings.bomb = static_cast<sf::Keyboard::Key>(saveData.bomb);
    keyBindings.restart = static_cast<sf::Keyboard::Key>(saveData.restart);
    keyBindings.mute = static_cast<sf::Keyboard::Key>(saveData.mute);
    keyBindings.volumeDown = static_cast<sf::Keyboard::Key>(saveData.volumeDown);
    keyBindings.volumeUp = static_cast<sf::Keyboard::Key>(saveData.volumeUp);
    keyBindings.menu = static_cast<sf::Keyboard::Key>(saveData.menu);
    
    bool debugMode = false;


    struct NullStreamBuf final : std::streambuf {
        int overflow(int c) override { return c; }
    };
    static NullStreamBuf nullStreamBuf;
    static std::streambuf* originalCoutBuf = nullptr;
    if (!debugMode) {
        originalCoutBuf = std::cout.rdbuf(&nullStreamBuf);
    }
    
    bool showVolumeIndicator = false;
    float volumeIndicatorTimer = 0.0f;
    const float VOLUME_INDICATOR_DURATION = 2.0f;
    
    int selectedRebindingIndex = 0;
    bool waitingForKeyPress = false;
    const int MAX_REBINDING_OPTIONS = 12;
    ControlScheme rebindingSelectedScheme = ControlScheme::Classic;
    ControlScheme rebindingHoveredScheme = ControlScheme::Classic;
    ControlScheme rebindingAppliedScheme = ControlScheme::Classic;
    
    int totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
    int currentLevel = 0;
    int totalScore = 0;
    bool gameOver = false;
    bool gameOverSoundPlayed = false;
    float gameOverMusicTimer = 0.0f;
    const float GAME_OVER_MUSIC_DELAY = 2.0f;
    float gameOverDelayTimer = 0.0f;
    const float GAME_OVER_PAUSE_DELAY = 1.5f;
    const float GAME_OVER_FALL_DELAY = 1.5f;
    bool gameOverScreenVisible = false;
    bool gameOverBlocksFalling = false;
    bool gameOverPauseComplete = false;
    float gameOverUiFadeTimer = 0.0f;
    const float GAME_OVER_UI_FADE_DURATION = 0.25f;
    bool gameOverNewHighScore = false;
    int gameOverHighScoreBaseline = 0;
    

    bool hasCollisionPiece = false;
    PieceType collisionPieceType = PieceType::I_Basic;
    int collisionPieceX = 0;
    int collisionPieceY = 0;
    

    float sessionPlayTime = 0.0f;
    int sessionPiecesPlaced = 0;
    

    #define RESET_SESSION_STATS() do { sessionPlayTime = 0.0f; sessionPiecesPlaced = 0; } while(0)
    

    const DifficultyConfig* currentConfig = nullptr;

    float sprintTimer = 0.0f;
    int sprintTargetLines = 0;
    bool sprintModeActive = false;
    bool sprintCompleted = false;
    bool challengeModeActive = false;
    bool practiceModeActive = false;
    

    bool gravityFlipped = false;
    int gravityFlipPieceCount = 0;
    
    float autoDropTimer = 0.0f;
    const float AUTO_DROP_INTERVAL = 1.337f;
    
    float hardDropCooldown = 0.0f;
    const float HARD_DROP_COOLDOWN_TIME = 0.1f;
    
    int linesSinceLastAbility = 0;
    bool bombAbilityAvailable = false;
    bool deliveryAbilityAvailable = false;
    bool stompAbilityAvailable = false;
    const int LINES_FOR_DELIVERY = 10;
    const int LINES_FOR_BOMB = 10;
    const int LINES_FOR_STOMP = 10;
    
    int currentCombo = 0;
    float displayCombo = 0.0f;
    float comboAnimTimer = 0.0f;
    const float COMBO_ANIM_SPEED = 15.0f;
    float comboFadeScale = 0.0f;
    bool comboFadingOut = false;
    const float COMBO_FADE_SPEED = 24.0f;
    const int COMBO_BONUS_PER_LINE = 250;
    int lastMoveScore = 0;
    
    int totalHardDropScore = 0;
    int totalLineScore = 0;
    int totalComboScore = 0;
    const int HARD_DROP_POINTS_PER_CELL = 5;
    

    bool bombUsedThisGame = false;
    int maxComboThisGame = 0;
    int consecutiveBombsUsed = 0;
    

    PieceType heldPiece = PieceType::I_Basic;
    bool hasHeldPiece = false;
    bool canUseHold = true;
    

    int currentPieceRotations = 0;
    

    float leftHoldTime = 0.0f;
    float rightHoldTime = 0.0f;
    const float DAS_DELAY = 0.2f;
    const float DAS_REPEAT = 0.04f;
    float dasTimer = 0.0f;
    bool leftPressed = false;
    bool rightPressed = false;
    
    PieceType initType = TesseraBag.getNextPiece();
    std::cout << "Initial spawn: " << pieceTypeToString(initType) << " (Bag System)" << std::endl;
    PieceShape initShape = getPieceShape(initType);
    int startX = (GRID_WIDTH - initShape.width) / 2;
    int firstFilledRow = findFirstFilledRow(initShape);
    int startY = -firstFilledRow;
    Piece activePiece(startX, startY, initType);
    sf::Clock clock;
    bool firstFrame = true;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        if (firstFrame) { window.requestFocus(); firstFrame = false; }
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) { window.close(); }
            

            if (event->is<sf::Event::MouseMoved>()) {
                if (gameState != GameState::SplashScreen && gameState != GameState::Playing) {
                    showCustomCursor = true;
                }
                
                if (gameState == GameState::FirstTimeSetup) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
                    float mouseX = mousePos.x;
                    float mouseY = mousePos.y;
                    float centerX = WINDOW_WIDTH / 2.0f;
                    float centerY = WINDOW_HEIGHT / 2.0f;
                    

                    if (mouseX >= centerX - 550 && mouseX <= centerX + 50 &&
                        mouseY >= centerY - 150 && mouseY <= centerY + 50) {
                        hoveredControlScheme = ControlScheme::Classic;
                    }

                    else if (mouseX >= centerX - 50 && mouseX <= centerX + 550 &&
                             mouseY >= centerY - 150 && mouseY <= centerY + 50) {
                        hoveredControlScheme = ControlScheme::Alternative;
                    }
                }
                
                if (gameState == GameState::Rebinding) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
                    float mouseX = mousePos.x;
                    float mouseY = mousePos.y;
                    float centerX = WINDOW_WIDTH / 2.0f;
                    
                    float buttonY = 180.0f;
                    float buttonSpacing = 350.0f;
                    

                    if (mouseX >= centerX - buttonSpacing - 150 && mouseX <= centerX - buttonSpacing + 150 &&
                        mouseY >= buttonY - 40 && mouseY <= buttonY + 60) {
                        rebindingHoveredScheme = ControlScheme::Classic;
                    }
                    else if (mouseX >= centerX - 150 && mouseX <= centerX + 150 &&
                             mouseY >= buttonY - 40 && mouseY <= buttonY + 60) {
                        rebindingHoveredScheme = ControlScheme::Alternative;
                    }
                    else if (mouseX >= centerX + buttonSpacing - 150 && mouseX <= centerX + buttonSpacing + 150 &&
                             mouseY >= buttonY - 40 && mouseY <= buttonY + 60) {
                        rebindingHoveredScheme = ControlScheme::Custom;
                    }
                    else {
                        rebindingHoveredScheme = rebindingSelectedScheme;
                    }
                }
                

                if (gameState == GameState::AudioSettings && draggingAudioSlider >= 0) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
                    float mouseX = mousePos.x;
                    
                    float centerX = WINDOW_WIDTH / 2.0f;
                    float sliderCenterX = centerX + 100;
                    float sliderLeft = sliderCenterX - MenuConfig::AudioMenu::SLIDER_WIDTH / 2.0f;
                    float sliderRight = sliderLeft + MenuConfig::AudioMenu::SLIDER_WIDTH;
                    
                    float clampedX = std::max(sliderLeft, std::min(mouseX, sliderRight));
                    float newVolume = ((clampedX - sliderLeft) / MenuConfig::AudioMenu::SLIDER_WIDTH) * 100.0f;
                    
                    if (draggingAudioSlider == 0) {
                        audioManager.setMasterVolume(newVolume);
                        saveData.masterVolume = newVolume;
                    } else if (draggingAudioSlider == 1) {
                        audioManager.setMusicVolume(newVolume);
                        saveData.musicVolume = newVolume;
                    } else if (draggingAudioSlider == 2) {
                        audioManager.setSfxVolume(newVolume);
                        saveData.sfxVolume = newVolume;
                    }
                }
                

                if (gameState == GameState::ClassicDifficultySelect) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
                    float mouseX = mousePos.x;
                    float mouseY = mousePos.y;
                    
                    float panelX = 1400;
                    float panelY = 450;
                    float themeHeight = 70;
                    
                    isThemeSelectorHovered = false;
                    
                    if (mouseX >= panelX && mouseX <= panelX + 220) {
                        for (int i = 0; i < 3; i++) {
                            float yPos = panelY + 35 + i * themeHeight;
                            if (mouseY >= yPos && mouseY <= yPos + themeHeight - 10) {
                                isThemeSelectorHovered = true;
                                hoveredThemeChoice = static_cast<GameThemeChoice>(i);
                                break;
                            }
                        }
                    }
                    

                    float abilityPanelX = 300;
                    float abilityPanelY = 450;
                    float abilityHeight = 70;
                    
                    isAbilitySelectorHovered = false;
                    
                    if (mouseX >= abilityPanelX && mouseX <= abilityPanelX + 220) {
                        for (int i = 0; i < 3; i++) {
                            float yPos = abilityPanelY + 35 + i * abilityHeight;
                            if (mouseY >= yPos && mouseY <= yPos + abilityHeight - 10) {
                                isAbilitySelectorHovered = true;
                                hoveredAbilityChoice = static_cast<AbilityChoice>(i);
                                break;
                            }
                        }
                    }
                }
            }
            
            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    useKeyboardNavigation = false;
                    if (gameState == GameState::SplashScreen) {

                        constexpr int CURRENT_SETUP_VERSION = 1;
                        audioManager.stopAllMusic();
                        if (saveData.setupVersion < CURRENT_SETUP_VERSION) {
                            gameState = GameState::FirstTimeSetup;
                            std::cout << "Splash screen skipped - showing FirstTimeSetup (mouse)" << std::endl;
                        } else {
                            gameState = GameState::MainMenu;
                            audioManager.playMenuMusic();
                            std::cout << "Splash screen skipped (mouse)" << std::endl;
                        }
                        showCustomCursor = true;
                    } else if (gameState == GameState::FirstTimeSetup) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        
                        constexpr int CURRENT_SETUP_VERSION = 1;
                        bool buttonClicked = false;
                        

                        if (clickX >= centerX - 400 && clickX <= centerX - 100 &&
                            clickY >= centerY - 50 && clickY <= centerY + 10) {
                            selectedControlScheme = ControlScheme::Classic;
                            buttonClicked = true;
                        }

                        else if (clickX >= centerX + 100 && clickX <= centerX + 400 &&
                                 clickY >= centerY - 50 && clickY <= centerY + 10) {
                            selectedControlScheme = ControlScheme::Alternative;
                            buttonClicked = true;
                        }
                        
                        if (buttonClicked) {
                            audioManager.playMenuClickSound();
                            
                            if (selectedControlScheme == ControlScheme::Classic) {
                                keyBindings.moveLeft = sf::Keyboard::Key::Left;
                                keyBindings.moveRight = sf::Keyboard::Key::Right;
                                keyBindings.rotateLeft = sf::Keyboard::Key::LControl;
                                keyBindings.rotateRight = sf::Keyboard::Key::Up;
                                keyBindings.quickFall = sf::Keyboard::Key::Down;
                                keyBindings.drop = sf::Keyboard::Key::Space;
                                keyBindings.hold = sf::Keyboard::Key::LShift;
                                keyBindings.bomb = sf::Keyboard::Key::Z;
                                
                                saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::Left);
                                saveData.moveRight = static_cast<int>(sf::Keyboard::Key::Right);
                                saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::LControl);
                                saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::Up);
                                saveData.quickFall = static_cast<int>(sf::Keyboard::Key::Down);
                                saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                                saveData.hold = static_cast<int>(sf::Keyboard::Key::LShift);
                                saveData.bomb = static_cast<int>(sf::Keyboard::Key::Z);
                                
                                std::cout << "Applied Classic controls (mouse)" << std::endl;
                            } else {
                                keyBindings.moveLeft = sf::Keyboard::Key::A;
                                keyBindings.moveRight = sf::Keyboard::Key::D;
                                keyBindings.rotateLeft = sf::Keyboard::Key::J;
                                keyBindings.rotateRight = sf::Keyboard::Key::K;
                                keyBindings.quickFall = sf::Keyboard::Key::S;
                                keyBindings.drop = sf::Keyboard::Key::Space;
                                keyBindings.hold = sf::Keyboard::Key::L;
                                keyBindings.bomb = sf::Keyboard::Key::I;
                                
                                saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::A);
                                saveData.moveRight = static_cast<int>(sf::Keyboard::Key::D);
                                saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::J);
                                saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::K);
                                saveData.quickFall = static_cast<int>(sf::Keyboard::Key::S);
                                saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                                saveData.hold = static_cast<int>(sf::Keyboard::Key::L);
                                saveData.bomb = static_cast<int>(sf::Keyboard::Key::I);
                                
                                std::cout << "Applied Alternative controls (mouse)" << std::endl;
                            }
                            
                            saveData.setupVersion = CURRENT_SETUP_VERSION;
                            saveGameData(saveData);
                            gameState = GameState::WelcomeScreen;
                            std::cout << "First time setup completed (mouse), showing welcome screen" << std::endl;
                        }
                    } else if (gameState == GameState::WelcomeScreen) {

                        gameState = GameState::MainMenu;
                        audioManager.playMenuMusic();
                        showCustomCursor = true;
                        std::cout << "Welcome screen skipped (mouse)" << std::endl;
                    } else if (gameState == GameState::MainMenu) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {

                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            

                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }

                        else {



                            float startWidth = calculateButtonWidth(menuFont, "START", 86.0f) / 2;
                            float extrasWidth = calculateButtonWidth(menuFont, "EXTRAS", 86.0f) / 2;
                            float optionsWidth = calculateButtonWidth(menuFont, "OPTIONS", 86.0f) / 2;
                            float exitWidth = calculateButtonWidth(menuFont, "EXIT", 86.0f) / 2;
                            float buttonHeight = 80.0f;
                            

                            std::cout << "CLICK: x=" << clickX << ", y=" << clickY << std::endl;
                            std::cout << "START hitbox: x=" << (centerX - startWidth) << " to " << (centerX + startWidth) 
                                      << ", y=" << (centerY - 60) << " to " << (centerY - 60 + buttonHeight) << std::endl;

                            if (clickX >= centerX - startWidth && clickX <= centerX + startWidth &&
                                clickY >= centerY - 60 && clickY <= centerY - 60 + buttonHeight) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::GameModeSelect;
                                selectedGameModeOption = GameModeOption::Classic;
                                std::cout << "Entered GAME MODE selection (mouse)" << std::endl;
                            }

                            else if (clickX >= centerX - extrasWidth && clickX <= centerX + extrasWidth &&
                                     clickY >= centerY + 50 && clickY <= centerY + 50 + buttonHeight) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::Extras;
                                selectedExtrasOption = ExtrasOption::Achievements;
                                std::cout << "Entered EXTRAS menu (mouse)" << std::endl;
                            }

                            else if (clickX >= centerX - optionsWidth && clickX <= centerX + optionsWidth &&
                                     clickY >= centerY + 160 && clickY <= centerY + 160 + buttonHeight) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::Options;
                                std::cout << "Entered OPTIONS menu (mouse)" << std::endl;
                            }

                            else if (clickX >= centerX - exitWidth && clickX <= centerX + exitWidth &&
                                     clickY >= centerY + 270 && clickY <= centerY + 270 + buttonHeight) {
                                window.close();
                            }
                        }
                    } else if (gameState == GameState::GameModeSelect) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }
                        else {
                            float startY = centerY - 80.0f;
                            float spacing = 90.0f;
                            float buttonHeight = 80.0f;
                            

                            std::string options[3] = {"CLASSIC", "BLITZ", "CHALLENGE"};

                            for (int i = 0; i < 3; i++) {
                                float selectorY = startY + i * spacing - 5;
                                float halfWidth = calculateButtonWidth(menuFont, options[i], 86.0f) / 2;
                                if (clickX >= centerX - halfWidth && clickX <= centerX + halfWidth &&
                                    clickY >= selectorY && clickY <= selectorY + buttonHeight) {
                                    GameModeOption clickedMode = static_cast<GameModeOption>(i);
                                    
                                    audioManager.playMenuClickSound();
                                    if (clickedMode == GameModeOption::Classic) {
                                        gameState = GameState::ClassicDifficultySelect;
                                        selectedClassicDifficulty = ClassicDifficulty::Hard;
                                        std::cout << "Entered CLASSIC difficulty selection (mouse)" << std::endl;
                                    } else if (clickedMode == GameModeOption::Sprint) {
                                        gameState = GameState::SprintLinesSelect;
                                        selectedSprintLines = SprintLines::Lines24;
                                        std::cout << "Entered BLITZ lines selection (mouse)" << std::endl;
                                    } else if (clickedMode == GameModeOption::Challenge) {
                                        gameState = GameState::ChallengeSelect;
                                        selectedChallengeMode = debugMode ? ChallengeMode::Debug : ChallengeMode::Randomness;
                                        std::cout << "Entered CHALLENGE selection (mouse)" << std::endl;
                                    }
                                    break;
                                }
                            }
                        }
                    } else if (gameState == GameState::PracticeSelect) {


                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        
                        float startY = centerY - 220.0f;
                        float spacing = 85.0f;
                        float startButtonY = startY + spacing * 4 + 25;
                        float buttonHeight = 80.0f;
                        

                        bool clickedOption = false;
                        for (int i = 0; i < 4; i++) {
                            float selectorY = startY + i * spacing - 5;
                            if (clickX >= centerX - 350 && clickX <= centerX + 350 &&
                                clickY >= selectorY && clickY <= selectorY + buttonHeight) {
                                audioManager.playMenuClickSound();
                                
                                if (i == 0) {
                                    selectedPracticeDifficulty = static_cast<PracticeDifficulty>(
                                        (static_cast<int>(selectedPracticeDifficulty) + 1) % 4
                                    );
                                } else if (i == 1) {
                                    selectedPracticeLineGoal = static_cast<PracticeLineGoal>(
                                        (static_cast<int>(selectedPracticeLineGoal) + 1) % 4
                                    );
                                } else if (i == 2) {
                                    practiceInfiniteBombs = !practiceInfiniteBombs;
                                } else if (i == 3) {
                                    selectedPracticeStartLevel = static_cast<PracticeStartLevel>(
                                        (static_cast<int>(selectedPracticeStartLevel) + 1) % 11
                                    );
                                }
                                clickedOption = true;
                                break;
                            }
                        }
                        

                        if (!clickedOption && selectedPracticeOption == 4) {
                            float startPracticeHalfWidth = calculateButtonWidth(menuFont, "START PRACTICE", 86.0f) / 2;
                            if (clickX >= centerX - startPracticeHalfWidth && clickX <= centerX + startPracticeHalfWidth &&
                                clickY >= startButtonY && clickY <= startButtonY + buttonHeight) {
                                
                            applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                            audioManager.playMenuClickSound();
                            
                            gameState = GameState::Playing;
                            showCustomCursor = false;
                            gameOver = false;
                            gameOverSoundPlayed = false;
                            gameOverMusicTimer = 0.0f;
                            
                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            consecutiveBombsUsed = 0;
                            hardDropCooldown = 0.0f;
                            
                            practiceModeActive = true;
                            sprintModeActive = false;
                            challengeModeActive = false;
                            sprintCompleted = false;
                            sprintTimer = 0.0f;
                            autoDropTimer = 0.0f;
                            
                            const DifficultyConfig* config = getDifficultyConfig(
                                selectedGameModeOption,
                                selectedClassicDifficulty,
                                selectedSprintLines,
                                selectedChallengeMode,
                                selectedPracticeDifficulty,
                                selectedPracticeLineGoal,
                                practiceInfiniteBombs
                            );
                            TesseraBag.setDifficultyConfig(config);
                            currentConfig = config;
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            

                            currentLevel = static_cast<int>(selectedPracticeStartLevel);
                            totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                            
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            TesseraBag.reset(currentLevel);
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = practiceInfiniteBombs ? true : debugMode;
                            deliveryAbilityAvailable = false;
                            explosionEffects.clear();
                            glowEffects.clear();
                            thermometerParticles.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            
                            PieceType firstType = TesseraBag.getNextPiece();
                            PieceShape firstShape = getPieceShape(firstType);
                            int spawnX = (GRID_WIDTH - firstShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(firstShape);
                            int spawnY = -firstFilledRow;
                            
                            activePiece = Piece(spawnX, spawnY, firstType);
                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(firstType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomColorIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomColorIndex]);
                            }
                            
                            std::cout << "Practice mode started with difficulty: " << static_cast<int>(selectedPracticeDifficulty) 
                                     << ", line goal: " << static_cast<int>(selectedPracticeLineGoal) 
                                     << ", infinite bombs: " << (practiceInfiniteBombs ? "YES" : "NO") << std::endl;
                        }
                    } else if (gameState == GameState::ClassicDifficultySelect || 
                               gameState == GameState::SprintLinesSelect || 
                               gameState == GameState::ChallengeSelect) {

                        applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                        
                        gameState = GameState::Playing;
                        showCustomCursor = false;
                        gameOver = false;
                        gameOverSoundPlayed = false;
                        gameOverMusicTimer = 0.0f;
                        

                        bombUsedThisGame = false;
                        maxComboThisGame = 0;
                        consecutiveBombsUsed = 0;
                        hardDropCooldown = 0.0f;
                        

                        if (selectedGameModeOption == GameModeOption::Sprint) {
                            sprintModeActive = true;
                            challengeModeActive = false;
                            sprintCompleted = false;
                            sprintTimer = 0.0f;
                            autoDropTimer = 0.0f;
                            switch (selectedSprintLines) {
                                case SprintLines::Lines24:
                                    sprintTargetLines = 24;
                                    break;
                                case SprintLines::Lines48:
                                    sprintTargetLines = 48;
                                    break;
                                case SprintLines::Lines96:
                                    sprintTargetLines = 96;
                                    break;
                                default:
                                    sprintTargetLines = 24;
                                    break;
                            }
                        } else if (selectedGameModeOption == GameModeOption::Challenge) {
                            challengeModeActive = true;
                            sprintModeActive = false;
                            practiceModeActive = false;
                            sprintCompleted = false;
                            sprintTimer = 0.0f;
                            autoDropTimer = 0.0f;
                            gravityFlipped = false;
                            gravityFlipPieceCount = 0;
                        } else if (selectedGameModeOption == GameModeOption::Practice) {
                            practiceModeActive = true;
                            sprintModeActive = false;
                            challengeModeActive = false;
                            sprintCompleted = false;
                            sprintTimer = 0.0f;
                            autoDropTimer = 0.0f;
                        } else {
                            sprintModeActive = false;
                            challengeModeActive = false;
                            practiceModeActive = false;
                            sprintCompleted = false;
                        }
                        

                        const DifficultyConfig* config = getDifficultyConfig(
                            selectedGameModeOption,
                            selectedClassicDifficulty,
                            selectedSprintLines,
                            selectedChallengeMode,
                            selectedPracticeDifficulty,
                            selectedPracticeLineGoal,
                            practiceInfiniteBombs
                        );
                        TesseraBag.setDifficultyConfig(config);
                        currentConfig = config;
                        
                        for (int i = 0; i < GRID_HEIGHT; ++i) {
                            for (int j = 0; j < GRID_WIDTH; ++j) {
                                grid[i][j] = Cell();
                            }
                        }
                        totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                        currentLevel = 0;
                        totalScore = 0;
                        currentCombo = 0;
                        lastMoveScore = 0;
                        totalHardDropScore = 0;
                        totalLineScore = 0;
                        totalComboScore = 0;
                        RESET_SESSION_STATS();
                        TesseraBag.reset();
                        hasHeldPiece = false;
                        canUseHold = true;
                        linesSinceLastAbility = 0;
                        bombAbilityAvailable = debugMode;
                        deliveryAbilityAvailable = false;
                        explosionEffects.clear();
                        glowEffects.clear();
                            thermometerParticles.clear();
                        leftHoldTime = 0.0f;
                        rightHoldTime = 0.0f;
                        dasTimer = 0.0f;
                        leftPressed = false;
                        rightPressed = false;
                        
                        PieceType startType = TesseraBag.getNextPiece();
                        PieceShape startShape = getPieceShape(startType);
                        int startX = (GRID_WIDTH - startShape.width) / 2;
                        int firstFilledRow = findFirstFilledRow(startShape);
                        int startY = -firstFilledRow;
                        

                        if (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) {
                            startY = GRID_HEIGHT / 2 - startShape.height / 2;
                            gravityFlipPieceCount = 1;
                            gravityFlipped = false;
                        }
                        
                        activePiece = Piece(startX, startY, startType);
                        

                        if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                            int colorIndex = getColorIndexForPieceType(startType);
                            if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                            }
                        } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                            int randomIndex = rand() % currentConfig->colorPalette.size();
                            activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                        }
                        
                        std::cout << "Game started (mouse) with mode: " << config->modeName << std::endl;
                            }
                    } else if (gameState == GameState::Extras) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }
                        else {
                            float startY = centerY + MenuConfig::ExtrasMenu::START_Y_OFFSET;
                            float spacing = MenuConfig::ExtrasMenu::SPACING;
                            float buttonHeight = 80.0f;

                            for (int i = 0; i < MenuConfig::ExtrasMenu::NUM_OPTIONS; i++) {
                                float selectorY = startY + i * spacing - 5;
                                float halfWidth = calculateButtonWidth(menuFont, MenuConfig::getExtrasOptionText(i), 86.0f) / 2;
                                if (clickX >= centerX - halfWidth && clickX <= centerX + halfWidth &&
                                    clickY >= selectorY && clickY <= selectorY + buttonHeight) {
                                    ExtrasOption clickedOption = static_cast<ExtrasOption>(i);
                                    
                                    if (clickedOption == ExtrasOption::Achievements) {
                                        audioManager.playMenuClickSound();
                                        gameState = GameState::AchievementsView;
                                        std::cout << "Entered ACHIEVEMENTS view (mouse)" << std::endl;
                                    } else if (clickedOption == ExtrasOption::Statistics) {
                                        audioManager.playMenuClickSound();
                                        gameState = GameState::StatisticsView;
                                        std::cout << "Entered STATISTICS view (mouse)" << std::endl;
                                    } else if (clickedOption == ExtrasOption::BestScores) {
                                        audioManager.playMenuClickSound();
                                        gameState = GameState::BestScoresView;
                                        std::cout << "Entered BEST SCORES view (mouse)" << std::endl;
                                    }
                                    break;
                                }
                            }
                        }
                    } else if (gameState == GameState::Options) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }
                        else {
                            float buttonHeight = 80.0f;
                            float audioHalfWidth = calculateButtonWidth(menuFont, MenuConfig::OptionsMenu::AUDIO, 86.0f) / 2;
                            float rebindKeysHalfWidth = calculateButtonWidth(menuFont, MenuConfig::OptionsMenu::REBIND_KEYS, 86.0f) / 2;
                            float clearDataHalfWidth = calculateButtonWidth(menuFont, MenuConfig::OptionsMenu::CLEAR_ALL_DATA, 86.0f) / 2;


                            if (clickX >= centerX - audioHalfWidth && clickX <= centerX + audioHalfWidth &&
                                clickY >= centerY + MenuConfig::OptionsMenu::OPTION1_Y_OFFSET && 
                                clickY <= centerY + MenuConfig::OptionsMenu::OPTION1_Y_OFFSET + buttonHeight) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::AudioSettings;
                                selectedAudioOption = AudioOption::MainVolume;
                                std::cout << "Opening AUDIO menu (mouse)" << std::endl;
                            }
                            

                            else if (clickX >= centerX - rebindKeysHalfWidth && clickX <= centerX + rebindKeysHalfWidth &&
                                     clickY >= centerY + MenuConfig::OptionsMenu::OPTION2_Y_OFFSET && 
                                     clickY <= centerY + MenuConfig::OptionsMenu::OPTION2_Y_OFFSET + buttonHeight) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::Rebinding;
                                std::cout << "Entering REBIND KEYS menu (mouse)" << std::endl;
                            }
                            

                            else if (clickX >= centerX - clearDataHalfWidth && clickX <= centerX + clearDataHalfWidth &&
                                     clickY >= centerY + MenuConfig::OptionsMenu::OPTION3_Y_OFFSET && 
                                     clickY <= centerY + MenuConfig::OptionsMenu::OPTION3_Y_OFFSET + buttonHeight) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::ConfirmClearScores;
                                selectedConfirmOption = ConfirmOption::No;
                                std::cout << "Opening confirmation dialog (mouse)" << std::endl;
                            }
                        }
                    } else if (gameState == GameState::AudioSettings) {
                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        
                        float sliderCenterX = centerX + 100;
                        float sliderHitHeight = MenuConfig::AudioMenu::SLIDER_HANDLE_HEIGHT + 20;
                        float sliderLeft = sliderCenterX - MenuConfig::AudioMenu::SLIDER_WIDTH/2;
                        float sliderRight = sliderCenterX + MenuConfig::AudioMenu::SLIDER_WIDTH/2;
                        

                        auto calculateVolumeFromClick = [&](float clickX) -> float {
                            float relativeX = clickX - sliderLeft;
                            float percentage = (relativeX / MenuConfig::AudioMenu::SLIDER_WIDTH) * 100.0f;
                            return std::max(0.0f, std::min(100.0f, percentage));
                        };
                        

                        if (clickX >= sliderLeft && clickX <= sliderRight &&
                            clickY >= centerY + MenuConfig::AudioMenu::OPTION1_Y_OFFSET - sliderHitHeight/2 && 
                            clickY <= centerY + MenuConfig::AudioMenu::OPTION1_Y_OFFSET + sliderHitHeight/2) {
                            float newVol = calculateVolumeFromClick(clickX);
                            audioManager.setMasterVolume(newVol);
                            saveData.masterVolume = newVol;
                            selectedAudioOption = AudioOption::MainVolume;
                            draggingAudioSlider = 0;
                        }

                        else if (clickX >= sliderLeft && clickX <= sliderRight &&
                                 clickY >= centerY + MenuConfig::AudioMenu::OPTION2_Y_OFFSET - sliderHitHeight/2 && 
                                 clickY <= centerY + MenuConfig::AudioMenu::OPTION2_Y_OFFSET + sliderHitHeight/2) {
                            float newVol = calculateVolumeFromClick(clickX);
                            audioManager.setMusicVolume(newVol);
                            saveData.musicVolume = newVol;
                            selectedAudioOption = AudioOption::MusicVolume;
                            draggingAudioSlider = 1;
                        }

                        else if (clickX >= sliderLeft && clickX <= sliderRight &&
                                 clickY >= centerY + MenuConfig::AudioMenu::OPTION3_Y_OFFSET - sliderHitHeight/2 && 
                                 clickY <= centerY + MenuConfig::AudioMenu::OPTION3_Y_OFFSET + sliderHitHeight/2) {
                            float newVol = calculateVolumeFromClick(clickX);
                            audioManager.setSfxVolume(newVol);
                            saveData.sfxVolume = newVol;
                            selectedAudioOption = AudioOption::SfxVolume;
                            draggingAudioSlider = 2;
                        }
                    } else if (gameState == GameState::AchievementsView) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }

                    } else if (gameState == GameState::StatisticsView) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }

                    } else if (gameState == GameState::BestScoresView) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }

                    } else if (gameState == GameState::Paused) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        

                        if (clickX >= centerX - 150 && clickX <= centerX + 150 &&
                            clickY >= centerY - 50 && clickY <= centerY) {
                            audioManager.playMenuClickSound();
                            gameState = GameState::Playing;
                            showCustomCursor = false;
                            std::cout << "Game resumed (mouse)" << std::endl;
                        }

                        else if (clickX >= centerX - 150 && clickX <= centerX + 150 &&
                                 clickY >= centerY + 20 && clickY <= centerY + 70) {
                            audioManager.playMenuClickSound();
                            applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                            
                            gameState = GameState::Playing;
                            showCustomCursor = false;
                            gameOver = false;
                            gameOverSoundPlayed = false;
                            gameOverMusicTimer = 0.0f;
                            

                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            consecutiveBombsUsed = 0;
                            hardDropCooldown = 0.0f;
                            

                            if (sprintModeActive || challengeModeActive) {
                                sprintTimer = 0.0f;
                                autoDropTimer = 0.0f;
                                sprintCompleted = false;
                            }
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            TesseraBag.reset();
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = false;
                            explosionEffects.clear();
                            glowEffects.clear();
                            thermometerParticles.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            
                            PieceType startType = TesseraBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            activePiece = Piece(startX, 0, startType);
                            

                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(startType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                            }
                            
                            std::cout << "Game restarted (mouse)" << std::endl;
                        }

                        else if (clickX >= centerX - 150 && clickX <= centerX + 150 &&
                                 clickY >= centerY + 90 && clickY <= centerY + 140) {
                            audioManager.playMenuClickSound();
                            audioManager.switchToMenuMusic();
                            
                            gameState = GameState::MainMenu;
                            selectedMenuOption = MenuOption::Start;
                            std::cout << "Returned to main menu (mouse)" << std::endl;
                        }
                    } else if (gameState == GameState::ClassicDifficultySelect) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        } else {

                            float startY = centerY - 80.0f;
                            float spacing = 90.0f;
                            float buttonHeight = 80.0f;
                            float buttonWidth = 550.0f;
                            
                            for (int i = 0; i < 3; i++) {
                                float selectorY = startY + i * spacing - 5;
                                if (clickX >= centerX - buttonWidth/2 && clickX <= centerX + buttonWidth/2 &&
                                    clickY >= selectorY && clickY <= selectorY + buttonHeight) {


                                    ClassicDifficulty menuToDifficulty[] = {ClassicDifficulty::Hard, ClassicDifficulty::Normal};
                                    selectedClassicDifficulty = menuToDifficulty[i];
                                    audioManager.playMenuClickSound();
                                    

                                    const DifficultyConfig* config = getDifficultyConfig(
                                        selectedGameModeOption,
                                        selectedClassicDifficulty,
                                        selectedSprintLines,
                                        selectedChallengeMode,
                                        selectedPracticeDifficulty,
                                        selectedPracticeLineGoal,
                                        practiceInfiniteBombs
                                    );
                                    TesseraBag.setDifficultyConfig(config);
                                    currentConfig = config;
                                    
                                    applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                                    gameState = GameState::Playing;
                                    showCustomCursor = false;
                                    gameOver = false;
                                    gameOverSoundPlayed = false;
                                    gameOverMusicTimer = 0.0f;
                                    sprintModeActive = false;
                                    challengeModeActive = false;
                                    practiceModeActive = false;
                                    sprintCompleted = false;
                                    bombUsedThisGame = false;
                                    maxComboThisGame = 0;
                                    consecutiveBombsUsed = 0;
                                    hardDropCooldown = 0.0f;
                                    
                                    for (int row = 0; row < GRID_HEIGHT; ++row) {
                                        for (int col = 0; col < GRID_WIDTH; ++col) {
                                            grid[row][col] = Cell();
                                        }
                                    }
                                    totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                                    currentLevel = 0;
                                    totalScore = 0;
                                    currentCombo = 0;
                                    lastMoveScore = 0;
                                    totalHardDropScore = 0;
                                    totalLineScore = 0;
                                    totalComboScore = 0;
                                    RESET_SESSION_STATS();
                                    TesseraBag.reset();
                                    hasHeldPiece = false;
                                    canUseHold = true;
                                    linesSinceLastAbility = 0;
                                    bombAbilityAvailable = debugMode;
                                    explosionEffects.clear();
                                    glowEffects.clear();
                            thermometerParticles.clear();
                                    leftHoldTime = 0.0f;
                                    rightHoldTime = 0.0f;
                                    dasTimer = 0.0f;
                                    leftPressed = false;
                                    rightPressed = false;
                                    
                                    PieceType startType = TesseraBag.getNextPiece();
                                    PieceShape startShape = getPieceShape(startType);
                                    int startX = (GRID_WIDTH - startShape.width) / 2;
                                    int firstFilledRow = findFirstFilledRow(startShape);
                                    int startY = -firstFilledRow;
                                    activePiece = Piece(startX, startY, startType);
                                    
                                    if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                        int colorIndex = getColorIndexForPieceType(startType);
                                        if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                            activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                        }
                                    } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                        int randomIndex = rand() % currentConfig->colorPalette.size();
                                        activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                                    }
                                    std::cout << "Game started (Classic " << static_cast<int>(selectedClassicDifficulty) << ") - mouse click" << std::endl;
                                    break;
                                }
                            }
                        }

                    } else if (gameState == GameState::SprintLinesSelect) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        } else {

                            float startY = centerY - 80.0f;
                            float spacing = 90.0f;
                            float buttonHeight = 80.0f;
                            float buttonWidth = 550.0f;
                            int numOptions = 3;
                            
                            SprintLines sprintOptions[] = {SprintLines::Lines24, SprintLines::Lines48, SprintLines::Lines96};
                            int sprintTargets[] = {24, 48, 96};
                            
                            for (int i = 0; i < numOptions; i++) {
                                float selectorY = startY + i * spacing - 5;
                                if (clickX >= centerX - buttonWidth/2 && clickX <= centerX + buttonWidth/2 &&
                                    clickY >= selectorY && clickY <= selectorY + buttonHeight) {
                                    selectedSprintLines = sprintOptions[i];
                                    audioManager.playMenuClickSound();
                                    

                                    const DifficultyConfig* config = getDifficultyConfig(
                                        selectedGameModeOption,
                                        selectedClassicDifficulty,
                                        selectedSprintLines,
                                        selectedChallengeMode,
                                        selectedPracticeDifficulty,
                                        selectedPracticeLineGoal,
                                        practiceInfiniteBombs
                                    );
                                    TesseraBag.setDifficultyConfig(config);
                                    currentConfig = config;
                                    
                                    applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                                    gameState = GameState::Playing;
                                    showCustomCursor = false;
                                    gameOver = false;
                                    gameOverSoundPlayed = false;
                                    gameOverMusicTimer = 0.0f;
                                    sprintModeActive = true;
                                    challengeModeActive = false;
                                    practiceModeActive = false;
                                    sprintCompleted = false;
                                    sprintTimer = 0.0f;
                                    autoDropTimer = 0.0f;
                                    sprintTargetLines = sprintTargets[i];
                                    bombUsedThisGame = false;
                                    maxComboThisGame = 0;
                                    consecutiveBombsUsed = 0;
                                    hardDropCooldown = 0.0f;
                                    
                                    for (int row = 0; row < GRID_HEIGHT; ++row) {
                                        for (int col = 0; col < GRID_WIDTH; ++col) {
                                            grid[row][col] = Cell();
                                        }
                                    }
                                    totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                                    currentLevel = 0;
                                    totalScore = 0;
                                    currentCombo = 0;
                                    lastMoveScore = 0;
                                    totalHardDropScore = 0;
                                    totalLineScore = 0;
                                    totalComboScore = 0;
                                    RESET_SESSION_STATS();
                                    TesseraBag.reset();
                                    hasHeldPiece = false;
                                    canUseHold = true;
                                    linesSinceLastAbility = 0;
                                    bombAbilityAvailable = debugMode;
                                    explosionEffects.clear();
                                    glowEffects.clear();
                            thermometerParticles.clear();
                                    leftHoldTime = 0.0f;
                                    rightHoldTime = 0.0f;
                                    dasTimer = 0.0f;
                                    leftPressed = false;
                                    rightPressed = false;
                                    
                                    PieceType startType = TesseraBag.getNextPiece();
                                    PieceShape startShape = getPieceShape(startType);
                                    int startX = (GRID_WIDTH - startShape.width) / 2;
                                    int firstFilledRow = findFirstFilledRow(startShape);
                                    int startY = -firstFilledRow;
                                    activePiece = Piece(startX, startY, startType);
                                    
                                    if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                        int colorIndex = getColorIndexForPieceType(startType);
                                        if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                            activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                        }
                                    } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                        int randomIndex = rand() % currentConfig->colorPalette.size();
                                        activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                                    }
                                    std::cout << "Game started (Sprint " << sprintTargets[i] << ") - mouse click" << std::endl;
                                    break;
                                }
                            }
                        }

                    } else if (gameState == GameState::ChallengeSelect) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        float centerY = WINDOW_HEIGHT / 2.0f;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        } else {

                            float columnSpacing = 450.0f;
                            float startY = centerY - 150.0f;
                            float spacing = 100.0f;
                            float buttonHeight = 80.0f;
                            float buttonWidth = 400.0f;
                            
                            int numOptions = 9;
                            int startEnumValue = 1;
                            int itemsPerColumn = (numOptions + 1) / 2;
                            
                            float leftColumnX = centerX - columnSpacing / 2;
                            float rightColumnX = centerX + columnSpacing / 2;
                            
                            for (int i = 0; i < numOptions; i++) {
                                int column = i < itemsPerColumn ? 0 : 1;
                                int row = column == 0 ? i : i - itemsPerColumn;
                                float columnX = column == 0 ? leftColumnX : rightColumnX;
                                float buttonY = startY + row * spacing - 5;
                                
                                if (clickX >= columnX - buttonWidth/2 && clickX <= columnX + buttonWidth/2 &&
                                    clickY >= buttonY && clickY <= buttonY + buttonHeight) {
                                    selectedChallengeMode = static_cast<ChallengeMode>(i + startEnumValue);
                                    audioManager.playMenuClickSound();
                                    

                                    const DifficultyConfig* config = getDifficultyConfig(
                                        selectedGameModeOption,
                                        selectedClassicDifficulty,
                                        selectedSprintLines,
                                        selectedChallengeMode,
                                        selectedPracticeDifficulty,
                                        selectedPracticeLineGoal,
                                        practiceInfiniteBombs
                                    );
                                    TesseraBag.setDifficultyConfig(config);
                                    currentConfig = config;
                                    
                                    applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                                    gameState = GameState::Playing;
                                    showCustomCursor = false;
                                    gameOver = false;
                                    gameOverSoundPlayed = false;
                                    gameOverMusicTimer = 0.0f;
                                    sprintModeActive = false;
                                    challengeModeActive = true;
                                    practiceModeActive = false;
                                    sprintCompleted = false;
                                    sprintTimer = 0.0f;
                                    autoDropTimer = 0.0f;
                                    gravityFlipped = false;
                                    gravityFlipPieceCount = 0;
                                    bombUsedThisGame = false;
                                    maxComboThisGame = 0;
                                    consecutiveBombsUsed = 0;
                                    hardDropCooldown = 0.0f;
                                    
                                    for (int r = 0; r < GRID_HEIGHT; ++r) {
                                        for (int c = 0; c < GRID_WIDTH; ++c) {
                                            grid[r][c] = Cell();
                                        }
                                    }
                                    totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                                    currentLevel = 0;
                                    totalScore = 0;
                                    currentCombo = 0;
                                    lastMoveScore = 0;
                                    totalHardDropScore = 0;
                                    totalLineScore = 0;
                                    totalComboScore = 0;
                                    RESET_SESSION_STATS();
                                    TesseraBag.reset();
                                    hasHeldPiece = false;
                                    canUseHold = true;
                                    linesSinceLastAbility = 0;
                                    bombAbilityAvailable = debugMode;
                                    explosionEffects.clear();
                                    glowEffects.clear();
                            thermometerParticles.clear();
                                    leftHoldTime = 0.0f;
                                    rightHoldTime = 0.0f;
                                    dasTimer = 0.0f;
                                    leftPressed = false;
                                    rightPressed = false;
                                    
                                    PieceType startType = TesseraBag.getNextPiece();
                                    PieceShape startShape = getPieceShape(startType);
                                    int startX = (GRID_WIDTH - startShape.width) / 2;
                                    int firstFilledRow = findFirstFilledRow(startShape);
                                    int startY = -firstFilledRow;
                                    

                                    if (selectedChallengeMode == ChallengeMode::GravityFlip) {
                                        startY = GRID_HEIGHT / 2 - startShape.height / 2;
                                        gravityFlipPieceCount = 1;
                                        gravityFlipped = false;
                                    }
                                    
                                    activePiece = Piece(startX, startY, startType);
                                    
                                    if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                        int colorIndex = getColorIndexForPieceType(startType);
                                        if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                            activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                        }
                                    } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                        int randomIndex = rand() % currentConfig->colorPalette.size();
                                        activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                                    }
                                    std::cout << "Game started (Challenge " << static_cast<int>(selectedChallengeMode) << ") - mouse click" << std::endl;
                                    break;
                                }
                            }
                        }

                    } else if (gameState == GameState::Rebinding) {
                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        float centerX = WINDOW_WIDTH / 2.0f;
                        

                        float buttonY = 180.0f;
                        float buttonSpacing = 350.0f;
                        

                        if (clickX >= centerX - buttonSpacing - 160 && clickX <= centerX - buttonSpacing + 160 &&
                            clickY >= buttonY && clickY <= buttonY + 60) {
                            rebindingSelectedScheme = ControlScheme::Classic;
                            selectedRebindingIndex = 0;
                            waitingForKeyPress = false;
                            

                            keyBindings.moveLeft = sf::Keyboard::Key::Left;
                            keyBindings.moveRight = sf::Keyboard::Key::Right;
                            keyBindings.rotateLeft = sf::Keyboard::Key::LControl;
                            keyBindings.rotateRight = sf::Keyboard::Key::Up;
                            keyBindings.quickFall = sf::Keyboard::Key::Down;
                            keyBindings.drop = sf::Keyboard::Key::Space;
                            keyBindings.hold = sf::Keyboard::Key::LShift;
                            keyBindings.bomb = sf::Keyboard::Key::Z;
                            saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::Left);
                            saveData.moveRight = static_cast<int>(sf::Keyboard::Key::Right);
                            saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::LControl);
                            saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::Up);
                            saveData.quickFall = static_cast<int>(sf::Keyboard::Key::Down);
                            saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                            saveData.hold = static_cast<int>(sf::Keyboard::Key::LShift);
                            saveData.bomb = static_cast<int>(sf::Keyboard::Key::Z);
                            saveGameData(saveData);
                            rebindingAppliedScheme = ControlScheme::Classic;
                            audioManager.playMenuClickSound();
                            std::cout << "Classic controls applied and saved!" << std::endl;
                        }

                        else if (clickX >= centerX - 160 && clickX <= centerX + 160 &&
                                 clickY >= buttonY && clickY <= buttonY + 60) {
                            rebindingSelectedScheme = ControlScheme::Alternative;
                            selectedRebindingIndex = 0;
                            waitingForKeyPress = false;
                            

                            keyBindings.moveLeft = sf::Keyboard::Key::A;
                            keyBindings.moveRight = sf::Keyboard::Key::D;
                            keyBindings.rotateLeft = sf::Keyboard::Key::J;
                            keyBindings.rotateRight = sf::Keyboard::Key::K;
                            keyBindings.quickFall = sf::Keyboard::Key::S;
                            keyBindings.drop = sf::Keyboard::Key::Space;
                            keyBindings.hold = sf::Keyboard::Key::L;
                            keyBindings.bomb = sf::Keyboard::Key::I;
                            saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::A);
                            saveData.moveRight = static_cast<int>(sf::Keyboard::Key::D);
                            saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::J);
                            saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::K);
                            saveData.quickFall = static_cast<int>(sf::Keyboard::Key::S);
                            saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                            saveData.hold = static_cast<int>(sf::Keyboard::Key::L);
                            saveData.bomb = static_cast<int>(sf::Keyboard::Key::I);
                            saveGameData(saveData);
                            rebindingAppliedScheme = ControlScheme::Alternative;
                            audioManager.playMenuClickSound();
                            std::cout << "Alternative controls applied and saved!" << std::endl;
                        }

                        else if (clickX >= centerX + buttonSpacing - 160 && clickX <= centerX + buttonSpacing + 160 &&
                                 clickY >= buttonY && clickY <= buttonY + 60) {
                            rebindingSelectedScheme = ControlScheme::Custom;
                            rebindingAppliedScheme = ControlScheme::Custom;
                            selectedRebindingIndex = 0;
                            waitingForKeyPress = false;
                            audioManager.playMenuClickSound();
                            std::cout << "Custom mode selected" << std::endl;
                        }
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }
                    } else if (gameState == GameState::ConfirmClearScores) {

                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f clickPos = window.mapPixelToCoords(pixelPos);
                        float clickX = clickPos.x;
                        float clickY = clickPos.y;
                        

                        int clickedBombIndex = checkBombClick(backgroundPieces, clickX, clickY);
                        if (clickedBombIndex >= 0) {
                            const auto& bomb = backgroundPieces[clickedBombIndex];
                            explodeMenuBomb(bomb, explosionEffects, glowEffects, audioManager, 
                                          shakeIntensity, shakeDuration, shakeTimer, 
                                          saveData, achievementPopups);
                            backgroundPieces.erase(backgroundPieces.begin() + clickedBombIndex);
                        }

                    }
                }
            }
            

            if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseButtonReleased->button == sf::Mouse::Button::Left) {
                    if (draggingAudioSlider >= 0) {

                        saveGameData(saveData);
                        draggingAudioSlider = -1;
                    }
                    

                    if (gameState == GameState::ClassicDifficultySelect && isThemeSelectorHovered) {
                        audioManager.playMenuClickSound();
                        selectedThemeChoice = hoveredThemeChoice;
                        std::cout << "Theme changed to: " << static_cast<int>(selectedThemeChoice) << std::endl;
                    }
                    

                    if (gameState == GameState::ClassicDifficultySelect && isAbilitySelectorHovered) {

                        if (hoveredAbilityChoice == AbilityChoice::Bomb || 
                            hoveredAbilityChoice == AbilityChoice::Delivery || 
                            hoveredAbilityChoice == AbilityChoice::Stomp) {
                            audioManager.playMenuClickSound();
                            selectedAbilityChoice = hoveredAbilityChoice;
                            std::cout << "Ability changed to: " << static_cast<int>(selectedAbilityChoice) << std::endl;
                        }
                    }
                }
            }
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                
                if (keyPressed->code == keyBindings.mute) {
                    audioManager.toggleMute();
                    showVolumeIndicator = true;
                    volumeIndicatorTimer = VOLUME_INDICATOR_DURATION;
                    saveData.masterVolume = audioManager.getMasterVolume();
                    saveData.isMuted = audioManager.isMutedStatus();
                    saveGameData(saveData);
                } else if (keyPressed->code == keyBindings.volumeDown) {
                    audioManager.decreaseMasterVolume();
                    showVolumeIndicator = true;
                    volumeIndicatorTimer = VOLUME_INDICATOR_DURATION;
                    saveData.masterVolume = audioManager.getMasterVolume();
                    saveGameData(saveData);
                } else if (keyPressed->code == keyBindings.volumeUp) {
                    audioManager.increaseMasterVolume();
                    showVolumeIndicator = true;
                    volumeIndicatorTimer = VOLUME_INDICATOR_DURATION;
                    saveData.masterVolume = audioManager.getMasterVolume();
                    saveGameData(saveData);
                }
                
                if (gameState == GameState::SplashScreen) {

                    constexpr int CURRENT_SETUP_VERSION = 1;
                    audioManager.stopAllMusic();
                    if (saveData.setupVersion < CURRENT_SETUP_VERSION) {
                        gameState = GameState::FirstTimeSetup;
                        std::cout << "Splash screen skipped - showing FirstTimeSetup (keyboard)" << std::endl;
                    } else {
                        gameState = GameState::MainMenu;
                        audioManager.playMenuMusic();
                        std::cout << "Splash screen skipped (keyboard)" << std::endl;
                    }
                    showCustomCursor = true;
                } else if (gameState == GameState::FirstTimeSetup) {

                    if (keyPressed->code == sf::Keyboard::Key::Left) {
                        selectedControlScheme = ControlScheme::Classic;
                        hoveredControlScheme = ControlScheme::Classic;
                        audioManager.playMenuClickSound();
                        std::cout << "Selected Classic controls" << std::endl;
                    } else if (keyPressed->code == sf::Keyboard::Key::Right) {
                        selectedControlScheme = ControlScheme::Alternative;
                        hoveredControlScheme = ControlScheme::Alternative;
                        audioManager.playMenuClickSound();
                        std::cout << "Selected Alternative controls" << std::endl;
                    } else if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {

                        constexpr int CURRENT_SETUP_VERSION = 1;
                        
                        if (selectedControlScheme == ControlScheme::Classic) {

                        keyBindings.moveLeft = sf::Keyboard::Key::Left;
                        keyBindings.moveRight = sf::Keyboard::Key::Right;
                        keyBindings.rotateLeft = sf::Keyboard::Key::LControl;
                        keyBindings.rotateRight = sf::Keyboard::Key::Up;
                        keyBindings.quickFall = sf::Keyboard::Key::Down;
                        keyBindings.drop = sf::Keyboard::Key::Space;
                        keyBindings.hold = sf::Keyboard::Key::LShift;
                        keyBindings.bomb = sf::Keyboard::Key::Z;
                        
                        saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::Left);
                        saveData.moveRight = static_cast<int>(sf::Keyboard::Key::Right);
                        saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::LControl);
                        saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::Up);
                        saveData.quickFall = static_cast<int>(sf::Keyboard::Key::Down);
                        saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                        saveData.hold = static_cast<int>(sf::Keyboard::Key::LShift);
                        saveData.bomb = static_cast<int>(sf::Keyboard::Key::Z);
                        
                        std::cout << "Applied Classic controls" << std::endl;
                    } else {

                        keyBindings.moveLeft = sf::Keyboard::Key::A;
                        keyBindings.moveRight = sf::Keyboard::Key::D;
                        keyBindings.rotateLeft = sf::Keyboard::Key::J;
                        keyBindings.rotateRight = sf::Keyboard::Key::K;
                        keyBindings.quickFall = sf::Keyboard::Key::S;
                        keyBindings.drop = sf::Keyboard::Key::Space;
                        keyBindings.hold = sf::Keyboard::Key::L;
                        keyBindings.bomb = sf::Keyboard::Key::I;
                        
                        saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::A);
                        saveData.moveRight = static_cast<int>(sf::Keyboard::Key::D);
                        saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::J);
                        saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::K);
                        saveData.quickFall = static_cast<int>(sf::Keyboard::Key::S);
                        saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                        saveData.hold = static_cast<int>(sf::Keyboard::Key::L);
                        saveData.bomb = static_cast<int>(sf::Keyboard::Key::I);
                        
                        std::cout << "Applied Alternative controls" << std::endl;
                    }
                    
                        saveData.setupVersion = CURRENT_SETUP_VERSION;
                        saveGameData(saveData);
                        gameState = GameState::WelcomeScreen;
                        std::cout << "First time setup completed, showing welcome screen" << std::endl;
                    }
                } else if (gameState == GameState::WelcomeScreen) {

                    gameState = GameState::MainMenu;
                    audioManager.playMenuMusic();
                    showCustomCursor = true;
                    std::cout << "Welcome screen completed, transitioning to main menu" << std::endl;
                } else if (gameState == GameState::MainMenu) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape: 
                            window.close(); 
                            break;
                        case sf::Keyboard::Key::D:
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
                                debugMode = !debugMode;
                                std::cout << "DEBUG MODE " << (debugMode ? "ENABLED" : "DISABLED") << std::endl;
                                if (debugMode) {
                                    bombAbilityAvailable = true;
                                    deliveryAbilityAvailable = true;
                                    stompAbilityAvailable = true;
                                    std::cout << "All abilities loaded in debug mode!" << std::endl;
                                }
                            }
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            showCustomCursor = false;
                            useKeyboardNavigation = true;
                            selectedMenuOption = static_cast<MenuOption>((static_cast<int>(selectedMenuOption) - 1 + 4) % 4);
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            showCustomCursor = false;
                            useKeyboardNavigation = true;
                            selectedMenuOption = static_cast<MenuOption>((static_cast<int>(selectedMenuOption) + 1) % 4);
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedMenuOption == MenuOption::Start) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::GameModeSelect;
                                selectedGameModeOption = GameModeOption::Classic;
                                std::cout << "Entered GAME MODE selection" << std::endl;
                            } else if (selectedMenuOption == MenuOption::Extras) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::Extras;
                                selectedExtrasOption = ExtrasOption::Achievements;
                                std::cout << "Entered EXTRAS menu" << std::endl;
                            } else if (selectedMenuOption == MenuOption::Options) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::Options;
                                std::cout << "Entered OPTIONS menu" << std::endl;
                            } else if (selectedMenuOption == MenuOption::Exit) {
                                window.close();
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::Paused) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::Playing;
                            showCustomCursor = false;
                            std::cout << "Game resumed" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            selectedPauseOption = static_cast<PauseOption>((static_cast<int>(selectedPauseOption) - 1 + 3) % 3);
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            selectedPauseOption = static_cast<PauseOption>((static_cast<int>(selectedPauseOption) + 1) % 3);
                            break;
                        case sf::Keyboard::Key::Space:
                            if (selectedPauseOption == PauseOption::Resume) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::Playing;
                                showCustomCursor = false;
                                std::cout << "Game resumed from menu" << std::endl;
                            } else if (selectedPauseOption == PauseOption::Restart) {
                                audioManager.playMenuClickSound();
                                applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                                
                                gameState = GameState::Playing;
                                showCustomCursor = false;
                                gameOver = false;
                                gameOverSoundPlayed = false;
                                gameOverMusicTimer = 0.0f;
                                

                                bombUsedThisGame = false;
                                maxComboThisGame = 0;
                                consecutiveBombsUsed = 0;
                                hardDropCooldown = 0.0f;
                                

                                if (sprintModeActive || challengeModeActive) {
                                    sprintTimer = 0.0f;
                                    autoDropTimer = 0.0f;
                                    sprintCompleted = false;
                                }
                                
                                for (int i = 0; i < GRID_HEIGHT; ++i) {
                                    for (int j = 0; j < GRID_WIDTH; ++j) {
                                        grid[i][j] = Cell();
                                    }
                                }
                                totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                                currentLevel = 0;
                                totalScore = 0;
                                currentCombo = 0;
                                lastMoveScore = 0;
                                totalHardDropScore = 0;
                                totalLineScore = 0;
                                totalComboScore = 0;
                                RESET_SESSION_STATS();
                                TesseraBag.reset();
                                hasHeldPiece = false;
                                canUseHold = true;
                                linesSinceLastAbility = 0;
                                bombAbilityAvailable = false;
                                explosionEffects.clear();
                                glowEffects.clear();
                            thermometerParticles.clear();
                                leftHoldTime = 0.0f;
                                rightHoldTime = 0.0f;
                                dasTimer = 0.0f;
                                leftPressed = false;
                                rightPressed = false;
                                
                                PieceType startType = TesseraBag.getNextPiece();
                                PieceShape startShape = getPieceShape(startType);
                                int startX = (GRID_WIDTH - startShape.width) / 2;
                                activePiece = Piece(startX, 0, startType);
                                

                                if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                    int colorIndex = getColorIndexForPieceType(startType);
                                    if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                        activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                    }
                                } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                    int randomIndex = rand() % currentConfig->colorPalette.size();
                                    activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                                }
                                
                                std::cout << "Game restarted from pause menu!" << std::endl;
                            } else if (selectedPauseOption == PauseOption::QuitToMenu) {
                                audioManager.playMenuClickSound();
                                audioManager.switchToMenuMusic();
                                
                                gameState = GameState::MainMenu;
                                selectedMenuOption = MenuOption::Start;
                                std::cout << "Returned to main menu" << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::Extras) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::MainMenu;
                            selectedMenuOption = MenuOption::Extras;
                            std::cout << "Returned to main menu from EXTRAS" << std::endl;
                            break;
                        case sf::Keyboard::Key::D:
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
                                debugMode = !debugMode;
                                std::cout << "DEBUG MODE " << (debugMode ? "ENABLED" : "DISABLED") << std::endl;
                            }
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            useKeyboardNavigation = true;
                            selectedExtrasOption = static_cast<ExtrasOption>((static_cast<int>(selectedExtrasOption) + 2) % 3);
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            useKeyboardNavigation = true;
                            selectedExtrasOption = static_cast<ExtrasOption>((static_cast<int>(selectedExtrasOption) + 1) % 3);
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedExtrasOption == ExtrasOption::Achievements) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::AchievementsView;
                                std::cout << "Entered ACHIEVEMENTS view (keyboard)" << std::endl;
                            } else if (selectedExtrasOption == ExtrasOption::Statistics) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::StatisticsView;
                                std::cout << "Entered STATISTICS view (keyboard)" << std::endl;
                            } else if (selectedExtrasOption == ExtrasOption::BestScores) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::BestScoresView;
                                std::cout << "Entered BEST SCORES view (keyboard)" << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::GameModeSelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::MainMenu;
                            std::cout << "Returned to main menu from GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            useKeyboardNavigation = true;
                            selectedGameModeOption = static_cast<GameModeOption>((static_cast<int>(selectedGameModeOption) + 2) % 3);
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            useKeyboardNavigation = true;
                            selectedGameModeOption = static_cast<GameModeOption>((static_cast<int>(selectedGameModeOption) + 1) % 3);
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            audioManager.playMenuClickSound();
                            if (selectedGameModeOption == GameModeOption::Classic) {
                                gameState = GameState::ClassicDifficultySelect;
                                selectedClassicDifficulty = ClassicDifficulty::Hard;
                                std::cout << "Entered CLASSIC difficulty selection" << std::endl;
                            } else if (selectedGameModeOption == GameModeOption::Sprint) {
                                gameState = GameState::SprintLinesSelect;
                                selectedSprintLines = SprintLines::Lines24;
                                std::cout << "Entered BLITZ lines selection" << std::endl;
                            } else if (selectedGameModeOption == GameModeOption::Challenge) {
                                gameState = GameState::ChallengeSelect;
                                selectedChallengeMode = debugMode ? ChallengeMode::Debug : ChallengeMode::TheForest;
                                std::cout << "Entered CHALLENGE selection" << std::endl;
                            } else if (selectedGameModeOption == GameModeOption::Practice) {
                                gameState = GameState::PracticeSelect;
                                selectedPracticeOption = 0;
                                std::cout << "Entered PRACTICE selection" << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::ClassicDifficultySelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::GameModeSelect;
                            std::cout << "Returned to GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            useKeyboardNavigation = true;


                            if (selectedClassicDifficulty == ClassicDifficulty::Normal) {
                                selectedClassicDifficulty = ClassicDifficulty::Hard;
                            } else {
                                selectedClassicDifficulty = ClassicDifficulty::Normal;
                            }
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            useKeyboardNavigation = true;


                            if (selectedClassicDifficulty == ClassicDifficulty::Hard) {
                                selectedClassicDifficulty = ClassicDifficulty::Normal;
                            } else {
                                selectedClassicDifficulty = ClassicDifficulty::Hard;
                            }
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                        {

                            const DifficultyConfig* config = getDifficultyConfig(
                                selectedGameModeOption,
                                selectedClassicDifficulty,
                                selectedSprintLines,
                                selectedChallengeMode,
                                selectedPracticeDifficulty,
                                selectedPracticeLineGoal,
                                practiceInfiniteBombs
                            );
                            TesseraBag.setDifficultyConfig(config);
                            currentConfig = config;
                            
                            audioManager.playMenuClickSound();
                            applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                            
                            gameState = GameState::Playing;
                            gameOver = false;
                            gameOverSoundPlayed = false;
                            gameOverMusicTimer = 0.0f;
                            sprintModeActive = false;
                            challengeModeActive = false;
                            practiceModeActive = false;
                            sprintCompleted = false;
                            

                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            consecutiveBombsUsed = 0;
                            hardDropCooldown = 0.0f;
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            TesseraBag.reset();
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = debugMode;
                            explosionEffects.clear();
                            glowEffects.clear();
                            thermometerParticles.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            PieceType startType = TesseraBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(startShape);
                            int startY = -firstFilledRow;
                            activePiece = Piece(startX, startY, startType);
                            

                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(startType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                            }
                            std::cout << "Game started (Classic) - Difficulty: " << config->modeName << std::endl;
                            break;
                        }
                        default: 
                            break;
                    }
                } else if (gameState == GameState::SprintLinesSelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::GameModeSelect;
                            std::cout << "Returned to GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W: {
                            useKeyboardNavigation = true;
                            if (debugMode) {

                                int current = static_cast<int>(selectedSprintLines);
                                selectedSprintLines = static_cast<SprintLines>((current + 3) % 4);
                            } else {

                                int current = static_cast<int>(selectedSprintLines);
                                if (current == 1) current = 3;
                                else current--;
                                selectedSprintLines = static_cast<SprintLines>(current);
                            }
                            break;
                        }
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S: {
                            useKeyboardNavigation = true;
                            if (debugMode) {

                                int current = static_cast<int>(selectedSprintLines);
                                selectedSprintLines = static_cast<SprintLines>((current + 1) % 4);
                            } else {

                                int current = static_cast<int>(selectedSprintLines);
                                if (current == 3) current = 1;
                                else current++;
                                selectedSprintLines = static_cast<SprintLines>(current);
                            }
                            break;
                        }
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                        {

                            const DifficultyConfig* config = getDifficultyConfig(
                                selectedGameModeOption,
                                selectedClassicDifficulty,
                                selectedSprintLines,
                                selectedChallengeMode,
                                selectedPracticeDifficulty,
                                selectedPracticeLineGoal,
                                practiceInfiniteBombs
                            );
                            TesseraBag.setDifficultyConfig(config);
                            currentConfig = config;
                            
                            audioManager.playMenuClickSound();
                            applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                            
                            gameState = GameState::Playing;
                            gameOver = false;
                            gameOverSoundPlayed = false;
                            gameOverMusicTimer = 0.0f;
                            

                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            consecutiveBombsUsed = 0;
                            hardDropCooldown = 0.0f;
                            

                            sprintModeActive = true;
                            challengeModeActive = false;
                            practiceModeActive = false;
                            sprintCompleted = false;
                            sprintTimer = 0.0f;
                            autoDropTimer = 0.0f;
                            switch (selectedSprintLines) {
                                case SprintLines::Lines24:
                                    sprintTargetLines = 24;
                                    break;
                                case SprintLines::Lines48:
                                    sprintTargetLines = 48;
                                    break;
                                case SprintLines::Lines96:
                                    sprintTargetLines = 96;
                                    break;
                                default:
                                    sprintTargetLines = 24;
                                    break;
                            }
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            TesseraBag.reset();
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = debugMode;
                            explosionEffects.clear();
                            glowEffects.clear();
                            thermometerParticles.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            PieceType startType = TesseraBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(startShape);
                            int startY = -firstFilledRow;
                            activePiece = Piece(startX, startY, startType);
                            

                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(startType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                            }
                            std::cout << "Game started (Sprint) - Lines: " << config->modeName << std::endl;
                            break;
                        }
                        default: 
                            break;
                    }
                } else if (gameState == GameState::ChallengeSelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::GameModeSelect;
                            std::cout << "Returned to GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W: {
                            useKeyboardNavigation = true;
                            int current = static_cast<int>(selectedChallengeMode);
                            int startEnum = 1;
                            int numOptions = 9;
                            int leftColSize = (numOptions + 1) / 2;
                            int rightColSize = numOptions - leftColSize;

                            int adjustedCurrent = current - startEnum;
                            
                            if (adjustedCurrent < leftColSize) {

                                if (adjustedCurrent > 0) {
                                    selectedChallengeMode = static_cast<ChallengeMode>(current - 1);
                                } else {

                                    selectedChallengeMode = static_cast<ChallengeMode>(startEnum + leftColSize - 1);
                                }
                            } else {

                                int rightColIndex = adjustedCurrent - leftColSize;
                                
                                if (rightColIndex > 0) {
                                    selectedChallengeMode = static_cast<ChallengeMode>(current - 1);
                                } else {

                                    selectedChallengeMode = static_cast<ChallengeMode>(startEnum + leftColSize + rightColSize - 1);
                                }
                            }
                            break;
                        }
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S: {
                            useKeyboardNavigation = true;
                            int current = static_cast<int>(selectedChallengeMode);
                            int startEnum = 1;
                            int numOptions = 9;
                            int leftColSize = (numOptions + 1) / 2;
                            int rightColSize = numOptions - leftColSize;

                            int adjustedCurrent = current - startEnum;
                            
                            if (adjustedCurrent < leftColSize) {

                                if (adjustedCurrent < leftColSize - 1) {
                                    selectedChallengeMode = static_cast<ChallengeMode>(current + 1);
                                } else {

                                    selectedChallengeMode = static_cast<ChallengeMode>(startEnum);
                                }
                            } else {

                                int rightColIndex = adjustedCurrent - leftColSize;
                                
                                if (rightColIndex < rightColSize - 1) {
                                    selectedChallengeMode = static_cast<ChallengeMode>(current + 1);
                                } else {

                                    selectedChallengeMode = static_cast<ChallengeMode>(startEnum + leftColSize);
                                }
                            }
                            break;
                        }
                        case sf::Keyboard::Key::Left:
                        case sf::Keyboard::Key::A: {
                            useKeyboardNavigation = true;
                            int current = static_cast<int>(selectedChallengeMode);
                            int startEnum = 1;
                            int numOptions = 9;
                            int leftColSize = (numOptions + 1) / 2;
                            int adjustedCurrent = current - startEnum;
                            

                            if (adjustedCurrent >= leftColSize) {
                                int rightColIndex = adjustedCurrent - leftColSize;

                                int leftColIndex = std::min(rightColIndex, leftColSize - 1);
                                selectedChallengeMode = static_cast<ChallengeMode>(startEnum + leftColIndex);
                            }
                            break;
                        }
                        case sf::Keyboard::Key::Right:
                        case sf::Keyboard::Key::D: {
                            useKeyboardNavigation = true;
                            int current = static_cast<int>(selectedChallengeMode);
                            int startEnum = 1;
                            int numOptions = 9;
                            int leftColSize = (numOptions + 1) / 2;
                            int rightColSize = numOptions - leftColSize;
                            int adjustedCurrent = current - startEnum;
                            

                            if (adjustedCurrent < leftColSize) {

                                int rightColIndex = std::min(adjustedCurrent, rightColSize - 1);
                                selectedChallengeMode = static_cast<ChallengeMode>(startEnum + leftColSize + rightColIndex);
                            }
                            break;
                        }
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                        {

                            const DifficultyConfig* config = getDifficultyConfig(
                                selectedGameModeOption,
                                selectedClassicDifficulty,
                                selectedSprintLines,
                                selectedChallengeMode,
                                selectedPracticeDifficulty,
                                selectedPracticeLineGoal,
                                practiceInfiniteBombs
                            );
                            TesseraBag.setDifficultyConfig(config);
                            currentConfig = config;
                            
                            audioManager.playMenuClickSound();
                            applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                            
                            gameState = GameState::Playing;
                            gameOver = false;
                            gameOverSoundPlayed = false;
                            gameOverMusicTimer = 0.0f;
                            

                            challengeModeActive = true;
                            sprintModeActive = false;
                            practiceModeActive = false;
                            

                            sprintTimer = 0.0f;
                            autoDropTimer = 0.0f;
                            sprintCompleted = false;
                            gravityFlipped = false;
                            gravityFlipPieceCount = 0;
                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            consecutiveBombsUsed = 0;
                            hardDropCooldown = 0.0f;
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            TesseraBag.reset();
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = debugMode;
                            explosionEffects.clear();
                            glowEffects.clear();
                            thermometerParticles.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            PieceType startType = TesseraBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(startShape);
                            int startY = -firstFilledRow;
                            

                            if (selectedChallengeMode == ChallengeMode::GravityFlip) {
                                startY = GRID_HEIGHT / 2 - startShape.height / 2;
                                gravityFlipPieceCount = 1;
                                gravityFlipped = false;
                            }
                            
                            activePiece = Piece(startX, startY, startType);
                            

                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(startType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                            }
                            

                            if (selectedChallengeMode == ChallengeMode::OneRot) {
                                currentPieceRotations = 0;
                            }
                            
                            std::cout << "Game started (Challenge) - Mode: " << config->modeName << std::endl;
                            break;
                        }
                        default: 
                            break;
                    }
                } else if (gameState == GameState::PracticeSelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::GameModeSelect;
                            std::cout << "Returned to GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            useKeyboardNavigation = true;
                            selectedPracticeOption = (selectedPracticeOption + 4) % 5;
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            useKeyboardNavigation = true;
                            selectedPracticeOption = (selectedPracticeOption + 1) % 5;
                            break;
                        case sf::Keyboard::Key::Left:
                        case sf::Keyboard::Key::A:
                            if (selectedPracticeOption == 0) {

                                int current = static_cast<int>(selectedPracticeDifficulty);
                                selectedPracticeDifficulty = static_cast<PracticeDifficulty>((current + 3) % 4);
                            } else if (selectedPracticeOption == 1) {

                                int current = static_cast<int>(selectedPracticeLineGoal);
                                selectedPracticeLineGoal = static_cast<PracticeLineGoal>((current + 3) % 4);
                            } else if (selectedPracticeOption == 2) {

                                practiceInfiniteBombs = !practiceInfiniteBombs;
                            } else if (selectedPracticeOption == 3) {

                                int current = static_cast<int>(selectedPracticeStartLevel);
                                selectedPracticeStartLevel = static_cast<PracticeStartLevel>((current + 10) % 11);
                            }
                            break;
                        case sf::Keyboard::Key::Right:
                        case sf::Keyboard::Key::D:
                            if (selectedPracticeOption == 0) {

                                int current = static_cast<int>(selectedPracticeDifficulty);
                                selectedPracticeDifficulty = static_cast<PracticeDifficulty>((current + 1) % 4);
                            } else if (selectedPracticeOption == 1) {

                                int current = static_cast<int>(selectedPracticeLineGoal);
                                selectedPracticeLineGoal = static_cast<PracticeLineGoal>((current + 1) % 4);
                            } else if (selectedPracticeOption == 2) {

                                practiceInfiniteBombs = !practiceInfiniteBombs;
                            } else if (selectedPracticeOption == 3) {

                                int current = static_cast<int>(selectedPracticeStartLevel);
                                selectedPracticeStartLevel = static_cast<PracticeStartLevel>((current + 1) % 11);
                            }
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedPracticeOption == 4) {

                                selectedGameModeOption = GameModeOption::Practice;
                                
                                const DifficultyConfig* config = getDifficultyConfig(
                                    selectedGameModeOption,
                                    selectedClassicDifficulty,
                                    selectedSprintLines,
                                    selectedChallengeMode,
                                    selectedPracticeDifficulty,
                                    selectedPracticeLineGoal,
                                    practiceInfiniteBombs
                                );
                                TesseraBag.setDifficultyConfig(config);
                                currentConfig = config;
                                
                                audioManager.playMenuClickSound();
                                applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                                
                                gameState = GameState::Playing;
                                gameOver = false;
                                gameOverSoundPlayed = false;
                                gameOverMusicTimer = 0.0f;
                                
                                bombUsedThisGame = false;
                                maxComboThisGame = 0;
                                consecutiveBombsUsed = 0;
                                hardDropCooldown = 0.0f;
                                
                                practiceModeActive = true;
                                sprintModeActive = false;
                                challengeModeActive = false;
                                sprintCompleted = false;
                                sprintTimer = 0.0f;
                                autoDropTimer = 0.0f;
                                
                                for (int i = 0; i < GRID_HEIGHT; ++i) {
                                    for (int j = 0; j < GRID_WIDTH; ++j) {
                                        grid[i][j] = Cell();
                                    }
                                }
                                

                                currentLevel = static_cast<int>(selectedPracticeStartLevel);
                                totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                                
                                totalScore = 0;
                                currentCombo = 0;
                                lastMoveScore = 0;
                                totalHardDropScore = 0;
                                totalLineScore = 0;
                                totalComboScore = 0;
                                RESET_SESSION_STATS();
                                TesseraBag.reset(currentLevel);
                                hasHeldPiece = false;
                                canUseHold = true;
                                linesSinceLastAbility = 0;
                                bombAbilityAvailable = practiceInfiniteBombs ? true : debugMode;
                                deliveryAbilityAvailable = false;
                                explosionEffects.clear();
                                glowEffects.clear();
                            thermometerParticles.clear();
                                leftHoldTime = 0.0f;
                                rightHoldTime = 0.0f;
                                dasTimer = 0.0f;
                                leftPressed = false;
                                rightPressed = false;
                                
                                PieceType startType = TesseraBag.getNextPiece();
                                PieceShape startShape = getPieceShape(startType);
                                int startX = (GRID_WIDTH - startShape.width) / 2;
                                int firstFilledRow = findFirstFilledRow(startShape);
                                int startY = -firstFilledRow;
                                activePiece = Piece(startX, startY, startType);
                                

                                if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                    int colorIndex = getColorIndexForPieceType(startType);
                                    if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                        activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                    }
                                } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                    int randomIndex = rand() % currentConfig->colorPalette.size();
                                    activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                                }
                                
                                std::cout << "Game started (Practice) - Difficulty: " << config->modeName << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::AchievementsView) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::Extras;
                            std::cout << "Returned to EXTRAS from ACHIEVEMENTS" << std::endl;
                            break;
                        case sf::Keyboard::Key::D:
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
                                debugMode = !debugMode;
                                std::cout << "DEBUG MODE " << (debugMode ? "ENABLED" : "DISABLED") << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::StatisticsView) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::Extras;
                            std::cout << "Returned to EXTRAS from STATISTICS" << std::endl;
                            break;
                        case sf::Keyboard::Key::D:
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
                                debugMode = !debugMode;
                                std::cout << "DEBUG MODE " << (debugMode ? "ENABLED" : "DISABLED") << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::BestScoresView) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::Extras;
                            std::cout << "Returned to EXTRAS from BEST SCORES" << std::endl;
                            break;
                        case sf::Keyboard::Key::D:
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
                                debugMode = !debugMode;
                                std::cout << "DEBUG MODE " << (debugMode ? "ENABLED" : "DISABLED") << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::Options) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::MainMenu;
                            selectedMenuOption = MenuOption::Options;
                            std::cout << "Returned to main menu from OPTIONS" << std::endl;
                            break;
                        case sf::Keyboard::Key::D:
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
                                debugMode = !debugMode;
                                std::cout << "DEBUG MODE " << (debugMode ? "ENABLED" : "DISABLED") << std::endl;
                            }
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            useKeyboardNavigation = true;
                            if (selectedOptionsOption == OptionsMenuOption::Audio) {
                                selectedOptionsOption = OptionsMenuOption::ClearScores;
                            } else if (selectedOptionsOption == OptionsMenuOption::RebindKeys) {
                                selectedOptionsOption = OptionsMenuOption::Audio;
                            } else if (selectedOptionsOption == OptionsMenuOption::ClearScores) {
                                selectedOptionsOption = OptionsMenuOption::RebindKeys;
                            }
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            useKeyboardNavigation = true;
                            if (selectedOptionsOption == OptionsMenuOption::Audio) {
                                selectedOptionsOption = OptionsMenuOption::RebindKeys;
                            } else if (selectedOptionsOption == OptionsMenuOption::RebindKeys) {
                                selectedOptionsOption = OptionsMenuOption::ClearScores;
                            } else if (selectedOptionsOption == OptionsMenuOption::ClearScores) {
                                selectedOptionsOption = OptionsMenuOption::Audio;
                            }
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedOptionsOption == OptionsMenuOption::Audio) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::AudioSettings;
                                selectedAudioOption = AudioOption::MainVolume;
                                std::cout << "Opening AUDIO menu" << std::endl;
                            } else if (selectedOptionsOption == OptionsMenuOption::ClearScores) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::ConfirmClearScores;
                                selectedConfirmOption = ConfirmOption::No;
                                std::cout << "Opening confirmation dialog for clearing scores" << std::endl;
                            } else if (selectedOptionsOption == OptionsMenuOption::RebindKeys) {
                                audioManager.playMenuClickSound();
                                gameState = GameState::Rebinding;
                                std::cout << "Entering REBIND KEYS menu" << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::AudioSettings) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::Options;
                            std::cout << "Returned to OPTIONS menu" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            useKeyboardNavigation = true;
                            if (selectedAudioOption == AudioOption::MainVolume) {
                                selectedAudioOption = AudioOption::SfxVolume;
                            } else if (selectedAudioOption == AudioOption::MusicVolume) {
                                selectedAudioOption = AudioOption::MainVolume;
                            } else {
                                selectedAudioOption = AudioOption::MusicVolume;
                            }
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            useKeyboardNavigation = true;
                            if (selectedAudioOption == AudioOption::MainVolume) {
                                selectedAudioOption = AudioOption::MusicVolume;
                            } else if (selectedAudioOption == AudioOption::MusicVolume) {
                                selectedAudioOption = AudioOption::SfxVolume;
                            } else {
                                selectedAudioOption = AudioOption::MainVolume;
                            }
                            break;
                        case sf::Keyboard::Key::Left:
                        case sf::Keyboard::Key::A:
                        {
                            float step = 5.0f;
                            if (selectedAudioOption == AudioOption::MainVolume) {
                                float newVol = std::max(0.0f, audioManager.getMasterVolume() - step);
                                audioManager.setMasterVolume(newVol);
                                saveData.masterVolume = newVol;
                            } else if (selectedAudioOption == AudioOption::MusicVolume) {
                                float newVol = std::max(0.0f, audioManager.getMusicVolume() - step);
                                audioManager.setMusicVolume(newVol);
                                saveData.musicVolume = newVol;
                            } else {
                                float newVol = std::max(0.0f, audioManager.getSfxVolume() - step);
                                audioManager.setSfxVolume(newVol);
                                saveData.sfxVolume = newVol;
                            }
                            saveGameData(saveData);
                            break;
                        }
                        case sf::Keyboard::Key::Right:
                        case sf::Keyboard::Key::D:
                        {
                            float step = 5.0f;
                            if (selectedAudioOption == AudioOption::MainVolume) {
                                float newVol = std::min(100.0f, audioManager.getMasterVolume() + step);
                                audioManager.setMasterVolume(newVol);
                                saveData.masterVolume = newVol;
                            } else if (selectedAudioOption == AudioOption::MusicVolume) {
                                float newVol = std::min(100.0f, audioManager.getMusicVolume() + step);
                                audioManager.setMusicVolume(newVol);
                                saveData.musicVolume = newVol;
                            } else {
                                float newVol = std::min(100.0f, audioManager.getSfxVolume() + step);
                                audioManager.setSfxVolume(newVol);
                                saveData.sfxVolume = newVol;
                            }
                            saveGameData(saveData);
                            break;
                        }
                        default:
                            break;
                    }
                } else if (gameState == GameState::Rebinding) {
                    if (rebindingSelectedScheme == ControlScheme::Custom && waitingForKeyPress) {

                        if (keyPressed->code == sf::Keyboard::Key::Escape) {
                            waitingForKeyPress = false;
                            std::cout << "Key binding cancelled" << std::endl;
                        } else {
                            switch (selectedRebindingIndex) {
                                case 0: keyBindings.moveLeft = keyPressed->code; break;
                                case 1: keyBindings.moveRight = keyPressed->code; break;
                                case 2: keyBindings.rotateLeft = keyPressed->code; break;
                                case 3: keyBindings.rotateRight = keyPressed->code; break;
                                case 4: keyBindings.quickFall = keyPressed->code; break;
                                case 5: keyBindings.drop = keyPressed->code; break;
                                case 6: keyBindings.hold = keyPressed->code; break;
                                case 7: keyBindings.bomb = keyPressed->code; break;
                                case 8: keyBindings.restart = keyPressed->code; break;
                                case 9: keyBindings.mute = keyPressed->code; break;
                                case 10: keyBindings.volumeDown = keyPressed->code; break;
                                case 11: keyBindings.volumeUp = keyPressed->code; break;
                            }
                            waitingForKeyPress = false;
                            saveData.moveLeft = static_cast<int>(keyBindings.moveLeft);
                            saveData.moveRight = static_cast<int>(keyBindings.moveRight);
                            saveData.rotateLeft = static_cast<int>(keyBindings.rotateLeft);
                            saveData.rotateRight = static_cast<int>(keyBindings.rotateRight);
                            saveData.quickFall = static_cast<int>(keyBindings.quickFall);
                            saveData.drop = static_cast<int>(keyBindings.drop);
                            saveData.hold = static_cast<int>(keyBindings.hold);
                            saveData.bomb = static_cast<int>(keyBindings.bomb);
                            saveData.restart = static_cast<int>(keyBindings.restart);
                            saveData.mute = static_cast<int>(keyBindings.mute);
                            saveData.volumeDown = static_cast<int>(keyBindings.volumeDown);
                            saveData.volumeUp = static_cast<int>(keyBindings.volumeUp);
                            saveGameData(saveData);
                            rebindingAppliedScheme = ControlScheme::Custom;
                            std::cout << "Key bound successfully and saved" << std::endl;
                        }
                    } else {

                        if (keyPressed->code == sf::Keyboard::Key::D && 
                            (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))) {
                            debugMode = !debugMode;
                            std::cout << "DEBUG MODE " << (debugMode ? "ENABLED" : "DISABLED") << std::endl;
                        } else {
                        switch (keyPressed->code) {
                            case sf::Keyboard::Key::Escape:
                                audioManager.playMenuBackSound();
                                gameState = GameState::Options;
                                selectedRebindingIndex = 0;
                                waitingForKeyPress = false;
                                std::cout << "Returned to OPTIONS menu" << std::endl;
                                break;
                            case sf::Keyboard::Key::Left:
                            case sf::Keyboard::Key::A:
                                if (rebindingSelectedScheme == ControlScheme::Classic) {
                                    rebindingSelectedScheme = ControlScheme::Custom;
                                    rebindingAppliedScheme = ControlScheme::Custom;
                                    std::cout << "Custom mode selected" << std::endl;
                                } else if (rebindingSelectedScheme == ControlScheme::Alternative) {
                                    rebindingSelectedScheme = ControlScheme::Classic;

                                    keyBindings.moveLeft = sf::Keyboard::Key::Left;
                                    keyBindings.moveRight = sf::Keyboard::Key::Right;
                                    keyBindings.rotateLeft = sf::Keyboard::Key::LControl;
                                    keyBindings.rotateRight = sf::Keyboard::Key::Up;
                                    keyBindings.quickFall = sf::Keyboard::Key::Down;
                                    keyBindings.drop = sf::Keyboard::Key::Space;
                                    keyBindings.hold = sf::Keyboard::Key::LShift;
                                    keyBindings.bomb = sf::Keyboard::Key::Z;
                                    keyBindings.restart = sf::Keyboard::Key::R;
                                    keyBindings.mute = sf::Keyboard::Key::M;
                                    keyBindings.volumeDown = sf::Keyboard::Key::Comma;
                                    keyBindings.volumeUp = sf::Keyboard::Key::Period;
                                    saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::Left);
                                    saveData.moveRight = static_cast<int>(sf::Keyboard::Key::Right);
                                    saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::LControl);
                                    saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::Up);
                                    saveData.quickFall = static_cast<int>(sf::Keyboard::Key::Down);
                                    saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                                    saveData.hold = static_cast<int>(sf::Keyboard::Key::LShift);
                                    saveData.bomb = static_cast<int>(sf::Keyboard::Key::Z);
                                    saveData.restart = static_cast<int>(sf::Keyboard::Key::R);
                                    saveData.mute = static_cast<int>(sf::Keyboard::Key::M);
                                    saveData.volumeDown = static_cast<int>(sf::Keyboard::Key::Comma);
                                    saveData.volumeUp = static_cast<int>(sf::Keyboard::Key::Period);
                                    saveGameData(saveData);
                                    rebindingAppliedScheme = ControlScheme::Classic;
                                    std::cout << "Classic controls applied!" << std::endl;
                                } else {
                                    rebindingSelectedScheme = ControlScheme::Alternative;

                                    keyBindings.moveLeft = sf::Keyboard::Key::A;
                                    keyBindings.moveRight = sf::Keyboard::Key::D;
                                    keyBindings.rotateLeft = sf::Keyboard::Key::J;
                                    keyBindings.rotateRight = sf::Keyboard::Key::K;
                                    keyBindings.quickFall = sf::Keyboard::Key::S;
                                    keyBindings.drop = sf::Keyboard::Key::Space;
                                    keyBindings.hold = sf::Keyboard::Key::L;
                                    keyBindings.bomb = sf::Keyboard::Key::I;
                                    keyBindings.restart = sf::Keyboard::Key::R;
                                    keyBindings.mute = sf::Keyboard::Key::M;
                                    keyBindings.volumeDown = sf::Keyboard::Key::Comma;
                                    keyBindings.volumeUp = sf::Keyboard::Key::Period;
                                    saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::A);
                                    saveData.moveRight = static_cast<int>(sf::Keyboard::Key::D);
                                    saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::J);
                                    saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::K);
                                    saveData.quickFall = static_cast<int>(sf::Keyboard::Key::S);
                                    saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                                    saveData.hold = static_cast<int>(sf::Keyboard::Key::L);
                                    saveData.bomb = static_cast<int>(sf::Keyboard::Key::I);
                                    saveData.restart = static_cast<int>(sf::Keyboard::Key::R);
                                    saveData.mute = static_cast<int>(sf::Keyboard::Key::M);
                                    saveData.volumeDown = static_cast<int>(sf::Keyboard::Key::Comma);
                                    saveData.volumeUp = static_cast<int>(sf::Keyboard::Key::Period);
                                    saveGameData(saveData);
                                    rebindingAppliedScheme = ControlScheme::Alternative;
                                    std::cout << "Alternative controls applied!" << std::endl;
                                }
                                rebindingHoveredScheme = rebindingSelectedScheme;
                                selectedRebindingIndex = 0;
                                waitingForKeyPress = false;
                                break;
                            case sf::Keyboard::Key::Right:
                            case sf::Keyboard::Key::D:
                                if (rebindingSelectedScheme == ControlScheme::Classic) {
                                    rebindingSelectedScheme = ControlScheme::Alternative;

                                    keyBindings.moveLeft = sf::Keyboard::Key::A;
                                    keyBindings.moveRight = sf::Keyboard::Key::D;
                                    keyBindings.rotateLeft = sf::Keyboard::Key::J;
                                    keyBindings.rotateRight = sf::Keyboard::Key::K;
                                    keyBindings.quickFall = sf::Keyboard::Key::S;
                                    keyBindings.drop = sf::Keyboard::Key::Space;
                                    keyBindings.hold = sf::Keyboard::Key::L;
                                    keyBindings.bomb = sf::Keyboard::Key::I;
                                    keyBindings.restart = sf::Keyboard::Key::R;
                                    keyBindings.mute = sf::Keyboard::Key::M;
                                    keyBindings.volumeDown = sf::Keyboard::Key::Comma;
                                    keyBindings.volumeUp = sf::Keyboard::Key::Period;
                                    saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::A);
                                    saveData.moveRight = static_cast<int>(sf::Keyboard::Key::D);
                                    saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::J);
                                    saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::K);
                                    saveData.quickFall = static_cast<int>(sf::Keyboard::Key::S);
                                    saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                                    saveData.hold = static_cast<int>(sf::Keyboard::Key::L);
                                    saveData.bomb = static_cast<int>(sf::Keyboard::Key::I);
                                    saveGameData(saveData);
                                    rebindingAppliedScheme = ControlScheme::Alternative;
                                    std::cout << "Alternative controls applied!" << std::endl;
                                } else if (rebindingSelectedScheme == ControlScheme::Alternative) {
                                    rebindingSelectedScheme = ControlScheme::Custom;
                                    rebindingAppliedScheme = ControlScheme::Custom;
                                    std::cout << "Custom mode selected" << std::endl;
                                } else {
                                    rebindingSelectedScheme = ControlScheme::Classic;

                                    keyBindings.moveLeft = sf::Keyboard::Key::Left;
                                    keyBindings.moveRight = sf::Keyboard::Key::Right;
                                    keyBindings.rotateLeft = sf::Keyboard::Key::LControl;
                                    keyBindings.rotateRight = sf::Keyboard::Key::Up;
                                    keyBindings.quickFall = sf::Keyboard::Key::Down;
                                    keyBindings.drop = sf::Keyboard::Key::Space;
                                    keyBindings.hold = sf::Keyboard::Key::LShift;
                                    keyBindings.bomb = sf::Keyboard::Key::Z;
                                    saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::Left);
                                    saveData.moveRight = static_cast<int>(sf::Keyboard::Key::Right);
                                    saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::LControl);
                                    saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::Up);
                                    saveData.quickFall = static_cast<int>(sf::Keyboard::Key::Down);
                                    saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                                    saveData.hold = static_cast<int>(sf::Keyboard::Key::LShift);
                                    saveData.bomb = static_cast<int>(sf::Keyboard::Key::Z);
                                    saveGameData(saveData);
                                    rebindingAppliedScheme = ControlScheme::Classic;
                                    std::cout << "Classic controls applied!" << std::endl;
                                }
                                rebindingHoveredScheme = rebindingSelectedScheme;
                                selectedRebindingIndex = 0;
                                waitingForKeyPress = false;
                                break;
                            case sf::Keyboard::Key::Up:
                            case sf::Keyboard::Key::W:
                                if (rebindingSelectedScheme == ControlScheme::Custom) {
                                    if (selectedRebindingIndex == 0) {

                                        selectedRebindingIndex = 0;
                                    } else {
                                        selectedRebindingIndex--;
                                    }
                                }
                                break;
                            case sf::Keyboard::Key::Down:
                            case sf::Keyboard::Key::S:
                                if (rebindingSelectedScheme == ControlScheme::Custom) {
                                    selectedRebindingIndex++;
                                    if (selectedRebindingIndex >= MAX_REBINDING_OPTIONS) {
                                        selectedRebindingIndex = 0;
                                    }
                                }
                                break;
                            case sf::Keyboard::Key::Enter:
                            case sf::Keyboard::Key::Space:
                                if (rebindingSelectedScheme == ControlScheme::Classic) {

                                    keyBindings.moveLeft = sf::Keyboard::Key::Left;
                                    keyBindings.moveRight = sf::Keyboard::Key::Right;
                                    keyBindings.rotateLeft = sf::Keyboard::Key::LControl;
                                    keyBindings.rotateRight = sf::Keyboard::Key::Up;
                                    keyBindings.quickFall = sf::Keyboard::Key::Down;
                                    keyBindings.drop = sf::Keyboard::Key::Space;
                                    keyBindings.hold = sf::Keyboard::Key::LShift;
                                    keyBindings.bomb = sf::Keyboard::Key::Z;
                                    saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::Left);
                                    saveData.moveRight = static_cast<int>(sf::Keyboard::Key::Right);
                                    saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::LControl);
                                    saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::Up);
                                    saveData.quickFall = static_cast<int>(sf::Keyboard::Key::Down);
                                    saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                                    saveData.hold = static_cast<int>(sf::Keyboard::Key::LShift);
                                    saveData.bomb = static_cast<int>(sf::Keyboard::Key::Z);
                                    saveGameData(saveData);
                                    audioManager.playMenuClickSound();
                                    rebindingAppliedScheme = ControlScheme::Classic;
                                    std::cout << "Classic controls applied and saved!" << std::endl;
                                } else if (rebindingSelectedScheme == ControlScheme::Alternative) {

                                    keyBindings.moveLeft = sf::Keyboard::Key::A;
                                    keyBindings.moveRight = sf::Keyboard::Key::D;
                                    keyBindings.rotateLeft = sf::Keyboard::Key::J;
                                    keyBindings.rotateRight = sf::Keyboard::Key::K;
                                    keyBindings.quickFall = sf::Keyboard::Key::S;
                                    keyBindings.drop = sf::Keyboard::Key::Space;
                                    keyBindings.hold = sf::Keyboard::Key::L;
                                    keyBindings.bomb = sf::Keyboard::Key::I;
                                    saveData.moveLeft = static_cast<int>(sf::Keyboard::Key::A);
                                    saveData.moveRight = static_cast<int>(sf::Keyboard::Key::D);
                                    saveData.rotateLeft = static_cast<int>(sf::Keyboard::Key::J);
                                    saveData.rotateRight = static_cast<int>(sf::Keyboard::Key::K);
                                    saveData.quickFall = static_cast<int>(sf::Keyboard::Key::S);
                                    saveData.drop = static_cast<int>(sf::Keyboard::Key::Space);
                                    saveData.hold = static_cast<int>(sf::Keyboard::Key::L);
                                    saveData.bomb = static_cast<int>(sf::Keyboard::Key::I);
                                    saveGameData(saveData);
                                    audioManager.playMenuClickSound();
                                    rebindingAppliedScheme = ControlScheme::Alternative;
                                    std::cout << "Alternative controls applied and saved!" << std::endl;
                                } else if (rebindingSelectedScheme == ControlScheme::Custom) {
                                    waitingForKeyPress = true;
                                    std::cout << "Waiting for key press..." << std::endl;
                                }
                                break;
                            default: 
                                break;
                        }
                        }
                    }
                } else if (gameState == GameState::ConfirmClearScores) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            audioManager.playMenuBackSound();
                            gameState = GameState::Options;
                            std::cout << "Cancelled clearing scores" << std::endl;
                            break;
                        case sf::Keyboard::Key::Left:
                        case sf::Keyboard::Key::A:
                            selectedConfirmOption = ConfirmOption::Yes;
                            break;
                        case sf::Keyboard::Key::Right:
                        case sf::Keyboard::Key::D:
                            selectedConfirmOption = ConfirmOption::No;
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            audioManager.playMenuClickSound();
                            if (selectedConfirmOption == ConfirmOption::Yes) {

                                saveData.highScore = 0;
                                saveData.highScoreClassicNormal = 0;
                                saveData.highScoreClassicHard = 0;
                                saveData.bestLines = 0;
                                saveData.bestLevel = 0;
                                

                                saveData.bestTimeSprint1 = 0.0f;
                                saveData.bestTimeSprint24 = 0.0f;
                                saveData.bestTimeSprint48 = 0.0f;
                                saveData.bestTimeSprint96 = 0.0f;
                                

                                saveData.bestTimeChallengeDebug = 0.0f;
                                saveData.bestTimeChallengeTheForest = 0.0f;
                                saveData.bestTimeChallengeRandomness = 0.0f;
                                saveData.bestTimeChallengeNonStraight = 0.0f;
                                saveData.bestTimeChallengeOneRot = 0.0f;
                                saveData.bestTimeChallengeChristopherCurse = 0.0f;
                                saveData.bestTimeChallengeVanishing = 0.0f;
                                saveData.bestTimeChallengeAutoDrop = 0.0f;
                                saveData.bestTimeChallengeGravityFlip = 0.0f;
                                

                                for (int i = 0; i < TOTAL_ACHIEVEMENTS; i++) {
                                    saveData.achievements[i] = false;
                                }

                                for (int i = 0; i < 3; i++) {
                                    saveData.topScores[i].score = 0;
                                    saveData.topScores[i].lines = 0;
                                    saveData.topScores[i].level = 0;
                                    
                                    saveData.topScoresNormal[i].score = 0;
                                    saveData.topScoresNormal[i].lines = 0;
                                    saveData.topScoresNormal[i].level = 0;
                                    
                                    saveData.topScoresHard[i].score = 0;
                                    saveData.topScoresHard[i].lines = 0;
                                    saveData.topScoresHard[i].level = 0;
                                }
                                saveGameData(saveData);
                                std::cout << "All data cleared (scores, times, achievements)!" << std::endl;
                                gameState = GameState::Options;
                            } else {

                                std::cout << "Cancelled clearing scores" << std::endl;
                                gameState = GameState::Options;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::Playing) {
                

                bool canRotate = true;
                if (challengeModeActive && selectedChallengeMode == ChallengeMode::OneRot) {
                    canRotate = (currentPieceRotations < 1);
                }
                
                if (keyPressed->code == keyBindings.rotateLeft) {
                    if (!gameOver && canRotate) {
                        activePiece.rotateLeft(grid);
                        saveData.totalRotations++;
                        if (challengeModeActive && selectedChallengeMode == ChallengeMode::OneRot) {
                            currentPieceRotations++;
                        }
                    }
                } else if (keyPressed->code == keyBindings.rotateRight) {
                    if (!gameOver && canRotate) {
                        activePiece.rotateRight(grid);
                        saveData.totalRotations++;
                        if (challengeModeActive && selectedChallengeMode == ChallengeMode::OneRot) {
                            currentPieceRotations++;
                        }
                    }
                } else if (keyPressed->code == keyBindings.hold) {
                    if (!canUseHold || gameOver) {
                        
                    } else {

                        if (challengeModeActive && selectedChallengeMode == ChallengeMode::AutoDrop) {
                            autoDropTimer = 0.0f;
                        }
                        
                        if (!hasHeldPiece) {

                            heldPiece = activePiece.getType();
                            hasHeldPiece = true;
                            

                            saveData.totalHolds++;
                            

                            if (heldPiece == PieceType::A_Bomb && !challengeModeActive && !practiceModeActive) {
                                unlockAchievement(saveData, Achievement::HoldBomb, &achievementPopups, &audioManager);
                            }
                            
                            PieceType newType = TesseraBag.getNextPiece();
                            std::cout << "HOLD: Stored " << pieceTypeToString(heldPiece) << ", spawning " << pieceTypeToString(newType) << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(newShape);
                            int spawnY = -firstFilledRow;
                            

                            if (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) {
                                spawnY = GRID_HEIGHT / 2 - newShape.height / 2;
                            }
                            
                            activePiece = Piece(spawnX, spawnY, newType);
                            

                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(newType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                            }
                        } else {

                            PieceType currentType = activePiece.getType();
                            PieceType swapType = heldPiece;
                            heldPiece = currentType;
                            

                            saveData.totalHolds++;
                            

                            if (heldPiece == PieceType::A_Bomb && !challengeModeActive && !practiceModeActive) {
                                unlockAchievement(saveData, Achievement::HoldBomb, &achievementPopups, &audioManager);
                            }
                            
                            std::cout << "SWAP: " << pieceTypeToString(currentType) << " <-> " << pieceTypeToString(swapType) << std::endl;
                            PieceShape swapShape = getPieceShape(swapType);
                            int spawnX = (GRID_WIDTH - swapShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(swapShape);
                            int spawnY = -firstFilledRow;
                            

                            if (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) {
                                spawnY = GRID_HEIGHT / 2 - swapShape.height / 2;
                            }
                            
                            activePiece = Piece(spawnX, spawnY, swapType);
                            

                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(swapType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                            }
                        }
                        
                        canUseHold = false;
                    }
                } else if (keyPressed->code == keyBindings.drop) {
                    if (!gameOver && hardDropCooldown <= 0.0f) {

                        if (activePiece.getAbility() == AbilityType::Bomb) {

                            if (challengeModeActive && selectedChallengeMode == ChallengeMode::AutoDrop) {
                                autoDropTimer = 0.0f;
                            }
                            
                            std::cout << "BOMB EXPLOSION TRIGGERED!" << std::endl;
                            

                            consecutiveBombsUsed++;
                            std::cout << "[BOMB] Consecutive explosions: " << consecutiveBombsUsed << "/3" << std::endl;
                            
                            if (consecutiveBombsUsed >= 3 && !challengeModeActive && !practiceModeActive) {
                                std::cout << "[ACHIEVEMENT] UNLOCKING EXPLOSION!" << std::endl;
                                unlockAchievement(saveData, Achievement::Explosion, &achievementPopups, &audioManager);
                            }
                            
                            int bombCenterX = activePiece.getX();
                            int bombCenterY = activePiece.getY();
                            

                            for (int dy = -2; dy <= 2; ++dy) {
                                for (int dx = -2; dx <= 2; ++dx) {
                                    int x = bombCenterX + dx;
                                    int y = bombCenterY + dy;
                                    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                                        sf::Color blockColor = grid[y][x].occupied ? grid[y][x].color : sf::Color(200, 200, 200);
                                        
                                        grid[y][x] = Cell();
                                        

                                        float explosionX = GRID_OFFSET_X + x * CELL_SIZE;
                                        float explosionY = GRID_OFFSET_Y + y * CELL_SIZE;
                                        float explosionRotation = static_cast<float>(rand() % 360);
                                        
                                        explosionEffects.push_back(ExplosionEffect(explosionX, explosionY, explosionRotation, 0.0f));
                                        
                                        float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                                        float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                                        float glowRotation = static_cast<float>(rand() % 360);
                                        
                                        glowEffects.push_back(GlowEffect(explosionX + offsetX, explosionY + offsetY, blockColor, glowRotation));
                                    }
                                }
                            }
                            
                            shakeIntensity = 15.0f;
                            shakeDuration = 0.4f;
                            shakeTimer = 0.0f;
                            
                            audioManager.playBombSound();
                            

                            int minX = std::max(0, bombCenterX - 2);
                            int maxX = std::min(GRID_WIDTH - 1, bombCenterX + 2);
                            int minY = std::max(0, bombCenterY - 2);
                            int maxY = std::min(GRID_HEIGHT - 1, bombCenterY + 2);
                            
                            for (int col = minX; col <= maxX; ++col) {
                                for (int row = minY - 1; row >= 0; --row) {
                                    if (grid[row][col].occupied) {
                                        int fallRow = row;
                                        
                                        while (fallRow + 1 < GRID_HEIGHT && !grid[fallRow + 1][col].occupied) {
                                            fallRow++;
                                        }
                                        
                                        if (fallRow != row) {
                                            grid[fallRow][col] = grid[row][col];
                                            grid[row][col] = Cell();
                                        }
                                    }
                                }
                            }
                            

                            int clearedLines;
                            bool isPetrifyMode = challengeModeActive && selectedChallengeMode == ChallengeMode::Petrify;
                            bool isRaceMode = sprintModeActive || challengeModeActive;
                            if (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) {
                                clearedLines = clearFullLinesGravityFlip(grid, audioManager, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer);
                            } else {
                                clearedLines = clearFullLines(grid, audioManager, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer, isPetrifyMode, isRaceMode ? &thermometerParticles : nullptr, isRaceMode);
                            }
                            

                            if (isPetrifyMode) {
                                updatePetrifyCounters(grid);
                            }
                            if (clearedLines > 0) {
                                totalLinesCleared += clearedLines;
                                
                                int fullScore = calculateScore(clearedLines);
                                int baseScore = clearedLines * 1000;
                                int lineBonus = fullScore - baseScore;
                                int comboBonus = currentCombo * COMBO_BONUS_PER_LINE * clearedLines;
                                int bombLineScore = fullScore + comboBonus;
                                totalScore += bombLineScore;
                                totalLineScore += baseScore;
                                totalComboScore += lineBonus + comboBonus;
                                lastMoveScore = bombLineScore;
                                
                                currentCombo += clearedLines;
                                

                                if (!challengeModeActive && !practiceModeActive) {
                                    if (currentCombo >= 12) {
                                        unlockAchievement(saveData, Achievement::Combo10, &achievementPopups, &audioManager);
                                    }
                                    
                                    if (clearedLines >= 5) {
                                        unlockAchievement(saveData, Achievement::Combo5OneClear, &achievementPopups, &audioManager);
                                    }
                                    
                                    if (clearedLines >= 6) {
                                        unlockAchievement(saveData, Achievement::Combo6OneClear, &achievementPopups, &audioManager);
                                    }
                                }
                                
                                if (currentCombo > maxComboThisGame) {
                                    maxComboThisGame = currentCombo;
                                }
                                
                                std::cout << "Bomb cleared " << clearedLines << " lines! Base: " << baseScore << " | Combo: x" << (currentCombo - clearedLines) << " (+" << comboBonus << ") | Total: +" << bombLineScore << std::endl;
                                
                                linesSinceLastAbility += clearedLines;
                                if (selectedAbilityChoice == AbilityChoice::Bomb && linesSinceLastAbility >= LINES_FOR_BOMB) {
                                    bombAbilityAvailable = true;
                                    std::cout << "BOMB ABILITY READY AGAIN!" << std::endl;
                                } else if (selectedAbilityChoice == AbilityChoice::Delivery && linesSinceLastAbility >= LINES_FOR_DELIVERY) {
                                    deliveryAbilityAvailable = true;
                                    std::cout << "DELIVERY ABILITY READY AGAIN!" << std::endl;
                                } else if (selectedAbilityChoice == AbilityChoice::Stomp && linesSinceLastAbility >= LINES_FOR_STOMP) {
                                    stompAbilityAvailable = true;
                                    std::cout << "STOMP ABILITY READY AGAIN!" << std::endl;
                                }
                            } else {
                                lastMoveScore = 0;
                            }
                            

                            PieceType newType = TesseraBag.getNextPiece();
                            std::cout << "Spawning new piece after explosion: " << pieceTypeToString(newType) << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(newShape);
                            int spawnY = -firstFilledRow;
                            activePiece = Piece(spawnX, spawnY, newType);
                            

                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(newType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                            }
                            canUseHold = true;
                        } else {

                            bool useGravityFlipForDrop = (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) ? gravityFlipped : false;
                            int dropDistance = activePiece.moveGround(grid, useGravityFlipForDrop);
                            int dropPoints = dropDistance * HARD_DROP_POINTS_PER_CELL;
                            totalScore += dropPoints;
                            totalHardDropScore += dropPoints;
                            std::cout << "Hard drop: " << dropDistance << " cells = +" << dropPoints << " points" << std::endl;
                            audioManager.playDropSound();
                        }
                    }
                } else if (keyPressed->code == keyBindings.menu) {
                    if (gameOver) {

                    } else {

                        gameState = GameState::Paused;
                        showCustomCursor = true;
                        selectedPauseOption = PauseOption::Resume;
                        std::cout << "Game paused" << std::endl;
                    }
                } else if (keyPressed->code == keyBindings.bomb) {

                    if (selectedAbilityChoice == AbilityChoice::Bomb) {
                        if ((bombAbilityAvailable || debugMode || practiceInfiniteBombs) && !gameOver) {
                            std::cout << "BOMB ABILITY ACTIVATED - Spawning bomb!" << (debugMode ? " (DEBUG MODE)" : "") << (practiceInfiniteBombs ? " (INFINITE BOMBS)" : "") << std::endl;
                            

                            bombUsedThisGame = true;
                            

                            saveData.totalBombsUsed++;
                            

                            PieceType replacedPiece = activePiece.getType();
                            TesseraBag.returnPieceToBag(replacedPiece);
                            std::cout << "Returned " << pieceTypeToString(replacedPiece) << " to bag" << std::endl;
                            
                            PieceType newType = PieceType::A_Bomb;
                            std::cout << "Spawning bomb piece!" << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(newShape);
                            int spawnY = -firstFilledRow;
                            activePiece = Piece(spawnX, spawnY, newType);
                            
                            if (!debugMode && !practiceInfiniteBombs) {
                                bombAbilityAvailable = false;
                                linesSinceLastAbility = 0;
                            }
                        } else if (!bombAbilityAvailable && !debugMode && !practiceInfiniteBombs && !gameOver) {
                            int linesNeeded = LINES_FOR_BOMB - linesSinceLastAbility;
                            std::cout << "Bomb ability not ready yet! Need " << linesNeeded << " more lines." << std::endl;
                        }
                    } else if (selectedAbilityChoice == AbilityChoice::Delivery) {
                        if ((deliveryAbilityAvailable || debugMode) && !gameOver) {
                            std::cout << "DELIVERY ABILITY ACTIVATED - Returning piece and adding 6 cream blocks!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
                            

                            PieceType replacedPiece = activePiece.getType();
                            TesseraBag.returnPieceToBag(replacedPiece);
                            std::cout << "Returned " << pieceTypeToString(replacedPiece) << " to bag" << std::endl;
                            

                            TesseraBag.insertPiecesToQueue(PieceType::Cream_Single, 6);
                            std::cout << "Added 6 cream blocks to queue!" << std::endl;
                            

                            PieceType newType = TesseraBag.getNextPiece();
                            std::cout << "Spawning next piece: " << pieceTypeToString(newType) << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(newShape);
                            int spawnY = -firstFilledRow;
                            activePiece = Piece(spawnX, spawnY, newType);
                            
                            if (!debugMode) {
                                deliveryAbilityAvailable = false;
                                linesSinceLastAbility = 0;
                            }
                        } else if (!deliveryAbilityAvailable && !debugMode && !gameOver) {
                            int linesNeeded = LINES_FOR_DELIVERY - linesSinceLastAbility;
                            std::cout << "Delivery ability not ready yet! Need " << linesNeeded << " more lines." << std::endl;
                        }
                    } else if (selectedAbilityChoice == AbilityChoice::Stomp) {
                        if ((stompAbilityAvailable || debugMode) && !gameOver) {
                            std::cout << "STOMP ABILITY ACTIVATED - Spawning stomp piece!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
                            

                            PieceType replacedPiece = activePiece.getType();
                            TesseraBag.returnPieceToBag(replacedPiece);
                            std::cout << "Returned " << pieceTypeToString(replacedPiece) << " to bag" << std::endl;
                            

                            PieceType newType = PieceType::A_Stomp;
                            std::cout << "Spawning stomp piece!" << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(newShape);
                            int spawnY = -firstFilledRow;
                            activePiece = Piece(spawnX, spawnY, newType);
                            
                            if (!debugMode) {
                                stompAbilityAvailable = false;
                                linesSinceLastAbility = 0;
                            }
                        } else if (!stompAbilityAvailable && !debugMode && !gameOver) {
                            int linesNeeded = LINES_FOR_STOMP - linesSinceLastAbility;
                            std::cout << "Stomp ability not ready yet! Need " << linesNeeded << " more lines." << std::endl;
                        }
                    }
                } else if (keyPressed->code == sf::Keyboard::Key::R) {
                    if (!gameOver) {
                        std::cout << "R key pressed during game! Restarting..." << std::endl;
                        

                        bombUsedThisGame = false;
                        maxComboThisGame = 0;
                        consecutiveBombsUsed = 0;
                        hardDropCooldown = 0.0f;
                        
                        if (sprintModeActive || challengeModeActive) {
                            sprintTimer = 0.0f;
                            autoDropTimer = 0.0f;
                            sprintCompleted = false;
                        }
                        

                        gravityFlipped = false;
                        gravityFlipPieceCount = 0;
                        

                        for (int i = 0; i < GRID_HEIGHT; ++i) {
                            for (int j = 0; j < GRID_WIDTH; ++j) {
                                if (grid[i][j].occupied) {
                                    float cellX = GRID_OFFSET_X + j * CELL_SIZE;
                                    float cellY = GRID_OFFSET_Y + i * CELL_SIZE;
                                    fallingCells.emplace_back(cellX, cellY, grid[i][j].color, grid[i][j].textureType);
                                }
                                grid[i][j] = Cell();
                            }
                        }
                        
                        if (practiceModeActive) {

                            currentLevel = static_cast<int>(selectedPracticeStartLevel);
                            totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                        } else {
                            totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                            currentLevel = 0;
                        }
                        
                        totalScore = 0;
                        currentCombo = 0;
                        lastMoveScore = 0;
                        totalHardDropScore = 0;
                        totalLineScore = 0;
                        totalComboScore = 0;
                        RESET_SESSION_STATS();
                        
                        TesseraBag.reset(currentLevel);
                        
                        hasHeldPiece = false;
                        canUseHold = true;
                        
                        leftHoldTime = 0.0f;
                        rightHoldTime = 0.0f;
                        dasTimer = 0.0f;
                        leftPressed = false;
                        rightPressed = false;
                        
                        linesSinceLastAbility = 0;
                        bombAbilityAvailable = practiceInfiniteBombs ? true : debugMode;
                        deliveryAbilityAvailable = false;
                        explosionEffects.clear();
                        glowEffects.clear();
                            thermometerParticles.clear();
                        
                        PieceType startType = TesseraBag.getNextPiece();
                        PieceShape startShape = getPieceShape(startType);
                        int startX = (GRID_WIDTH - startShape.width) / 2;
                        int firstFilledRow = findFirstFilledRow(startShape);
                        int startY = -firstFilledRow;
                        

                        if (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) {
                            startY = GRID_HEIGHT / 2 - startShape.height / 2;
                            gravityFlipPieceCount = 1;
                            gravityFlipped = false;
                        }
                        
                        activePiece = Piece(startX, startY, startType);
                        

                        if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                            int colorIndex = getColorIndexForPieceType(startType);
                            if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                            }
                        } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                            int randomIndex = rand() % currentConfig->colorPalette.size();
                            activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                        }
                        
                        std::cout << "Game restarted during play!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
                    }
                }
                

                if (gameOver) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape: {
                            std::cout << "ESC pressed! Returning to main menu..." << std::endl;
                            audioManager.switchToMenuMusic();
                            
                            gameState = GameState::MainMenu;
                            gameOver = false;
                            gameOverSoundPlayed = false;
                            gameOverMusicTimer = 0.0f;
                            selectedMenuOption = MenuOption::Start;
                            break;
                        }
                        case sf::Keyboard::Key::R: {
                            std::cout << "R key pressed! Restarting game..." << std::endl;
                            
                            applyGameTheme(currentTheme, audioManager, selectedGameModeOption, selectedClassicDifficulty, selectedChallengeMode, selectedThemeChoice);
                            
                            gameState = GameState::Playing;
                            gameOver = false;
                            gameOverSoundPlayed = false;
                            gameOverMusicTimer = 0.0f;
                            

                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            consecutiveBombsUsed = 0;
                            hardDropCooldown = 0.0f;
                            

                            if (sprintModeActive || challengeModeActive) {
                                sprintTimer = 0.0f;
                                autoDropTimer = 0.0f;
                                sprintCompleted = false;
                            }
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            
                            if (practiceModeActive) {

                                currentLevel = static_cast<int>(selectedPracticeStartLevel);
                                totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                            } else {
                                totalLinesCleared = 0;
                            displayedThermometerFill = 0.0f;
                                currentLevel = 0;
                            }
                            
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            
                            TesseraBag.reset(currentLevel);
                            
                            hasHeldPiece = false;
                            canUseHold = true;

                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = practiceInfiniteBombs ? true : debugMode;
                            explosionEffects.clear();
                            glowEffects.clear();
                            thermometerParticles.clear();
                            

                            PieceType startType = TesseraBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(startShape);
                            int startY = -firstFilledRow;
                            activePiece = Piece(startX, startY, startType);
                            

                            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                                int colorIndex = getColorIndexForPieceType(startType);
                                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                                }
                            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                                int randomIndex = rand() % currentConfig->colorPalette.size();
                                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
                            }
                            
                            std::cout << "Game restarted!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
                            
                            break;
                        }
                        default: break;
                    }
                }
                

                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Backspace: {
                        isFullscreen = !isFullscreen;
                        if (isFullscreen) {
                            auto desktopMode = sf::VideoMode::getDesktopMode();
                            window.create(desktopMode, "Tessera", sf::Style::None);
                            std::cout << "Switched to FULLSCREEN mode" << std::endl;
                        } else {
                            window.create(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Tessera", sf::Style::Default);
                            std::cout << "Switched to WINDOWED mode" << std::endl;
                        }
                        window.setFramerateLimit(144);
                        window.requestFocus();
                        break;
                    }
                    default: break;
                }
                }
            }
        }
        
        if (showVolumeIndicator && volumeIndicatorTimer > 0.0f) {
            volumeIndicatorTimer -= deltaTime;
            if (volumeIndicatorTimer <= 0.0f) {
                showVolumeIndicator = false;
            }
        }
        
        if (gameState == GameState::Extras) {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);
            
            float centerX = WINDOW_WIDTH / 2.0f;
            float centerY = WINDOW_HEIGHT / 2.0f;
            float startY = centerY - 200.0f;
            float spacing = 70.0f;
            

            for (int i = 0; i < 4; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mousePos.x >= centerX - 250 && mousePos.x <= centerX + 250 &&
                    mousePos.y >= selectorY && mousePos.y <= selectorY + 55) {
                    selectedExtrasOption = static_cast<ExtrasOption>(i);
                    break;
                }
            }
        }
        
        if (gameState == GameState::AchievementsView) {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);
            

            const int COLS = 5;
            const int ROWS = 5;
            const float CELL_SIZE = 128.0f;
            const float SPACING = 15.0f;
            const float START_X = 150.0f;
            const float START_Y = 220;
            
            hoveredAchievement = -1;
            
            for (int row = 0; row < ROWS; ++row) {
                for (int col = 0; col < COLS; ++col) {
                    int achievementId = row * COLS + col;
                    if (achievementId >= 25) break;
                    
                    float x = START_X + col * (CELL_SIZE + SPACING);
                    float y = START_Y + row * (CELL_SIZE + SPACING);
                    
                    if (mousePos.x >= x && mousePos.x <= x + CELL_SIZE &&
                        mousePos.y >= y && mousePos.y <= y + CELL_SIZE) {
                        hoveredAchievement = achievementId;
                        break;
                    }
                }
                if (hoveredAchievement != -1) break;
            }
        }
        
        if (gameState == GameState::Playing) {
        

        float targetCombo = static_cast<float>(currentCombo);
        if (std::abs(displayCombo - targetCombo) > 0.01f) {

            if (currentCombo == 0) {
                comboFadingOut = true;
            } else {

                float diff = targetCombo - displayCombo;
                displayCombo += diff * COMBO_ANIM_SPEED * deltaTime;
                

                if (std::abs(displayCombo - targetCombo) < 0.1f) {
                    displayCombo = targetCombo;
                }
            }
        }
        

        if (comboFadingOut) {

            comboFadeScale -= deltaTime * COMBO_FADE_SPEED;
            if (comboFadeScale <= 0.0f) {
                comboFadeScale = 0.0f;
                comboFadingOut = false;
                displayCombo = 0.0f;
                comboAnimTimer = 0.0f;
            }
        } else if (displayCombo > 0.0f && comboFadeScale < 1.0f) {

            comboFadeScale += deltaTime * COMBO_FADE_SPEED;
            if (comboFadeScale > 1.0f) {
                comboFadeScale = 1.0f;
            }
        }

        if (!gameOver) {
            sessionPlayTime += deltaTime;
            

            if (hardDropCooldown > 0.0f) {
                hardDropCooldown -= deltaTime;
                if (hardDropCooldown < 0.0f) {
                    hardDropCooldown = 0.0f;
                }
            }
        }

        bool currentLeftPressed = sf::Keyboard::isKeyPressed(keyBindings.moveLeft);
        bool currentRightPressed = sf::Keyboard::isKeyPressed(keyBindings.moveRight);
        

        if (!currentLeftPressed || currentRightPressed) {
            leftHoldTime = 0.0f;
        }
        if (!currentRightPressed || currentLeftPressed) {
            rightHoldTime = 0.0f;
        }
        

        if (currentLeftPressed && !currentRightPressed && !gameOver) {
            if (!leftPressed) {

                activePiece.moveLeft(grid);
                leftHoldTime = 0.0f;
                dasTimer = 0.0f;
            } else {

                leftHoldTime += deltaTime;
                if (leftHoldTime >= DAS_DELAY) {
                    dasTimer += deltaTime;
                    if (dasTimer >= DAS_REPEAT) {
                        activePiece.moveLeft(grid);
                        dasTimer = 0.0f;
                    }
                }
            }
        }
        

        if (currentRightPressed && !currentLeftPressed && !gameOver) {
            if (!rightPressed) {

                activePiece.moveRight(grid);
                rightHoldTime = 0.0f;
                dasTimer = 0.0f;
            } else {

                rightHoldTime += deltaTime;
                if (rightHoldTime >= DAS_DELAY) {
                    dasTimer += deltaTime;
                    if (dasTimer >= DAS_REPEAT) {
                        activePiece.moveRight(grid);
                        dasTimer = 0.0f;
                    }
                }
            }
        }
        

        leftPressed = currentLeftPressed;
        rightPressed = currentRightPressed;
        

        bool fastFall = sf::Keyboard::isKeyPressed(keyBindings.quickFall);
        if (!gameOver) {


            float currentGravity = 1.5f;
            if (currentConfig && !currentConfig->gravityTable.empty()) {
                int levelIndex = std::min(currentLevel, static_cast<int>(currentConfig->gravityTable.size()) - 1);
                currentGravity = currentConfig->gravityTable[levelIndex];
            } else {

                const float* gravityTable = getGravityTable(selectedClassicDifficulty);
                currentGravity = gravityTable[std::min(currentLevel, MAX_LEVEL)];
            }
            

            bool useGravityFlip = (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) ? gravityFlipped : false;
            activePiece.update(deltaTime, fastFall, grid, currentLevel, currentGravity, useGravityFlip);
            

            if (challengeModeActive && selectedChallengeMode == ChallengeMode::Vanishing) {
                for (int y = 0; y < GRID_HEIGHT; y++) {
                    for (int x = 0; x < GRID_WIDTH; x++) {
                        if (grid[y][x].occupied && grid[y][x].isVanishing) {
                            grid[y][x].vanishTimer += deltaTime;
                            


                        }
                    }
                }
            }
            

            bool hasLineGoal = currentConfig && currentConfig->hasLineGoal;
            if (hasLineGoal) {
                sprintTimer += deltaTime;
            }
            

            if (challengeModeActive && selectedChallengeMode == ChallengeMode::AutoDrop && !gameOver) {
                autoDropTimer += deltaTime;
                if (autoDropTimer >= AUTO_DROP_INTERVAL) {
                    autoDropTimer = 0.0f;
                    

                    int dropDistance = activePiece.moveGround(grid, false);
                    int dropPoints = dropDistance * HARD_DROP_POINTS_PER_CELL;
                    totalScore += dropPoints;
                    totalHardDropScore += dropPoints;
                    std::cout << "Auto drop: " << dropDistance << " cells = +" << dropPoints << " points" << std::endl;
                    audioManager.playDropSound();
                }
            }
        }
        
        if (activePiece.hasStopped() && !gameOver) {

            hardDropCooldown = HARD_DROP_COOLDOWN_TIME;
            

            if (challengeModeActive && selectedChallengeMode == ChallengeMode::AutoDrop) {
                autoDropTimer = 0.0f;
            }
            
            AbilityType usedAbility = activePiece.getAbility();
            bool isVanishing = (challengeModeActive && selectedChallengeMode == ChallengeMode::Vanishing);
            activePiece.ChangeToStatic(grid, usedAbility, &audioManager, &explosionEffects, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer, &consecutiveBombsUsed, &saveData, &achievementPopups, isVanishing);
            

            if (usedAbility != AbilityType::Bomb) {
                if (consecutiveBombsUsed > 0) {
                    std::cout << "[BOMB] Streak broken! Reset to 0" << std::endl;
                }
                consecutiveBombsUsed = 0;
            }
            

            sessionPiecesPlaced++;
            

            bool isPetrifyMode = challengeModeActive && selectedChallengeMode == ChallengeMode::Petrify;
            if (isPetrifyMode) {
                updatePetrifyCounters(grid);
            }

            int clearedLines;
            bool isRaceMode = sprintModeActive || challengeModeActive;
            if (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) {
                clearedLines = clearFullLinesGravityFlip(grid, audioManager, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer);
            } else {
                clearedLines = clearFullLines(grid, audioManager, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer, isPetrifyMode, isRaceMode ? &thermometerParticles : nullptr, isRaceMode);
            }
            totalLinesCleared += clearedLines;
            

            bool hasLineGoal = currentConfig && currentConfig->hasLineGoal;
            int lineGoal = hasLineGoal ? currentConfig->lineGoal : 0;
            
            if (hasLineGoal && totalLinesCleared >= lineGoal) {
                gameOver = true;
                gameOverDelayTimer = 0.0f;
                gameOverScreenVisible = false;
                gameOverBlocksFalling = true;
                gameOverPauseComplete = true;
                sprintCompleted = true;
                

                shakeIntensity = 15.0f;
                shakeDuration = 0.4f;
                shakeTimer = 0.0f;
                

                for (int i = 0; i < GRID_HEIGHT; ++i) {
                    for (int j = 0; j < GRID_WIDTH; ++j) {
                        if (grid[i][j].occupied) {
                            float cellX = GRID_OFFSET_X + j * CELL_SIZE;
                            float cellY = GRID_OFFSET_Y + i * CELL_SIZE;
                            fallingCells.emplace_back(cellX, cellY, grid[i][j].color, grid[i][j].textureType);
                        }
                        grid[i][j] = Cell();
                    }
                }
                
                std::cout << "MODE COMPLETED! Time: " << sprintTimer << " seconds | Lines: " << totalLinesCleared << "/" << lineGoal << std::endl;
                

                if (sprintModeActive && lineGoal == 48 && sprintTimer < 133.7f && !challengeModeActive && !practiceModeActive) {
                    unlockAchievement(saveData, Achievement::Blitz48Under230, &achievementPopups, &audioManager);
                }
                

                if (sprintModeActive) {
                    float* bestTimePtr = nullptr;
                    if (sprintTargetLines == 1) bestTimePtr = &saveData.bestTimeSprint1;
                    else if (sprintTargetLines == 24) bestTimePtr = &saveData.bestTimeSprint24;
                    else if (sprintTargetLines == 48) bestTimePtr = &saveData.bestTimeSprint48;
                    else if (sprintTargetLines == 96) bestTimePtr = &saveData.bestTimeSprint96;
                    
                    if (bestTimePtr && (*bestTimePtr == 0.0f || sprintTimer < *bestTimePtr)) {
                        *bestTimePtr = sprintTimer;
                        std::cout << "NEW BEST TIME for " << sprintTargetLines << " lines: " << sprintTimer << " seconds!" << std::endl;
                        saveGameData(saveData);
                    }
                }
                

                if (challengeModeActive) {
                    float* bestTimePtr = nullptr;
                    Achievement challengeAchievement;
                    bool hasAchievement = true;
                    
                    switch (selectedChallengeMode) {
                        case ChallengeMode::Debug:
                            bestTimePtr = &saveData.bestTimeChallengeDebug;
                            hasAchievement = false;
                            break;
                        case ChallengeMode::TheForest:
                            bestTimePtr = &saveData.bestTimeChallengeTheForest;
                            challengeAchievement = Achievement::ChallengeTheForest;
                            break;
                        case ChallengeMode::Randomness:
                            bestTimePtr = &saveData.bestTimeChallengeRandomness;
                            challengeAchievement = Achievement::ChallengeRandomness;
                            break;
                        case ChallengeMode::NonStraight:
                            bestTimePtr = &saveData.bestTimeChallengeNonStraight;
                            challengeAchievement = Achievement::ChallengeNonStraight;
                            break;
                        case ChallengeMode::OneRot:
                            bestTimePtr = &saveData.bestTimeChallengeOneRot;
                            challengeAchievement = Achievement::ChallengeOneRot;
                            break;
                        case ChallengeMode::ChristopherCurse:
                            bestTimePtr = &saveData.bestTimeChallengeChristopherCurse;
                            challengeAchievement = Achievement::ChallengeChristopherCurse;
                            break;
                        case ChallengeMode::Vanishing:
                            bestTimePtr = &saveData.bestTimeChallengeVanishing;
                            challengeAchievement = Achievement::ChallengeVanishing;
                            break;
                        case ChallengeMode::AutoDrop:
                            bestTimePtr = &saveData.bestTimeChallengeAutoDrop;
                            challengeAchievement = Achievement::ChallengeAutoDrop;
                            break;
                        case ChallengeMode::GravityFlip:
                            bestTimePtr = &saveData.bestTimeChallengeGravityFlip;
                            hasAchievement = false;
                            break;
                        case ChallengeMode::Petrify:
                            bestTimePtr = &saveData.bestTimeChallengePetrify;
                            hasAchievement = false;
                            break;
                    }
                    
                    if (bestTimePtr) {
                        bool isFirstCompletion = (*bestTimePtr == 0.0f);
                        
                        if (isFirstCompletion || sprintTimer < *bestTimePtr) {
                            *bestTimePtr = sprintTimer;
                            std::cout << "NEW BEST TIME for challenge: " << sprintTimer << " seconds!" << std::endl;
                            saveGameData(saveData);
                        }
                        

                        if (isFirstCompletion && hasAchievement) {
                            unlockAchievement(saveData, challengeAchievement, &achievementPopups, &audioManager);
                        }
                    }
                }
                

                audioManager.stopAllMusic();
                audioManager.playGameWinSound();
                gameOverSoundPlayed = true;
                gameOverMusicTimer = 0.0f;
                std::cout << "Playing GAME WIN music (challenge completed)!" << std::endl;
            }
            
            if (clearedLines > 0) {

                if (clearedLines >= 4) {
                    saveData.totalPerfectClears++;
                }
                
                int fullScore = calculateScore(clearedLines);
                int baseScore = clearedLines * 1000;
                int lineBonus = fullScore - baseScore;
                int comboBonus = currentCombo * COMBO_BONUS_PER_LINE * clearedLines;
                lastMoveScore = fullScore + comboBonus;
                totalScore += lastMoveScore;
                totalLineScore += baseScore;
                totalComboScore += lineBonus + comboBonus;
                
                currentCombo += clearedLines;
                

                if (currentCombo >= 12 && !challengeModeActive && !practiceModeActive) {
                    unlockAchievement(saveData, Achievement::Combo10, &achievementPopups, &audioManager);
                }
                

                if (clearedLines >= 5 && !challengeModeActive && !practiceModeActive) {
                    unlockAchievement(saveData, Achievement::Combo5OneClear, &achievementPopups, &audioManager);
                }
                

                if (clearedLines >= 6 && !challengeModeActive && !practiceModeActive) {
                    unlockAchievement(saveData, Achievement::Combo6OneClear, &achievementPopups, &audioManager);
                }
                

                if (currentCombo > maxComboThisGame) {
                    maxComboThisGame = currentCombo;
                }
                

                if (currentCombo > saveData.maxComboEver) {
                    saveData.maxComboEver = currentCombo;
                }
                
                std::cout << "Lines cleared: " << clearedLines << " | Base: " << baseScore << " | Combo before: x" << (currentCombo - clearedLines) << " (+" << comboBonus << ") | Total: +" << lastMoveScore << " | New combo: x" << currentCombo << " | Score: " << totalScore << std::endl;
                
                linesSinceLastAbility += clearedLines;
                
                if (selectedAbilityChoice == AbilityChoice::Bomb && linesSinceLastAbility >= LINES_FOR_BOMB) {
                    bombAbilityAvailable = true;
                    std::cout << "BOMB ABILITY READY! Press 'I' to activate!" << std::endl;
                } else if (selectedAbilityChoice == AbilityChoice::Delivery && linesSinceLastAbility >= LINES_FOR_DELIVERY) {
                    deliveryAbilityAvailable = true;
                    std::cout << "DELIVERY ABILITY READY! Press 'I' to activate!" << std::endl;
                } else if (selectedAbilityChoice == AbilityChoice::Stomp && linesSinceLastAbility >= LINES_FOR_STOMP) {
                    stompAbilityAvailable = true;
                    std::cout << "STOMP ABILITY READY! Press 'I' to activate!" << std::endl;
                }
                

                if (totalLinesCleared >= 100) {
                    bool hasEmptyColumn = false;
                    for (int x = 0; x < GRID_WIDTH; x++) {
                        bool columnEmpty = true;
                        for (int y = 0; y < GRID_HEIGHT; y++) {
                            if (grid[y][x].occupied) {
                                columnEmpty = false;
                                break;
                            }
                        }
                        if (columnEmpty) {
                            hasEmptyColumn = true;
                            break;
                        }
                    }
                    
                    if (hasEmptyColumn && !challengeModeActive && !practiceModeActive) {
                        unlockAchievement(saveData, Achievement::PerfectClear, &achievementPopups, &audioManager);
                        std::cout << "[ACHIEVEMENT] Perfect Clear! At least one column is empty to the floor with " << totalLinesCleared << " lines cleared!" << std::endl;
                    }
                }
            } else {
                lastMoveScore = 0;
                currentCombo = std::max(0, currentCombo - 2);
                std::cout << "No lines cleared | Combo decreased to: " << currentCombo << std::endl;
            }
            

            if (!practiceModeActive) {
                int newLevel = calculateLevel(totalLinesCleared);
                if (newLevel != currentLevel) {
                    currentLevel = newLevel;
                    TesseraBag.updateLevel(currentLevel);
                    std::cout << "LEVEL UP! Level " << currentLevel << " reached! (Lines: " << totalLinesCleared << ")" << std::endl;
                }
            }
            
            PieceType randomType = TesseraBag.getNextPiece();
            std::cout << "Respawn: " << pieceTypeToString(randomType) << " (Bag System)" << std::endl;
            PieceShape newShape = getPieceShape(randomType);
            int spawnX = (GRID_WIDTH - newShape.width) / 2;
            

            int firstFilledRow = findFirstFilledRow(newShape);
            int spawnY = -firstFilledRow;
            

            if (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) {
                gravityFlipPieceCount++;
                gravityFlipped = (gravityFlipPieceCount % 2 == 0);
                

                spawnY = GRID_HEIGHT / 2 - newShape.height / 2;
                
                std::cout << "GravityFlip: Piece #" << gravityFlipPieceCount << " | Gravity: " << (gravityFlipped ? "UP" : "DOWN") << " | SpawnY: " << spawnY << std::endl;
            }
            
            activePiece = Piece(spawnX, spawnY, randomType);
            

            if (currentConfig && currentConfig->useTypeBasedColors && !currentConfig->colorPalette.empty()) {
                int colorIndex = getColorIndexForPieceType(randomType);
                if (colorIndex < static_cast<int>(currentConfig->colorPalette.size())) {
                    activePiece.setColor(currentConfig->colorPalette[colorIndex]);
                }
            } else if (currentConfig && currentConfig->useRandomColorPalette && !currentConfig->colorPalette.empty()) {
                int randomIndex = rand() % currentConfig->colorPalette.size();
                activePiece.setColor(currentConfig->colorPalette[randomIndex]);
            }
            

            if (challengeModeActive && selectedChallengeMode == ChallengeMode::OneRot) {
                currentPieceRotations = 0;
            }
            
            if (activePiece.collidesAt(grid, spawnX, spawnY)) {
                gameOver = true;
                gameOverDelayTimer = 0.0f;
                gameOverScreenVisible = false;
                gameOverBlocksFalling = false;
                gameOverPauseComplete = false;
                

                hasCollisionPiece = true;
                collisionPieceType = randomType;
                collisionPieceX = spawnX;
                collisionPieceY = spawnY;
                

                shakeIntensity = 20.0f;
                shakeDuration = 0.5f;
                shakeTimer = 0.0f;
                

                
                std::cout << "GAME OVER! New piece overlaps with existing blocks at spawn position (" << spawnX << ", " << spawnY << ")" << std::endl;
                std::cout << "Final Score: " << totalScore << " | Lines: " << totalLinesCleared << " | Level: " << currentLevel << std::endl;
                
                bool playWinSound = false;
                gameOverNewHighScore = false;
                gameOverHighScoreBaseline = 0;


                if (!debugMode && !sprintModeActive && !challengeModeActive && !practiceModeActive) {
                    int previousHighScore = (selectedClassicDifficulty == ClassicDifficulty::Normal)
                        ? saveData.highScoreClassicNormal
                        : saveData.highScoreClassicHard;
                    gameOverHighScoreBaseline = previousHighScore;
                    gameOverNewHighScore = totalScore > previousHighScore;

                    bool madeTopThree = insertNewScore(saveData, totalScore, totalLinesCleared, currentLevel, selectedClassicDifficulty);
                    

                    if (madeTopThree) {
                        playWinSound = true;
                        std::cout << "CONGRATULATIONS! You made it to the TOP 3!" << std::endl;
                    }
                    

                    if (totalScore >= 200000 && !bombUsedThisGame && !challengeModeActive && !practiceModeActive) {
                        unlockAchievement(saveData, Achievement::Score200kNoBomb, &achievementPopups, &audioManager);
                    }
                    

                    if (totalScore >= 400000 && selectedClassicDifficulty == ClassicDifficulty::Hard && !challengeModeActive && !practiceModeActive) {
                        unlockAchievement(saveData, Achievement::Score400kMedHard, &achievementPopups, &audioManager);
                    }
                    
                    bool recordsUpdated = false;
                    if (totalLinesCleared > saveData.bestLines) {
                        saveData.bestLines = totalLinesCleared;
                        recordsUpdated = true;
                        std::cout << "NEW BEST LINES: " << totalLinesCleared << "!" << std::endl;
                    }
                    if (currentLevel > saveData.bestLevel) {
                        saveData.bestLevel = currentLevel;
                        recordsUpdated = true;
                        std::cout << "NEW BEST LEVEL: " << currentLevel << "!" << std::endl;
                    }
                    

                    saveData.totalGamesPlayed++;
                    saveData.totalLinesCleared += totalLinesCleared;
                    saveData.totalPiecesPlaced += sessionPiecesPlaced;
                    saveData.totalScore += totalScore;
                    saveData.totalPlayTimeSeconds += sessionPlayTime;
                    std::cout << "Statistics updated: +1 game, +" << totalLinesCleared << " lines, +" << sessionPiecesPlaced << " pieces, +" << totalScore << " score, +" << sessionPlayTime << "s playtime" << std::endl;
                    

                    saveData.totalGamesPlayed++;
                    saveData.totalLinesCleared += totalLinesCleared;
                    saveData.totalPiecesPlaced += sessionPiecesPlaced;
                    saveData.totalScore += totalScore;
                    saveData.totalPlayTimeSeconds += sessionPlayTime;
                    std::cout << "Session stats saved: " << sessionPiecesPlaced << " pieces, " << totalLinesCleared << " lines, " << sessionPlayTime << "s playtime" << std::endl;
                    
                    saveData.masterVolume = audioManager.getMasterVolume();
                    saveData.isMuted = audioManager.isMutedStatus();
                    
                    saveGameData(saveData);
                    
                    std::cout << "Game data saved with new scores!" << std::endl;
                } else if (practiceModeActive) {
                    std::cout << "Score not saved (PRACTICE MODE)" << std::endl;
                } else {
                    std::cout << "Score not saved (DEBUG MODE)" << std::endl;
                }
                

                audioManager.stopAllMusic();
                if (playWinSound) {
                    audioManager.playGameWinSound();
                    std::cout << "Playing GAME WIN music!" << std::endl;
                } else {
                    audioManager.playGameOverSound();
                }
                gameOverSoundPlayed = true;
                gameOverMusicTimer = 0.0f;
            }
            
            canUseHold = true;
            

            leftHoldTime = 0.0f;
            rightHoldTime = 0.0f;
            dasTimer = 0.0f;
            leftPressed = false;
            rightPressed = false;
        }
        
        }
        

        for (auto it = achievementPopups.begin(); it != achievementPopups.end(); ) {
            it->update(deltaTime);
            if (it->isFinished()) {
                it = achievementPopups.erase(it);
            } else {
                ++it;
            }
        }
        

        if (shakeTimer < shakeDuration) {
            shakeTimer += deltaTime;
        }
        

        for (auto it = explosionEffects.begin(); it != explosionEffects.end(); ) {
            it->timer -= deltaTime;
            if (it->timer <= 0.0f) {
                it = explosionEffects.erase(it);
            } else {
                ++it;
            }
        }
        

        for (auto it = glowEffects.begin(); it != glowEffects.end(); ) {
            it->update(deltaTime);
            if (it->isFinished()) {
                it = glowEffects.erase(it);
            } else {
                ++it;
            }
        }
        

        for (auto it = thermometerParticles.begin(); it != thermometerParticles.end(); ) {
            it->update(deltaTime);
            if (it->isFinished()) {
                it = thermometerParticles.erase(it);
            } else {
                ++it;
            }
        }
        

        for (auto it = fallingCells.begin(); it != fallingCells.end(); ) {
            it->update(deltaTime);
            if (it->isFinished()) {
                it = fallingCells.erase(it);
            } else {
                ++it;
            }
        }
        

        if (sprintModeActive || (currentConfig && currentConfig->hasLineGoal)) {
            int targetLines = sprintModeActive ? sprintTargetLines : currentConfig->lineGoal;
            float targetFill = static_cast<float>(totalLinesCleared) / static_cast<float>(targetLines);
            targetFill = std::min(1.0f, targetFill);
            

            float fillSpeed = 2.0f;
            if (displayedThermometerFill < targetFill) {
                displayedThermometerFill += deltaTime * fillSpeed;
                if (displayedThermometerFill > targetFill) {
                    displayedThermometerFill = targetFill;
                }
            }
        }
        

        if (gameState != GameState::Playing && gameState != GameState::Paused && gameState != GameState::SplashScreen && gameState != GameState::FirstTimeSetup) {
            updateBackgroundPieces(backgroundPieces, deltaTime);
            
            backgroundSpawnTimer += deltaTime;
            if (backgroundSpawnTimer >= BACKGROUND_SPAWN_INTERVAL) {
                spawnBackgroundPiece(backgroundPieces);
                backgroundSpawnTimer = 0.0f;
            }
        }
        

        if (gameState == GameState::Playing || gameState == GameState::Paused) {
            updateGameBackgroundPieces(gameBackgroundPieces, deltaTime);
            
            gameBackgroundSpawnTimer += deltaTime;
            if (gameBackgroundSpawnTimer >= GAME_BACKGROUND_SPAWN_INTERVAL) {
                spawnGameBackgroundPiece(gameBackgroundPieces);
                gameBackgroundSpawnTimer = 0.0f;
            }
        }
        
        window.clear(sf::Color(0, 0, 0));
        


        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePixelPos);
        float mouseX = mousePosF.x;
        float mouseY = mousePosF.y;
        float centerX = WINDOW_WIDTH / 2.0f;
        float centerY = WINDOW_HEIGHT / 2.0f;
        

        sf::Vector2f currentMousePos(mouseX, mouseY);
        if (lastMousePos.x >= 0.0f && currentMousePos != lastMousePos) {
            useKeyboardNavigation = false;
        }
        lastMousePos = currentMousePos;
        
        if (gameState == GameState::MainMenu && !useKeyboardNavigation) {


            float startWidth = calculateButtonWidth(menuFont, "START", 86.0f) / 2;
            float extrasWidth = calculateButtonWidth(menuFont, "EXTRAS", 86.0f) / 2;
            float optionsWidth = calculateButtonWidth(menuFont, "OPTIONS", 86.0f) / 2;
            float exitWidth = calculateButtonWidth(menuFont, "EXIT", 86.0f) / 2;
            float buttonHeight = 80.0f;

            if (mouseX >= centerX - startWidth && mouseX <= centerX + startWidth &&
                mouseY >= centerY - 60 && mouseY <= centerY - 60 + buttonHeight) {
                selectedMenuOption = MenuOption::Start;
            }

            else if (mouseX >= centerX - extrasWidth && mouseX <= centerX + extrasWidth &&
                     mouseY >= centerY + 50 && mouseY <= centerY + 50 + buttonHeight) {
                selectedMenuOption = MenuOption::Extras;
            }

            else if (mouseX >= centerX - optionsWidth && mouseX <= centerX + optionsWidth &&
                     mouseY >= centerY + 160 && mouseY <= centerY + 160 + buttonHeight) {
                selectedMenuOption = MenuOption::Options;
            }

            else if (mouseX >= centerX - exitWidth && mouseX <= centerX + exitWidth &&
                     mouseY >= centerY + 270 && mouseY <= centerY + 270 + buttonHeight) {
                selectedMenuOption = MenuOption::Exit;
            }
        } else if (gameState == GameState::GameModeSelect && !useKeyboardNavigation) {

            float startY = centerY - 80.0f;
            float spacing = 90.0f;
            float buttonHeight = 80.0f;
            std::string options[3] = {"CLASSIC", "BLITZ", "CHALLENGE"};
            
            for (int i = 0; i < 3; i++) {
                float selectorY = startY + i * spacing - 5;
                float halfWidth = calculateButtonWidth(menuFont, options[i], 86.0f) / 2;
                if (mouseX >= centerX - halfWidth && mouseX <= centerX + halfWidth &&
                    mouseY >= selectorY && mouseY <= selectorY + buttonHeight) {
                    selectedGameModeOption = static_cast<GameModeOption>(i);
                    break;
                }
            }
        } else if (gameState == GameState::ClassicDifficultySelect && !useKeyboardNavigation) {

            float startY = centerY - 80.0f;
            float spacing = 90.0f;
            float buttonHeight = 80.0f;

            ClassicDifficulty menuToDifficulty[] = {ClassicDifficulty::Hard, ClassicDifficulty::Normal};
            
            for (int i = 0; i < 2; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mouseX >= centerX - 275 && mouseX <= centerX + 275 &&
                    mouseY >= selectorY && mouseY <= selectorY + buttonHeight) {
                    selectedClassicDifficulty = menuToDifficulty[i];
                    break;
                }
            }
        } else if (gameState == GameState::SprintLinesSelect && !useKeyboardNavigation) {

            float startY = centerY - 80.0f;
            float spacing = 90.0f;
            float buttonHeight = 80.0f;
            int numOptions = debugMode ? 4 : 3;
            int startEnumValue = debugMode ? 0 : 1;
            
            for (int i = 0; i < numOptions; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mouseX >= centerX - 275 && mouseX <= centerX + 275 &&
                    mouseY >= selectorY && mouseY <= selectorY + buttonHeight) {
                    selectedSprintLines = static_cast<SprintLines>(i + startEnumValue);
                    break;
                }
            }
        } else if (gameState == GameState::ChallengeSelect && !useKeyboardNavigation) {

            float startY = centerY - 150.0f;
            float spacing = 100.0f;
            float buttonHeight = 80.0f;
            float buttonWidth = 400.0f;
            float columnSpacing = 450.0f;
            int numOptions = 9;
            int itemsPerColumn = (numOptions + 1) / 2;
            float leftColumnX = centerX - columnSpacing / 2;
            float rightColumnX = centerX + columnSpacing / 2;
            
            for (int i = 0; i < numOptions; i++) {
                int column = (i < itemsPerColumn) ? 0 : 1;
                int rowIndex = (column == 0) ? i : (i - itemsPerColumn);
                float columnX = (column == 0) ? leftColumnX : rightColumnX;
                float selectorY = startY + rowIndex * spacing - 5;
                
                if (mouseX >= columnX - buttonWidth / 2 && mouseX <= columnX + buttonWidth / 2 &&
                    mouseY >= selectorY && mouseY <= selectorY + buttonHeight) {

                    selectedChallengeMode = static_cast<ChallengeMode>(i + 1);
                    break;
                }
            }
        } else if (gameState == GameState::PracticeSelect && !useKeyboardNavigation) {

            float startY = centerY - 220.0f;
            float spacing = 85.0f;
            float buttonHeight = 80.0f;
            

            for (int i = 0; i < 4; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mouseX >= centerX - 350 && mouseX <= centerX + 350 &&
                    mouseY >= selectorY && mouseY <= selectorY + buttonHeight) {
                    selectedPracticeOption = i;
                    break;
                }
            }
            

            float startButtonY = startY + spacing * 4 + 25;
            float startPracticeHalfWidth = calculateButtonWidth(menuFont, "START PRACTICE", 86.0f) / 2;
            if (mouseX >= centerX - startPracticeHalfWidth && mouseX <= centerX + startPracticeHalfWidth &&
                mouseY >= startButtonY && mouseY <= startButtonY + buttonHeight) {
                selectedPracticeOption = 4;
            }
        } else if (gameState == GameState::Extras && !useKeyboardNavigation) {

            float startY = centerY + MenuConfig::ExtrasMenu::START_Y_OFFSET;
            float spacing = MenuConfig::ExtrasMenu::SPACING;
            float buttonHeight = 80.0f;
            
            for (int i = 0; i < MenuConfig::ExtrasMenu::NUM_OPTIONS; i++) {
                float selectorY = startY + i * spacing - 5;
                float halfWidth = calculateButtonWidth(menuFont, MenuConfig::getExtrasOptionText(i), 86.0f) / 2;
                if (mouseX >= centerX - halfWidth && mouseX <= centerX + halfWidth &&
                    mouseY >= selectorY && mouseY <= selectorY + buttonHeight) {
                    selectedExtrasOption = static_cast<ExtrasOption>(i);
                    break;
                }
            }
        } else if (gameState == GameState::Options && !useKeyboardNavigation) {

            float buttonHeight = 80.0f;
            float audioHalfWidth = calculateButtonWidth(menuFont, MenuConfig::OptionsMenu::AUDIO, 86.0f) / 2;
            float rebindKeysHalfWidth = calculateButtonWidth(menuFont, MenuConfig::OptionsMenu::REBIND_KEYS, 86.0f) / 2;
            float clearDataHalfWidth = calculateButtonWidth(menuFont, MenuConfig::OptionsMenu::CLEAR_ALL_DATA, 86.0f) / 2;
            

            if (mouseX >= centerX - audioHalfWidth && mouseX <= centerX + audioHalfWidth &&
                mouseY >= centerY + MenuConfig::OptionsMenu::OPTION1_Y_OFFSET && 
                mouseY <= centerY + MenuConfig::OptionsMenu::OPTION1_Y_OFFSET + buttonHeight) {
                selectedOptionsOption = OptionsMenuOption::Audio;
            }

            else if (mouseX >= centerX - rebindKeysHalfWidth && mouseX <= centerX + rebindKeysHalfWidth &&
                     mouseY >= centerY + MenuConfig::OptionsMenu::OPTION2_Y_OFFSET && 
                     mouseY <= centerY + MenuConfig::OptionsMenu::OPTION2_Y_OFFSET + buttonHeight) {
                selectedOptionsOption = OptionsMenuOption::RebindKeys;
            }
            
            else if (mouseX >= centerX - clearDataHalfWidth && mouseX <= centerX + clearDataHalfWidth &&
                     mouseY >= centerY + MenuConfig::OptionsMenu::OPTION3_Y_OFFSET && 
                     mouseY <= centerY + MenuConfig::OptionsMenu::OPTION3_Y_OFFSET + buttonHeight) {
                selectedOptionsOption = OptionsMenuOption::ClearScores;
            }
        } else if (gameState == GameState::AudioSettings && !useKeyboardNavigation) {

            float sliderCenterX = centerX + 100;
            float sliderHitHeight = MenuConfig::AudioMenu::SLIDER_HANDLE_HEIGHT + 20;
            

            if (mouseX >= sliderCenterX - MenuConfig::AudioMenu::SLIDER_WIDTH/2 && 
                mouseX <= sliderCenterX + MenuConfig::AudioMenu::SLIDER_WIDTH/2 &&
                mouseY >= centerY + MenuConfig::AudioMenu::OPTION1_Y_OFFSET - sliderHitHeight/2 && 
                mouseY <= centerY + MenuConfig::AudioMenu::OPTION1_Y_OFFSET + sliderHitHeight/2) {
                selectedAudioOption = AudioOption::MainVolume;
            }

            else if (mouseX >= sliderCenterX - MenuConfig::AudioMenu::SLIDER_WIDTH/2 && 
                     mouseX <= sliderCenterX + MenuConfig::AudioMenu::SLIDER_WIDTH/2 &&
                     mouseY >= centerY + MenuConfig::AudioMenu::OPTION2_Y_OFFSET - sliderHitHeight/2 && 
                     mouseY <= centerY + MenuConfig::AudioMenu::OPTION2_Y_OFFSET + sliderHitHeight/2) {
                selectedAudioOption = AudioOption::MusicVolume;
            }

            else if (mouseX >= sliderCenterX - MenuConfig::AudioMenu::SLIDER_WIDTH/2 && 
                     mouseX <= sliderCenterX + MenuConfig::AudioMenu::SLIDER_WIDTH/2 &&
                     mouseY >= centerY + MenuConfig::AudioMenu::OPTION3_Y_OFFSET - sliderHitHeight/2 && 
                     mouseY <= centerY + MenuConfig::AudioMenu::OPTION3_Y_OFFSET + sliderHitHeight/2) {
                selectedAudioOption = AudioOption::SfxVolume;
            }
        } else if (gameState == GameState::Paused && !useKeyboardNavigation) {




            

            if (mouseX >= centerX - 150 && mouseX <= centerX + 150 &&
                mouseY >= centerY - 50 && mouseY <= centerY) {
                selectedPauseOption = PauseOption::Resume;
            }

            else if (mouseX >= centerX - 150 && mouseX <= centerX + 150 &&
                     mouseY >= centerY + 20 && mouseY <= centerY + 70) {
                selectedPauseOption = PauseOption::Restart;
            }

            else if (mouseX >= centerX - 150 && mouseX <= centerX + 150 &&
                     mouseY >= centerY + 90 && mouseY <= centerY + 140) {
                selectedPauseOption = PauseOption::QuitToMenu;
            }
        }
        

        sf::View view(sf::Vector2f(static_cast<float>(WINDOW_WIDTH) / 2.0f, static_cast<float>(WINDOW_HEIGHT) / 2.0f),
                      sf::Vector2f(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)));
        
        if (shakeTimer < shakeDuration) {
            float progress = shakeTimer / shakeDuration;
            float currentIntensity = shakeIntensity * (1.0f - progress);
            
            float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * currentIntensity * 2.0f;
            float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * currentIntensity * 2.0f;
            
            static int debugCounter = 0;
            if (debugCounter++ % 10 == 0) {
                std::cout << "SHAKE ACTIVE! Timer: " << shakeTimer << "/" << shakeDuration 
                          << " Intensity: " << currentIntensity 
                          << " Offset: (" << offsetX << ", " << offsetY << ")" << std::endl;
            }
            
            view.setCenter(sf::Vector2f(WINDOW_WIDTH / 2.0f + offsetX, WINDOW_HEIGHT / 2.0f + offsetY));
        } else {
            view.setCenter(sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
        }
        window.setView(view);
        
        if (gameState == GameState::SplashScreen) {
            splashElapsedTime += deltaTime;
            
            switch (splashSequenceStep) {
                case 0:
                    blackScreenAlpha = 1.0f;
                    if (splashElapsedTime >= 1.0f) {
                        audioManager.playSplashMusic();
                        splashElapsedTime = 0.0f;
                        splashSequenceStep++;
                    }
                    break;
                    
                case 1:
                    blackScreenAlpha = std::max(0.0f, 1.0f - (splashElapsedTime / FADE_DURATION));
                    if (blackScreenAlpha <= 0.0f) {
                        splashElapsedTime = 0.0f;
                        splashSequenceStep++;
                    }
                    break;
                    
                case 2:
                    blackScreenAlpha = 0.0f;
                    if (splashElapsedTime >= 3.0f) {
                        splashElapsedTime = 0.0f;
                        splashSequenceStep++;
                    }
                    break;
                    
                case 3:
                    blackScreenAlpha = std::min(1.0f, splashElapsedTime / FADE_DURATION);
                    if (blackScreenAlpha >= 1.0f) {
                        splashElapsedTime = 0.0f;
                        splashSequenceStep++;
                    }
                    break;
                    
                case 4:
                    blackScreenAlpha = 1.0f;
                    if (splashElapsedTime >= 1.0f) {
                        splashElapsedTime = 0.0f;
                        splashSequenceStep++;
                    }
                    break;
                    
                case 5:
                    blackScreenAlpha = std::max(0.0f, 1.0f - (splashElapsedTime / FADE_DURATION));
                    if (blackScreenAlpha <= 0.0f) {

                        constexpr int CURRENT_SETUP_VERSION = 1;
                        if (saveData.setupVersion < CURRENT_SETUP_VERSION) {
                            gameState = GameState::FirstTimeSetup;
                            std::cout << "First time setup required (version " << saveData.setupVersion << " < " << CURRENT_SETUP_VERSION << ")" << std::endl;
                        } else {
                            gameState = GameState::MainMenu;
                            audioManager.playMenuMusic();
                            std::cout << "Splash sequence finished, transitioning to main menu" << std::endl;
                        }
                        showCustomCursor = true;
                    }
                    break;
            }
            

            if (splashLoaded && splashSequenceStep >= 1 && splashSequenceStep <= 3) {
                sf::Sprite splashSprite(splashTexture);
                

                sf::Vector2u textureSize = splashTexture.getSize();
                float scaleX = WINDOW_WIDTH / static_cast<float>(textureSize.x);
                float scaleY = WINDOW_HEIGHT / static_cast<float>(textureSize.y);
                float scale = std::min(scaleX, scaleY);
                
                splashSprite.setScale(sf::Vector2f(scale, scale));
                
                sf::Vector2f spriteSize(textureSize.x * scale, textureSize.y * scale);
                sf::Vector2f position((WINDOW_WIDTH - spriteSize.x) / 2.0f, (WINDOW_HEIGHT - spriteSize.y) / 2.0f);
                splashSprite.setPosition(position);
                
                window.draw(splashSprite);
            }
            

            sf::RectangleShape blackScreen(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
            blackScreen.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(blackScreenAlpha * 255)));
            window.draw(blackScreen);
            
        } else if (gameState == GameState::FirstTimeSetup) {
            drawFirstTimeSetup(window, titleFont, menuFont, fontLoaded, hoveredControlScheme, textures, useTextures);
        } else if (gameState == GameState::WelcomeScreen) {
            drawWelcomeScreen(window, titleFont, menuFont, fontLoaded);
        } else if (gameState == GameState::MainMenu) {
            splashElapsedTime += deltaTime;
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawMainMenu(window, titleFont, menuFont, fontLoaded, selectedMenuOption, debugMode, textures, useTextures, splashElapsedTime);
        } else if (gameState == GameState::GameModeSelect) {
            splashElapsedTime += deltaTime;
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawGameModeMenu(window, titleFont, menuFont, fontLoaded, selectedGameModeOption, textures, useTextures, splashElapsedTime, debugMode);
        } else if (gameState == GameState::ClassicDifficultySelect) {
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawClassicDifficultyMenu(window, titleFont, menuFont, fontLoaded, selectedClassicDifficulty, saveData, textures, useTextures, debugMode);
            drawThemeSelector(window, menuFont, fontLoaded, selectedThemeChoice, hoveredThemeChoice, isThemeSelectorHovered);
            drawAbilitySelector(window, menuFont, fontLoaded, selectedAbilityChoice, hoveredAbilityChoice, isAbilitySelectorHovered);
        } else if (gameState == GameState::SprintLinesSelect) {
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawSprintLinesMenu(window, titleFont, menuFont, fontLoaded, selectedSprintLines, saveData, debugMode, textures, useTextures);
        } else if (gameState == GameState::ChallengeSelect) {
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawChallengeMenu(window, titleFont, menuFont, fontLoaded, selectedChallengeMode, debugMode, saveData, textures, useTextures);
        } else if (gameState == GameState::PracticeSelect) {
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawPracticeMenu(window, titleFont, menuFont, fontLoaded, selectedPracticeDifficulty, selectedPracticeLineGoal, practiceInfiniteBombs, selectedPracticeStartLevel, selectedPracticeOption, textures, useTextures, debugMode);
        } else if (gameState == GameState::Extras) {
            splashElapsedTime += deltaTime;
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawExtrasMenu(window, titleFont, menuFont, fontLoaded, selectedExtrasOption, textures, useTextures, splashElapsedTime, debugMode);
        } else if (gameState == GameState::AchievementsView) {
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawAchievementsScreen(window, titleFont, menuFont, fontLoaded, saveData, hoveredAchievement, debugMode);
        } else if (gameState == GameState::StatisticsView) {
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawStatisticsScreen(window, titleFont, menuFont, fontLoaded, saveData, debugMode);
        } else if (gameState == GameState::BestScoresView) {
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawBestScoresScreen(window, titleFont, menuFont, fontLoaded, saveData, debugMode);
        } else if (gameState == GameState::Options) {
            splashElapsedTime += deltaTime;
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawOptionsMenu(window, menuFont, fontLoaded, debugMode, selectedOptionsOption, textures, useTextures, splashElapsedTime);
        } else if (gameState == GameState::AudioSettings) {
            splashElapsedTime += deltaTime;
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawAudioMenu(window, menuFont, fontLoaded, debugMode, selectedAudioOption,
                         audioManager.getMasterVolume(), audioManager.getMusicVolume(), audioManager.getSfxVolume(),
                         textures, useTextures, splashElapsedTime);
        } else if (gameState == GameState::Rebinding) {
            drawBackgroundPiecesWithExplosions(window, backgroundPieces, explosionEffects, textures, useTextures);
            drawGlowEffects(window, glowEffects, textures);
            drawRebindingScreen(window, titleFont, menuFont, fontLoaded, keyBindings, rebindingSelectedScheme, rebindingHoveredScheme, rebindingAppliedScheme, selectedRebindingIndex, waitingForKeyPress, textures, useTextures, splashElapsedTime, debugMode);
        } else if (gameState == GameState::ConfirmClearScores) {

            drawOptionsMenu(window, menuFont, fontLoaded, debugMode, selectedOptionsOption, textures, useTextures, splashElapsedTime);

            drawConfirmClearScores(window, menuFont, fontLoaded, selectedConfirmOption);
        } else if (gameState == GameState::Playing || gameState == GameState::Paused) {

        drawGridBackground(window, currentTheme.backgroundColor);
        

        drawGameBackgroundPieces(window, gameBackgroundPieces, textures, useTextures, currentTheme.fallingPiecesColor);

        std::string modeText = getGameModeText(selectedGameModeOption, selectedClassicDifficulty, selectedSprintLines, selectedChallengeMode);

        bool isRaceMode = sprintModeActive || challengeModeActive;
        float bestTime = getBestTimeForMode(saveData, selectedGameModeOption, selectedSprintLines, selectedChallengeMode);
        int highScore = getHighScoreForMode(saveData, selectedClassicDifficulty);
        drawGameModeLabel(window, modeText, menuFont, fontLoaded, isRaceMode, sprintTimer, totalScore, bestTime, highScore);
        
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                float worldX = GRID_OFFSET_X + j * CELL_SIZE;
                float worldY = GRID_OFFSET_Y + i * CELL_SIZE;
                


                bool isVanished = grid[i][j].isVanishing && grid[i][j].vanishTimer >= 8.0f;
                
                if (grid[i][j].occupied && !isVanished) {

                    TextureType cellTextureType = grid[i][j].getTextureType();
                    sf::Color cellColor = grid[i][j].color;
                    

                    if (grid[i][j].isVanishing && grid[i][j].vanishTimer >= 4.0f && grid[i][j].vanishTimer < 8.0f) {

                        float fadeProgress = (grid[i][j].vanishTimer - 4.0f) / 4.0f;

                        cellColor.a = static_cast<std::uint8_t>(255.0f * (1.0f - fadeProgress));
                    }
                    
                    drawCell(window, worldX, worldY, CELL_SIZE, cellColor, cellTextureType, textures, useTextures);
                } else {

                    if (useTextures && textures.find(TextureType::Empty) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(TextureType::Empty));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        sf::Vector2u textureSize = textures.at(TextureType::Empty).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                        window.draw(cellSprite);
                    } else {

                        sf::RectangleShape cellShape;
                        cellShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                        cellShape.setPosition(sf::Vector2f(worldX, worldY));
                        cellShape.setFillColor(sf::Color(50, 50, 60));
                        window.draw(cellShape);
                    }
                }
            }
        }



        if (!gameOver) {
            bool useGravityFlipForGhost = (challengeModeActive && selectedChallengeMode == ChallengeMode::GravityFlip) ? gravityFlipped : false;
            activePiece.drawGhost(window, textures, useTextures, grid, useGravityFlipForGhost);
            

            if (hasBlocksInTopRows(grid, 5)) {
                const auto& nextQueue = TesseraBag.getNextQueue();
                if (!nextQueue.empty()) {
                    drawSpawnPreview(window, nextQueue[0], grid, textures, useTextures);
                }
            }
            
            activePiece.draw(window, textures, useTextures);
        }
        drawExplosionEffects(window, explosionEffects);
        drawGlowEffects(window, glowEffects, textures);
        drawFallingCells(window, fallingCells, textures, useTextures);
        drawNextPieces(window, TesseraBag.getNextQueue(), textures, useTextures, currentTheme.frameColor, menuFont, fontLoaded);
        drawHeldPiece(window, heldPiece, hasHeldPiece, textures, useTextures, menuFont, fontLoaded, currentTheme.frameColor);
        
        int currentAbilityLinesRequired = LINES_FOR_BOMB;
        if (selectedAbilityChoice == AbilityChoice::Delivery) {
            currentAbilityLinesRequired = LINES_FOR_DELIVERY;
        } else if (selectedAbilityChoice == AbilityChoice::Stomp) {
            currentAbilityLinesRequired = LINES_FOR_STOMP;
        } else if (selectedAbilityChoice == AbilityChoice::Bomb) {
            currentAbilityLinesRequired = LINES_FOR_BOMB;
        }
        
        drawBombAbility(window, bombAbilityAvailable, linesSinceLastAbility, textures, useTextures, menuFont, fontLoaded, practiceModeActive && practiceInfiniteBombs, currentTheme.frameColor, selectedAbilityChoice, currentAbilityLinesRequired);
        

        bool useSprintUI = sprintModeActive || (currentConfig && currentConfig->hasLineGoal);
        int targetLines = sprintModeActive ? sprintTargetLines : (currentConfig && currentConfig->hasLineGoal ? currentConfig->lineGoal : 0);
        drawLevelInfo(window, totalLinesCleared, currentLevel, totalScore, textures, useTextures, menuFont, fontLoaded, useSprintUI, sprintTimer, targetLines, sessionPlayTime, useSprintUI ? displayedThermometerFill : -1.0f, currentTheme.frameColor);
        if (useSprintUI) {
            drawThermometerParticles(window, thermometerParticles);
        }

        drawCombo(window, displayCombo, currentCombo, lastMoveScore, menuFont, fontLoaded, !useSprintUI, comboFadeScale);
        drawGridBorder(window, currentTheme.frameColor);
        drawTesseraTitle(window, titleFont, fontLoaded);
        

        if (gameOver && !gameOverScreenVisible) {
            gameOverDelayTimer += deltaTime;
            

            if (hasCollisionPiece && !gameOverBlocksFalling) {
                drawCollisionPiece(window, collisionPieceType, collisionPieceX, collisionPieceY, grid, textures, useTextures);
            }
            

            if (!gameOverPauseComplete && gameOverDelayTimer >= GAME_OVER_PAUSE_DELAY) {
                gameOverPauseComplete = true;
                gameOverBlocksFalling = true;
                gameOverDelayTimer = 0.0f;
                

                for (int i = 0; i < GRID_HEIGHT; ++i) {
                    for (int j = 0; j < GRID_WIDTH; ++j) {
                        if (grid[i][j].occupied) {
                            float cellX = GRID_OFFSET_X + j * CELL_SIZE;
                            float cellY = GRID_OFFSET_Y + i * CELL_SIZE;
                            fallingCells.emplace_back(cellX, cellY, grid[i][j].color, grid[i][j].textureType);
                        }
                        grid[i][j] = Cell();
                    }
                }
                

                if (hasCollisionPiece) {
                    PieceShape shape = getPieceShape(collisionPieceType);
                    int firstFilledRow = findFirstFilledRow(shape);
                    int spawnY = -firstFilledRow;
                    TextureType texType = getTextureType(collisionPieceType);
                    
                    for (int i = 0; i < shape.height; ++i) {
                        for (int j = 0; j < shape.width; ++j) {
                            if (shape.blocks[i][j]) {
                                int gridRow = spawnY + i;
                                int gridCol = collisionPieceX + j;
                                
                                if (gridRow >= 0 && gridCol >= 0 && gridCol < GRID_WIDTH) {
                                    float cellX = GRID_OFFSET_X + gridCol * CELL_SIZE;
                                    float cellY = GRID_OFFSET_Y + gridRow * CELL_SIZE;

                                    fallingCells.emplace_back(cellX, cellY, sf::Color(255, 50, 50), texType);
                                }
                            }
                        }
                    }
                    hasCollisionPiece = false;
                }
            }
            

            if (gameOverPauseComplete && gameOverDelayTimer >= GAME_OVER_FALL_DELAY) {
                if (!gameOverScreenVisible) {
                    gameOverScreenVisible = true;
                    gameOverUiFadeTimer = 0.0f;
                }
            }
        }
        
        if (gameOver && gameOverScreenVisible) {
            gameOverUiFadeTimer += deltaTime;
            float uiAlpha = (GAME_OVER_UI_FADE_DURATION > 0.0f)
                ? std::min(gameOverUiFadeTimer / GAME_OVER_UI_FADE_DURATION, 1.0f)
                : 1.0f;

            bool useLineGoalInterface = currentConfig && currentConfig->hasLineGoal;
            int lineTarget = useLineGoalInterface ? currentConfig->lineGoal : 0;
            
            drawGameOver(window, totalScore, totalLinesCleared, currentLevel, textures, useTextures, menuFont, fontLoaded, saveData, totalHardDropScore, totalLineScore, totalComboScore, selectedClassicDifficulty, useLineGoalInterface, sprintTimer, lineTarget, sprintCompleted, challengeModeActive, practiceModeActive, uiAlpha, currentTheme.frameColor, gameOverUiFadeTimer, gameOverNewHighScore, gameOverHighScoreBaseline, currentTheme.backgroundColor);
        }
        
        if (gameState == GameState::Paused) {
            drawPauseMenu(window, menuFont, fontLoaded, selectedPauseOption, currentTheme.frameColor, currentTheme.backgroundColor);
        }
        
        if (debugMode) {
            drawDebugMode(window, menuFont, fontLoaded);
        }
        


        
        }
        

        if (showVolumeIndicator) {
            drawVolumeIndicator(window, menuFont, fontLoaded, audioManager.getMasterVolume(), audioManager.isMutedStatus());
        }

        drawAchievementPopups(window, achievementPopups, menuFont, fontLoaded);
        

        if (showCustomCursor) {
            drawCustomCursor(window, textures, useTextures);
        }
        
        window.display();
    }
    return 0;
}





