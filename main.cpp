#include "types.h"
#include "menu_render.h"
#include "piece_utils.h"
#include "texture_utils.h"
#include "difficulty_config.h"
#include "achievements.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif

std::string getSaveFilePath() {
    
    std::filesystem::path saveDataPath;
    
#ifdef _WIN32
    
    char* appData = std::getenv("APPDATA");
    if (appData) {
        saveDataPath = std::filesystem::path(appData) / "Jigtriz";
    } else {
        saveDataPath = std::filesystem::current_path() / "Jigtriz_Saves";
    }
#else
    
    char* homePath = std::getenv("HOME");
    if (homePath) {
        saveDataPath = std::filesystem::path(homePath) / ".jigz";
    } else {
        saveDataPath = std::filesystem::current_path() / "Jigtriz_Saves";
    }
#endif
    
    
    std::filesystem::path gameFolder = saveDataPath;
    if (!std::filesystem::exists(gameFolder)) {
        std::filesystem::create_directories(gameFolder);
    }
    
    return (gameFolder / "save_data.txt").string();
}

void saveGameData(const SaveData& data) {
    std::string filePath = getSaveFilePath();
    std::ofstream file(filePath);
    
    if (file.is_open()) {
        file << "HIGH_SCORE=" << data.highScore << std::endl;
        file << "HIGH_SCORE_CLASSIC_EASY=" << data.highScoreClassicEasy << std::endl;
        file << "HIGH_SCORE_CLASSIC_MEDIUM=" << data.highScoreClassicMedium << std::endl;
        file << "HIGH_SCORE_CLASSIC_HARD=" << data.highScoreClassicHard << std::endl;
        file << "BEST_TIME_SPRINT_1=" << data.bestTimeSprint1 << std::endl;
        file << "BEST_TIME_SPRINT_24=" << data.bestTimeSprint24 << std::endl;
        file << "BEST_TIME_SPRINT_48=" << data.bestTimeSprint48 << std::endl;
        file << "BEST_TIME_SPRINT_96=" << data.bestTimeSprint96 << std::endl;
        file << "BEST_TIME_CHALLENGE_DEBUG=" << data.bestTimeChallengeDebug << std::endl;
        file << "BEST_TIME_CHALLENGE_THE_FOREST=" << data.bestTimeChallengeTheForest << std::endl;
        file << "BEST_TIME_CHALLENGE_RANDOMNESS=" << data.bestTimeChallengeRandomness << std::endl;
        file << "BEST_TIME_CHALLENGE_NON_STRAIGHT=" << data.bestTimeChallengeNonStraight << std::endl;
        file << "BEST_LINES=" << data.bestLines << std::endl;
        file << "BEST_LEVEL=" << data.bestLevel << std::endl;
        

        file << "STAT_TOTAL_LINES=" << data.totalLinesCleared << std::endl;
        file << "STAT_TOTAL_PIECES=" << data.totalPiecesPlaced << std::endl;
        file << "STAT_TOTAL_GAMES=" << data.totalGamesPlayed << std::endl;
        file << "STAT_TOTAL_SCORE=" << data.totalScore << std::endl;
        file << "STAT_MAX_COMBO=" << data.maxComboEver << std::endl;
        file << "STAT_TOTAL_BOMBS=" << data.totalBombsUsed << std::endl;
        file << "STAT_TOTAL_PLAYTIME=" << data.totalPlayTimeSeconds << std::endl;
        file << "STAT_TOTAL_ROTATIONS=" << data.totalRotations << std::endl;
        file << "STAT_TOTAL_HOLDS=" << data.totalHolds << std::endl;
        file << "STAT_TOTAL_PERFECT_CLEARS=" << data.totalPerfectClears << std::endl;
        
        file << "MASTER_VOLUME=" << data.masterVolume << std::endl;
        file << "IS_MUTED=" << (data.isMuted ? 1 : 0) << std::endl;
        

        for (int i = 0; i < 3; i++) {
            file << "TOP" << (i+1) << "_SCORE=" << data.topScores[i].score << std::endl;
            file << "TOP" << (i+1) << "_LINES=" << data.topScores[i].lines << std::endl;
            file << "TOP" << (i+1) << "_LEVEL=" << data.topScores[i].level << std::endl;
        }
        

        for (int i = 0; i < 3; i++) {
            file << "TOP_EASY_" << (i+1) << "_SCORE=" << data.topScoresEasy[i].score << std::endl;
            file << "TOP_EASY_" << (i+1) << "_LINES=" << data.topScoresEasy[i].lines << std::endl;
            file << "TOP_EASY_" << (i+1) << "_LEVEL=" << data.topScoresEasy[i].level << std::endl;
        }
        

        for (int i = 0; i < 3; i++) {
            file << "TOP_MEDIUM_" << (i+1) << "_SCORE=" << data.topScoresMedium[i].score << std::endl;
            file << "TOP_MEDIUM_" << (i+1) << "_LINES=" << data.topScoresMedium[i].lines << std::endl;
            file << "TOP_MEDIUM_" << (i+1) << "_LEVEL=" << data.topScoresMedium[i].level << std::endl;
        }
        

        for (int i = 0; i < 3; i++) {
            file << "TOP_HARD_" << (i+1) << "_SCORE=" << data.topScoresHard[i].score << std::endl;
            file << "TOP_HARD_" << (i+1) << "_LINES=" << data.topScoresHard[i].lines << std::endl;
            file << "TOP_HARD_" << (i+1) << "_LEVEL=" << data.topScoresHard[i].level << std::endl;
        }
        

        for (int i = 0; i < 6; i++) {
            file << "ACHIEVEMENT_" << i << "=" << (data.achievements[i] ? 1 : 0) << std::endl;
        }
        

        file << "KEY_MOVE_LEFT=" << data.moveLeft << std::endl;
        file << "KEY_MOVE_RIGHT=" << data.moveRight << std::endl;
        file << "KEY_ROTATE_LEFT=" << data.rotateLeft << std::endl;
        file << "KEY_ROTATE_RIGHT=" << data.rotateRight << std::endl;
        file << "KEY_QUICK_FALL=" << data.quickFall << std::endl;
        file << "KEY_DROP=" << data.drop << std::endl;
        file << "KEY_HOLD=" << data.hold << std::endl;
        file << "KEY_BOMB=" << data.bomb << std::endl;
        file << "KEY_MUTE=" << data.mute << std::endl;
        file << "KEY_VOLUME_DOWN=" << data.volumeDown << std::endl;
        file << "KEY_VOLUME_UP=" << data.volumeUp << std::endl;
        file << "KEY_MENU=" << data.menu << std::endl;
        
        file.close();
        std::cout << "Game data saved to: " << filePath << std::endl;
    } else {
        std::cout << "Failed to save game data to: " << filePath << std::endl;
    }
}

SaveData loadGameData() {
    SaveData data;
    std::string filePath = getSaveFilePath();
    std::ifstream file(filePath);
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                if (key == "HIGH_SCORE") {
                    data.highScore = std::stoi(value);
                } else if (key == "HIGH_SCORE_CLASSIC_EASY") {
                    data.highScoreClassicEasy = std::stoi(value);
                } else if (key == "HIGH_SCORE_CLASSIC_MEDIUM") {
                    data.highScoreClassicMedium = std::stoi(value);
                } else if (key == "HIGH_SCORE_CLASSIC_HARD") {
                    data.highScoreClassicHard = std::stoi(value);
                } else if (key == "BEST_TIME_SPRINT_1") {
                    data.bestTimeSprint1 = std::stof(value);
                } else if (key == "BEST_TIME_SPRINT_24") {
                    data.bestTimeSprint24 = std::stof(value);
                } else if (key == "BEST_TIME_SPRINT_48") {
                    data.bestTimeSprint48 = std::stof(value);
                } else if (key == "BEST_TIME_SPRINT_96") {
                    data.bestTimeSprint96 = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_DEBUG") {
                    data.bestTimeChallengeDebug = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_THE_FOREST") {
                    data.bestTimeChallengeTheForest = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_RANDOMNESS") {
                    data.bestTimeChallengeRandomness = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_NON_STRAIGHT") {
                    data.bestTimeChallengeNonStraight = std::stof(value);
                } else if (key == "BEST_LINES") {
                    data.bestLines = std::stoi(value);
                } else if (key == "BEST_LEVEL") {
                    data.bestLevel = std::stoi(value);
                } else if (key == "STAT_TOTAL_LINES") {
                    data.totalLinesCleared = std::stoi(value);
                } else if (key == "STAT_TOTAL_PIECES") {
                    data.totalPiecesPlaced = std::stoi(value);
                } else if (key == "STAT_TOTAL_GAMES") {
                    data.totalGamesPlayed = std::stoi(value);
                } else if (key == "STAT_TOTAL_SCORE") {
                    data.totalScore = std::stoi(value);
                } else if (key == "STAT_MAX_COMBO") {
                    data.maxComboEver = std::stoi(value);
                } else if (key == "STAT_TOTAL_BOMBS") {
                    data.totalBombsUsed = std::stoi(value);
                } else if (key == "STAT_TOTAL_PLAYTIME") {
                    data.totalPlayTimeSeconds = std::stof(value);
                } else if (key == "STAT_TOTAL_ROTATIONS") {
                    data.totalRotations = std::stoi(value);
                } else if (key == "STAT_TOTAL_HOLDS") {
                    data.totalHolds = std::stoi(value);
                } else if (key == "STAT_TOTAL_PERFECT_CLEARS") {
                    data.totalPerfectClears = std::stoi(value);
                } else if (key == "MASTER_VOLUME") {
                    data.masterVolume = std::stof(value);
                } else if (key == "IS_MUTED") {
                    data.isMuted = (std::stoi(value) == 1);
                } else if (key.rfind("ACHIEVEMENT_", 0) == 0) {

                    int achievementId = std::stoi(key.substr(12));
                    if (achievementId >= 0 && achievementId < 6) {
                        data.achievements[achievementId] = (std::stoi(value) == 1);
                    }
                } else if (key == "KEY_MOVE_LEFT") {
                    data.moveLeft = std::stoi(value);
                } else if (key == "KEY_MOVE_RIGHT") {
                    data.moveRight = std::stoi(value);
                } else if (key == "KEY_ROTATE_LEFT") {
                    data.rotateLeft = std::stoi(value);
                } else if (key == "KEY_ROTATE_RIGHT") {
                    data.rotateRight = std::stoi(value);
                } else if (key == "KEY_QUICK_FALL") {
                    data.quickFall = std::stoi(value);
                } else if (key == "KEY_DROP") {
                    data.drop = std::stoi(value);
                } else if (key == "KEY_HOLD") {
                    data.hold = std::stoi(value);
                } else if (key == "KEY_BOMB") {
                    data.bomb = std::stoi(value);
                } else if (key == "KEY_MUTE") {
                    data.mute = std::stoi(value);
                } else if (key == "KEY_VOLUME_DOWN") {
                    data.volumeDown = std::stoi(value);
                } else if (key == "KEY_VOLUME_UP") {
                    data.volumeUp = std::stoi(value);
                } else if (key == "KEY_MENU") {
                    data.menu = std::stoi(value);
                } else if (key.find("TOP") == 0) {

                    if (key.find("TOP_EASY_") == 0) {
                        char topNum = key[9];
                        int index = topNum - '1';
                        if (index >= 0 && index < 3) {
                            if (key.find("_SCORE") != std::string::npos) {
                                data.topScoresEasy[index].score = std::stoi(value);
                            } else if (key.find("_LINES") != std::string::npos) {
                                data.topScoresEasy[index].lines = std::stoi(value);
                            } else if (key.find("_LEVEL") != std::string::npos) {
                                data.topScoresEasy[index].level = std::stoi(value);
                            }
                        }
                    } else if (key.find("TOP_MEDIUM_") == 0) {
                        char topNum = key[11];
                        int index = topNum - '1';
                        if (index >= 0 && index < 3) {
                            if (key.find("_SCORE") != std::string::npos) {
                                data.topScoresMedium[index].score = std::stoi(value);
                            } else if (key.find("_LINES") != std::string::npos) {
                                data.topScoresMedium[index].lines = std::stoi(value);
                            } else if (key.find("_LEVEL") != std::string::npos) {
                                data.topScoresMedium[index].level = std::stoi(value);
                            }
                        }
                    } else if (key.find("TOP_HARD_") == 0) {
                        char topNum = key[9];
                        int index = topNum - '1';
                        if (index >= 0 && index < 3) {
                            if (key.find("_SCORE") != std::string::npos) {
                                data.topScoresHard[index].score = std::stoi(value);
                            } else if (key.find("_LINES") != std::string::npos) {
                                data.topScoresHard[index].lines = std::stoi(value);
                            } else if (key.find("_LEVEL") != std::string::npos) {
                                data.topScoresHard[index].level = std::stoi(value);
                            }
                        }
                    } else {

                        char topNum = key[3];
                        int index = topNum - '1';
                        if (index >= 0 && index < 3) {
                            if (key.find("_SCORE") != std::string::npos) {
                                data.topScores[index].score = std::stoi(value);
                            } else if (key.find("_LINES") != std::string::npos) {
                                data.topScores[index].lines = std::stoi(value);
                            } else if (key.find("_LEVEL") != std::string::npos) {
                                data.topScores[index].level = std::stoi(value);
                            }
                        }
                    }
                }
            }
        }
        file.close();
        std::cout << "Game data loaded from: " << filePath << std::endl;
        std::cout << "High Score: " << data.highScore << " | Best Lines: " << data.bestLines << " | Best Level: " << data.bestLevel << std::endl;
    } else {
        std::cout << "No save file found, using default values" << std::endl;
    }
    
    return data;
}


void unlockAchievement(SaveData& saveData, Achievement ach) {
    if (tryUnlockAchievement(saveData, ach)) {
        saveGameData(saveData);
    }
}

void drawGridBackground(sf::RenderWindow& window) {
}

int calculateLevel(int linesCleared) {
    for (int level = MAX_LEVEL; level >= 1; level--) {
        if (linesCleared >= LEVEL_THRESHOLDS[level]) {
            return level;
        }
    }
    return 0;
}


int findFirstFilledRow(const PieceShape& shape) {
    for (int y = 0; y < shape.height; y++) {
        for (int x = 0; x < shape.width; x++) {
            if (shape.blocks[y][x]) {
                std::cout << "[DEBUG] First filled row found at y=" << y << " (shape height=" << shape.height << ")" << std::endl;
                return y;
            }
        }
    }
    std::cout << "[WARNING] No filled cells found in shape!" << std::endl;
    return 0;
}


void drawGridBorder(sf::RenderWindow& window) {
    sf::RectangleShape border;
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(100, 150, 255, 255));
    border.setOutlineThickness(BORDER_WIDTH);
    float borderX = GRID_OFFSET_X - BORDER_WIDTH;
    float borderY = GRID_OFFSET_Y - BORDER_WIDTH;
    float borderW = GRID_WIDTH * CELL_SIZE + (2 * BORDER_WIDTH);
    float borderH = GRID_HEIGHT * CELL_SIZE + (2 * BORDER_WIDTH);
    border.setPosition(sf::Vector2f(borderX, borderY));
    border.setSize(sf::Vector2f(borderW, borderH));
    window.draw(border);
}

AbilityType getAbilityType(PieceType type) {
    switch(type) {
        case PieceType::A_Bomb: return AbilityType::Bomb;
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
    currentLevel = 0;
    bagIndex = 0;
    nextBagReady = false;
    refillMediumBag();
    refillHardBag();
    currentBag = createNewBag(currentLevel);
    nextBag.clear();
    nextQueue.clear();
    fillNextQueue();
    std::cout << "Bag system reset to level 0!" << std::endl;
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

void drawBombAbility(sf::RenderWindow& window, bool isAvailable, int linesSinceLastAbility, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y + 300;

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(30, 30, 40, 200));
    if (isAvailable) {
        panelBg.setOutlineColor(sf::Color(255, 100, 100, 255));
    } else {
        panelBg.setOutlineColor(sf::Color(100, 100, 100, 255));
    }
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(120, 180));
    window.draw(panelBg);
    
    if (fontLoaded) {
        sf::Text titleText(font, "BOMB");
        titleText.setCharacterSize(24);
        titleText.setFillColor(isAvailable ? sf::Color(255, 100, 100) : sf::Color(100, 100, 100));
        titleText.setPosition(sf::Vector2f(panelX + 25, panelY));
        window.draw(titleText);
    } else {
        sf::RectangleShape titleLabel;
        titleLabel.setFillColor(isAvailable ? sf::Color(255, 100, 100) : sf::Color(100, 100, 100));
        titleLabel.setSize(sf::Vector2f(60, 8));
        titleLabel.setPosition(sf::Vector2f(panelX + 20, panelY));
        window.draw(titleLabel);
    }
    
    if (isAvailable) {
        PieceShape bombShape = getPieceShape(PieceType::A_Bomb);
        TextureType texType = TextureType::A_Bomb;
        
        float centerX = panelX + 50;
        float centerY = panelY + 70;
        float miniCellSize = 64.0f;
        
        if (useTextures && textures.find(texType) != textures.end()) {
            sf::Sprite miniSprite(textures.at(texType));
            miniSprite.setPosition(sf::Vector2f(centerX - miniCellSize/2, centerY - miniCellSize/2));
            sf::Vector2u textureSize = textures.at(texType).getSize();
            miniSprite.setScale(sf::Vector2f(miniCellSize / textureSize.x, miniCellSize / textureSize.y));
            window.draw(miniSprite);
        } else {
            sf::RectangleShape miniCell;
            miniCell.setSize(sf::Vector2f(miniCellSize, miniCellSize));
            miniCell.setPosition(sf::Vector2f(centerX - miniCellSize/2, centerY - miniCellSize/2));
            miniCell.setFillColor(bombShape.color);
            window.draw(miniCell);
        }
        
        if (fontLoaded) {
            sf::Text readyText(font, "READY!");
            readyText.setCharacterSize(25);
            readyText.setFillColor(sf::Color(100, 255, 100));
            readyText.setPosition(sf::Vector2f(panelX + 22, panelY + 100));
            window.draw(readyText);
            
            sf::Text keyText(font, "Press 'I'");
            keyText.setCharacterSize(16);
            keyText.setFillColor(sf::Color(200, 200, 200));
            keyText.setPosition(sf::Vector2f(panelX + 22, panelY + 135));
            window.draw(keyText);
        }
    } else {
        int linesNeeded = 10 - linesSinceLastAbility;
        
        if (fontLoaded) {
            sf::Text progressText(font, std::to_string(linesSinceLastAbility) + " / 10");
            progressText.setCharacterSize(25);
            progressText.setFillColor(sf::Color(200, 200, 200));
            progressText.setPosition(sf::Vector2f(panelX + 22, panelY + 100));
            window.draw(progressText);
            
            sf::Text needText(font, std::to_string(linesNeeded) + " more");
            needText.setCharacterSize(16);
            needText.setFillColor(sf::Color(150, 150, 150));
            needText.setPosition(sf::Vector2f(panelX + 22, panelY + 135));
            window.draw(needText);
        } else {
            sf::RectangleShape progressBar;
            float progress = linesSinceLastAbility / 10.0f;
            progressBar.setFillColor(sf::Color(100, 150, 200));
            progressBar.setSize(sf::Vector2f(80 * progress, 10));
            progressBar.setPosition(sf::Vector2f(panelX + 10, panelY + 40));
            window.draw(progressBar);
        }
    }
}

void drawHeldPiece(sf::RenderWindow& window, PieceType heldType, bool hasHeld, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y + 200;

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(30, 30, 40, 200));
    panelBg.setOutlineColor(sf::Color(255, 150, 100, 255));
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(120, 80));
    window.draw(panelBg);
    
    if (fontLoaded) {
        sf::Text holdLabel(font, "HOLD");
        holdLabel.setCharacterSize(14);
        holdLabel.setFillColor(sf::Color(255, 150, 100));
        holdLabel.setStyle(sf::Text::Bold);
        holdLabel.setPosition(sf::Vector2f(panelX + 35, panelY + 5));
        window.draw(holdLabel);
    } else {
        sf::RectangleShape holdLabel;
        holdLabel.setFillColor(sf::Color(255, 150, 100));
        holdLabel.setSize(sf::Vector2f(80, 8));
        holdLabel.setPosition(sf::Vector2f(panelX + 10, panelY + 5));
        window.draw(holdLabel);
    }
    
    if (hasHeld) {
        PieceShape shape = getPieceShape(heldType);
        TextureType texType = getTextureType(heldType);
        
        float centerX = panelX + 50;
        float centerY = panelY + 40;
        float miniCellSize = 16.0f;
        
        for (int row = 0; row < shape.height; ++row) {
            for (int col = 0; col < shape.width; ++col) {
                if (shape.blocks[row][col]) {
                    float miniX = centerX - (shape.width * miniCellSize) / 2 + col * miniCellSize;
                    float miniY = centerY - (shape.height * miniCellSize) / 2 + row * miniCellSize;
                    
                    if (useTextures && textures.find(TextureType::GenericBlock) != textures.end()) {
                        sf::Sprite miniSprite(textures.at(TextureType::GenericBlock));
                        miniSprite.setPosition(sf::Vector2f(miniX, miniY));
                        miniSprite.setColor(shape.color);
                        sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
                        miniSprite.setScale(sf::Vector2f(miniCellSize / textureSize.x, miniCellSize / textureSize.y));
                        window.draw(miniSprite);
                    } else if (useTextures && textures.find(texType) != textures.end()) {
                        sf::Sprite miniSprite(textures.at(texType));
                        miniSprite.setPosition(sf::Vector2f(miniX, miniY));
                        sf::Vector2u textureSize = textures.at(texType).getSize();
                        miniSprite.setScale(sf::Vector2f(miniCellSize / textureSize.x, miniCellSize / textureSize.y));
                        window.draw(miniSprite);
                    } else {
                        sf::RectangleShape miniCell;
                        miniCell.setSize(sf::Vector2f(miniCellSize, miniCellSize));
                        miniCell.setPosition(sf::Vector2f(miniX, miniY));
                        miniCell.setFillColor(shape.color);
                        window.draw(miniCell);
                    }
                }
            }
        }
    } else {
        sf::RectangleShape emptyIndicator;
        emptyIndicator.setFillColor(sf::Color(100, 100, 100, 150));
        emptyIndicator.setSize(sf::Vector2f(60, 30));
        emptyIndicator.setPosition(sf::Vector2f(panelX + 20, panelY + 25));
        window.draw(emptyIndicator);
    }
}

void drawCombo(sf::RenderWindow& window, int currentCombo, int lastMoveScore, const sf::Font& font, bool fontLoaded) {
    float panelX = 250;
    float panelY = GRID_OFFSET_Y + 200;
    
    sf::RectangleShape comboBg;
    comboBg.setFillColor(sf::Color(40, 20, 60, 220));
    comboBg.setOutlineColor(currentCombo > 0 ? sf::Color(255, 100, 255, 255) : sf::Color(120, 60, 140, 200));
    comboBg.setOutlineThickness(3);
    comboBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    comboBg.setSize(sf::Vector2f(140, 90));
    window.draw(comboBg);
    
    if (fontLoaded) {
        sf::Text comboLabel(font, "COMBO");
        comboLabel.setCharacterSize(20);
        comboLabel.setFillColor(sf::Color(255, 200, 255));
        comboLabel.setStyle(sf::Text::Bold);
        comboLabel.setPosition(sf::Vector2f(panelX + 20, panelY));
        window.draw(comboLabel);
        
        sf::Text comboValue(font, "x" + std::to_string(currentCombo));
        comboValue.setCharacterSize(32);
        comboValue.setFillColor(currentCombo > 0 ? sf::Color(255, 100, 255) : sf::Color(150, 100, 150));
        comboValue.setStyle(sf::Text::Bold);
        comboValue.setPosition(sf::Vector2f(panelX + 25, panelY + 35));
        window.draw(comboValue);
    }
    
    float scorePanelY = panelY + 105;
    
    sf::RectangleShape scoreBg;
    scoreBg.setFillColor(sf::Color(20, 40, 60, 220));
    scoreBg.setOutlineColor(sf::Color(100, 200, 255, 255));
    scoreBg.setOutlineThickness(3);
    scoreBg.setPosition(sf::Vector2f(panelX - 10, scorePanelY - 10));
    scoreBg.setSize(sf::Vector2f(140, 75));
    window.draw(scoreBg);
    
    if (fontLoaded) {
        sf::Text lastScoreLabel(font, "LAST MOVE");
        lastScoreLabel.setCharacterSize(14);
        lastScoreLabel.setFillColor(sf::Color(150, 220, 255));
        lastScoreLabel.setStyle(sf::Text::Bold);
        lastScoreLabel.setPosition(sf::Vector2f(panelX + 10, scorePanelY));
        window.draw(lastScoreLabel);
        
        sf::Text lastScoreValue(font, "+" + std::to_string(lastMoveScore));
        lastScoreValue.setCharacterSize(26);
        lastScoreValue.setFillColor(lastMoveScore > 0 ? sf::Color(100, 255, 150) : sf::Color(150, 150, 150));
        lastScoreValue.setStyle(sf::Text::Bold);
        lastScoreValue.setPosition(sf::Vector2f(panelX + 20, scorePanelY + 30));
        window.draw(lastScoreValue);
    }
}

void drawLevelInfo(sf::RenderWindow& window, int totalLinesCleared, int currentLevel, int totalScore, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, bool sprintMode = false, float sprintTime = 0.0f, int sprintTarget = 0) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y + 50;
    
    if (sprintMode) {


        sf::RectangleShape timerBg;
        timerBg.setFillColor(sf::Color(30, 30, 40, 200));
        timerBg.setOutlineColor(sf::Color(255, 200, 100, 255));
        timerBg.setOutlineThickness(2);
        timerBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
        timerBg.setSize(sf::Vector2f(160, 90));
        window.draw(timerBg);
        
        if (fontLoaded) {
            sf::Text timerLabel(font, "TIME");
            timerLabel.setCharacterSize(14);
            timerLabel.setFillColor(sf::Color::Yellow);
            timerLabel.setStyle(sf::Text::Bold);
            timerLabel.setPosition(sf::Vector2f(panelX + 5, panelY));
            window.draw(timerLabel);
            

            int minutes = static_cast<int>(sprintTime) / 60;
            int seconds = static_cast<int>(sprintTime) % 60;
            int milliseconds = static_cast<int>((sprintTime - static_cast<int>(sprintTime)) * 100);
            std::string timeStr = std::to_string(minutes) + ":" + 
                                 (seconds < 10 ? "0" : "") + std::to_string(seconds) + "." +
                                 (milliseconds < 10 ? "0" : "") + std::to_string(milliseconds);
            
            sf::Text timeValue(font, timeStr);
            timeValue.setCharacterSize(24);
            timeValue.setFillColor(sf::Color::White);
            timeValue.setPosition(sf::Vector2f(panelX + 10, panelY + 25));
            window.draw(timeValue);
            

            sf::Text linesLabel(font, "LINES");
            linesLabel.setCharacterSize(14);
            linesLabel.setFillColor(sf::Color::Green);
            linesLabel.setStyle(sf::Text::Bold);
            linesLabel.setPosition(sf::Vector2f(panelX + 5, panelY + 60));
            window.draw(linesLabel);
            
            sf::Text linesValue(font, std::to_string(totalLinesCleared) + " / " + std::to_string(sprintTarget));
            linesValue.setCharacterSize(18);
            linesValue.setFillColor(sf::Color::White);
            linesValue.setPosition(sf::Vector2f(panelX + 60, panelY + 58));
            window.draw(linesValue);
        }
        

        float barX = GRID_OFFSET_X - GRID_WIDTH * CELL_SIZE + 200;
        float barY = panelY - 75;
        float barWidth = 75;
        float barHeight = 750;
        

        sf::RectangleShape progressBg;
        progressBg.setFillColor(sf::Color(20, 20, 30, 200));
        progressBg.setOutlineColor(sf::Color(100, 150, 255, 255));
        progressBg.setOutlineThickness(2);
        progressBg.setPosition(sf::Vector2f(barX, barY));
        progressBg.setSize(sf::Vector2f(barWidth, barHeight));
        window.draw(progressBg);
        

        float progressPercent = std::min(1.0f, static_cast<float>(totalLinesCleared) / static_cast<float>(sprintTarget));
        float fillHeight = barHeight * progressPercent;
        
        sf::RectangleShape progressFill;
        sf::Color fillColor;
        if (progressPercent < 0.33f) fillColor = sf::Color(255, 100, 100);
        else if (progressPercent < 0.66f) fillColor = sf::Color(255, 200, 100);
        else fillColor = sf::Color(100, 255, 100);
        
        progressFill.setFillColor(fillColor);
        progressFill.setPosition(sf::Vector2f(barX + 2, barY + barHeight - fillHeight - 2));
        progressFill.setSize(sf::Vector2f(barWidth - 4, fillHeight));
        window.draw(progressFill);
        

        if (fontLoaded) {
            sf::Text percentText(font, std::to_string(static_cast<int>(progressPercent * 100)) + "%");
            percentText.setCharacterSize(20);
            percentText.setFillColor(sf::Color::White);
            percentText.setStyle(sf::Text::Bold);
            sf::FloatRect percentBounds = percentText.getLocalBounds();
            percentText.setPosition(sf::Vector2f(barX + barWidth/2 - percentBounds.size.x/2, barY + barHeight/2 - 15));
            window.draw(percentText);
        }
    } else {

        sf::RectangleShape bg;
        bg.setFillColor(sf::Color(30, 30, 40, 200));
        bg.setOutlineColor(sf::Color(255, 200, 100, 255));
        bg.setOutlineThickness(2);
        bg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
        bg.setSize(sf::Vector2f(140, 130));
        window.draw(bg);
        
        if (fontLoaded) {
            sf::Text scoreLabel(font, "SCORE");
            scoreLabel.setCharacterSize(14);
            scoreLabel.setFillColor(sf::Color::Yellow);
            scoreLabel.setStyle(sf::Text::Bold);
            scoreLabel.setPosition(sf::Vector2f(panelX + 5, panelY));
            window.draw(scoreLabel);
            
            sf::Text scoreValue(font, std::to_string(totalScore));
            scoreValue.setCharacterSize(18);
            scoreValue.setFillColor(sf::Color::White);
            scoreValue.setPosition(sf::Vector2f(panelX + 45, panelY));
            window.draw(scoreValue);
            
            sf::Text linesLabel(font, "LINES");
            linesLabel.setCharacterSize(14);
            linesLabel.setFillColor(sf::Color::Green);
            linesLabel.setStyle(sf::Text::Bold);
            linesLabel.setPosition(sf::Vector2f(panelX + 5, panelY + 40));
            window.draw(linesLabel);
            
            sf::Text linesValue(font, std::to_string(totalLinesCleared));
            linesValue.setCharacterSize(18);
            linesValue.setFillColor(sf::Color::White);
            linesValue.setPosition(sf::Vector2f(panelX + 45, panelY + 40));
            window.draw(linesValue);
            
            sf::Text levelLabel(font, "LEVEL");
            levelLabel.setCharacterSize(14);
            levelLabel.setFillColor(sf::Color::Cyan);
            levelLabel.setStyle(sf::Text::Bold);
            levelLabel.setPosition(sf::Vector2f(panelX + 5, panelY + 80));
            window.draw(levelLabel);
            
            sf::Text levelValue(font, std::to_string(currentLevel));
            levelValue.setCharacterSize(18);
            levelValue.setFillColor(sf::Color::White);
            levelValue.setPosition(sf::Vector2f(panelX + 45, panelY + 80));
            window.draw(levelValue);
        }
    }
}

void drawNextPieces(sf::RenderWindow& window, const std::vector<PieceType>& nextQueue, const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    float panelX = GRID_OFFSET_X + GRID_WIDTH * CELL_SIZE + 50;
    float panelY = GRID_OFFSET_Y + 50;
    

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(30, 30, 40, 200));
    panelBg.setOutlineColor(sf::Color(100, 150, 255, 255));
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(120, 200));
    window.draw(panelBg);
    

    sf::Font font;

    
    for (int i = 0; i < nextQueue.size(); ++i) {
        PieceShape shape = getPieceShape(nextQueue[i]);
        TextureType texType = getTextureType(nextQueue[i]);
        
        float startY = panelY + i * 60;
        float centerX = panelX + 50;
        float centerY = startY + 25;
        

        for (int row = 0; row < shape.height; ++row) {
            for (int col = 0; col < shape.width; ++col) {
                if (shape.blocks[row][col]) {
                    float miniCellSize = 16.0f;
                    float miniX = centerX - (shape.width * miniCellSize) / 2 + col * miniCellSize;
                    float miniY = centerY - (shape.height * miniCellSize) / 2 + row * miniCellSize;
                    
                    if (useTextures && textures.find(TextureType::GenericBlock) != textures.end()) {
                        sf::Sprite miniSprite(textures.at(TextureType::GenericBlock));
                        miniSprite.setPosition(sf::Vector2f(miniX, miniY));
                        miniSprite.setColor(shape.color);
                        sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
                        miniSprite.setScale(sf::Vector2f(miniCellSize / textureSize.x, miniCellSize / textureSize.y));
                        window.draw(miniSprite);
                    } else if (useTextures && textures.find(texType) != textures.end()) {
                        sf::Sprite miniSprite(textures.at(texType));
                        miniSprite.setPosition(sf::Vector2f(miniX, miniY));
                        sf::Vector2u textureSize = textures.at(texType).getSize();
                        miniSprite.setScale(sf::Vector2f(miniCellSize / textureSize.x, miniCellSize / textureSize.y));
                        window.draw(miniSprite);
                    } else {
                        sf::RectangleShape miniCell;
                        miniCell.setSize(sf::Vector2f(miniCellSize, miniCellSize));
                        miniCell.setPosition(sf::Vector2f(miniX, miniY));
                        miniCell.setFillColor(shape.color);
                        miniCell.setOutlineColor(sf::Color::Black);
                        miniCell.setOutlineThickness(1);
                        window.draw(miniCell);
                    }
                }
            }
        }
    }
}

class Piece {
private:
    PieceShape shape;
    PieceType type;
    int x, y;
    bool isStatic = false;
    float fallTimer = 0.0f;
    bool touchingGround = false;
    float lockDelayTimer = 0.0f;
    static constexpr float LOCK_DELAY_TIME = 2.0f;
    AbilityType ability;
    void updatePosition() {
        float worldX = GRID_OFFSET_X + x * CELL_SIZE;
        float worldY = GRID_OFFSET_Y + y * CELL_SIZE;
    }
public:
    Piece(int x, int y, PieceType pieceType, bool isStatic = false)
    : isStatic(isStatic), x(x), y(y), type(pieceType), touchingGround(false), lockDelayTimer(0.0f)
    {
        shape = getPieceShape(pieceType);
        ability = getAbilityType(pieceType);
    }
    PieceType getType() const { return type; }
    AbilityType getAbility() const { return ability; }
    int getX() const { return x; }
    int getY() const { return y; }
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
    void update(float deltaTime, bool fastFall, std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) {
        if (isStatic) return;

        fallTimer += deltaTime;
        bool isGrounded = collidesAt(grid, x, y + 1);

        if (isGrounded) {
            if (!touchingGround) {
                touchingGround = true;
                lockDelayTimer = 0.0f;
            }
            
            lockDelayTimer += deltaTime;

            if (lockDelayTimer >= LOCK_DELAY_TIME) {
                isStatic = true;
                ChangeToStatic(grid, ability);
            }
        } else {
            touchingGround = false;
            lockDelayTimer = 0.0f;

            if (fallTimer >= (fastFall ? 0.03f : 0.5f)) {
                y++;
                fallTimer = 0.0f;
                updatePosition();
            }
        }
    }
    void ChangeToStatic(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, AbilityType ability = AbilityType::None, std::unique_ptr<sf::Sound>* bombSound = nullptr, std::vector<ExplosionEffect>* explosions = nullptr, std::vector<GlowEffect>* glowEffects = nullptr, float* shakeIntensity = nullptr, float* shakeDuration = nullptr, float* shakeTimer = nullptr) {
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    int gx = x + j;
                    int gy = y + i;
                    if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT) {
                        grid[gy][gx] = Cell(shape.color, getTextureType(type));
                    }
                }
            }
        }
        
        if (ability != AbilityType::None) {
            AbilityEffect(grid, bombSound, explosions, glowEffects, shakeIntensity, shakeDuration, shakeTimer);
        }
    }
    void BombEffect(int centerX, int centerY, std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, std::unique_ptr<sf::Sound>& bombSound, std::vector<ExplosionEffect>& explosions, std::vector<GlowEffect>& glowEffects, float& shakeIntensity, float& shakeDuration, float& shakeTimer) {
        if (bombSound) {
            std::cout << "Playing bomb sound! Volume: " << bombSound->getVolume() << std::endl;
            bombSound->play();
            std::cout << "Bomb sound status: " << static_cast<int>(bombSound->getStatus()) << std::endl;
        } else {
            std::cout << "ERROR: bombSound is null!" << std::endl;
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
                    explosions.push_back(ExplosionEffect(x, y));
                    
                    float baseWorldX = GRID_OFFSET_X + x * CELL_SIZE;
                    float baseWorldY = GRID_OFFSET_Y + y * CELL_SIZE;
                    float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                    float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                    
                    glowEffects.push_back(GlowEffect(baseWorldX + offsetX, baseWorldY + offsetY, blockColor));
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
    void AbilityEffect(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, std::unique_ptr<sf::Sound>* bombSound = nullptr, std::vector<ExplosionEffect>* explosions = nullptr, std::vector<GlowEffect>* glowEffects = nullptr, float* shakeIntensity = nullptr, float* shakeDuration = nullptr, float* shakeTimer = nullptr)
    {
        switch(ability)
        {
            case AbilityType::Bomb:
                if (bombSound && explosions && glowEffects && shakeIntensity && shakeDuration && shakeTimer) {
                    BombEffect(x, y, grid, *bombSound, *explosions, *glowEffects, *shakeIntensity, *shakeDuration, *shakeTimer);
                } else {
                    std::cout << "Bomb ability activated (no sound/explosions)!" << std::endl;
                }
                std::cout << "Bomb ability activated!" << std::endl;
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
        }
        
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float worldX = GRID_OFFSET_X + (x + j) * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + (y + i) * CELL_SIZE;
                    
                    TextureType pieceTexType = getTextureType(type);
                    

                    if (useTextures && pieceTexType != TextureType::GenericBlock && 
                        textures.find(pieceTexType) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(pieceTexType));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        sf::Vector2u textureSize = textures.at(pieceTexType).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                        window.draw(cellSprite);
                    }

                    else if (useTextures && textures.find(TextureType::GenericBlock) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(TextureType::GenericBlock));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        cellSprite.setColor(shape.color);
                        sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                        window.draw(cellSprite);
                    }

                    else {
                        sf::RectangleShape cellShape;
                        cellShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                        cellShape.setPosition(sf::Vector2f(worldX, worldY));
                        cellShape.setFillColor(shape.color);
                        window.draw(cellShape);
                    }
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
                if (touchingGround) lockDelayTimer = 0.0f;
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
                if (touchingGround) lockDelayTimer = 0.0f;
                return;
            }
            shape = originalShape;
        }
    }
    
    void moveLeft(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) { 
        if (!isStatic && !collidesAt(grid, x - 1, y)) {
            --x;
            if (touchingGround) lockDelayTimer = 0.0f;
        }
    }
    void moveRight(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) { 
        if (!isStatic && !collidesAt(grid, x + 1, y)) {
            ++x;
            if (touchingGround) lockDelayTimer = 0.0f;
        }
    }
    int moveGround(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) {
        if (isStatic) return 0;
        int startY = y;
        while (!collidesAt(grid, x, y + 1)) {
            ++y;
        }
        isStatic = true;
        ChangeToStatic(grid, ability);
        return y - startY;
    }
    

    int getGhostY(const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) const {
        if (isStatic) return y;
        int ghostY = y;
        while (!collidesAt(grid, x, ghostY + 1)) {
            ++ghostY;
        }
        return ghostY;
    }
    

    void drawGhost(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) const {
        if (isStatic) return;
        

        if (ability == AbilityType::Bomb) return;
        
        int ghostY = getGhostY(grid);
        if (ghostY == y) return;
        
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float worldX = GRID_OFFSET_X + (x + j) * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + (ghostY + i) * CELL_SIZE;
                    
                    TextureType pieceTexType = getTextureType(type);
                    

                    if (useTextures && pieceTexType != TextureType::GenericBlock && 
                        textures.find(pieceTexType) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(pieceTexType));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        sf::Vector2u textureSize = textures.at(pieceTexType).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                        cellSprite.setColor(sf::Color(255, 255, 255, 80));
                        window.draw(cellSprite);
                    }

                    else if (useTextures && textures.find(TextureType::GenericBlock) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(TextureType::GenericBlock));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));

                        sf::Color ghostColor = shape.color;
                        ghostColor.a = 80;
                        cellSprite.setColor(ghostColor);
                        window.draw(cellSprite);
                    }

                    else {
                        sf::RectangleShape cellShape;
                        cellShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                        cellShape.setPosition(sf::Vector2f(worldX, worldY));
                        sf::Color ghostColor = shape.color;
                        ghostColor.a = 80;
                        cellShape.setFillColor(ghostColor);
                        cellShape.setOutlineThickness(1);
                        cellShape.setOutlineColor(sf::Color(255, 255, 255, 120));
                        window.draw(cellShape);
                    }
                }
            }
        }
    }
};



int calculateScore(int linesCleared) {
    if (linesCleared == 0) return 0;
    int baseScore = 1000;
    int bonus = (linesCleared - 1) * 250;
    int scorePerLine = baseScore + bonus;
    
    return linesCleared * scorePerLine;
}

int clearFullLines(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, std::unique_ptr<sf::Sound>& laserSound, std::vector<std::unique_ptr<sf::Sound>>& wowSounds, std::vector<GlowEffect>* glowEffects = nullptr, float* shakeIntensity = nullptr, float* shakeDuration = nullptr, float* shakeTimer = nullptr) {
    int linesCleared = 0;
    

    std::vector<std::pair<int, int>> fullLineRows;
    for (int row = GRID_HEIGHT - 1; row >= 0; row--) {
        bool isFullLine = true;
        for (int col = 0; col < GRID_WIDTH; col++) {
            if (!grid[row][col].occupied) {
                isFullLine = false;
                break;
            }
        }
        if (isFullLine) {
            fullLineRows.push_back({row, row});
            

            if (glowEffects) {
                for (int col = 0; col < GRID_WIDTH; col++) {
                    sf::Color blockColor = grid[row][col].color;
                    float baseWorldX = GRID_OFFSET_X + col * CELL_SIZE;
                    float baseWorldY = GRID_OFFSET_Y + row * CELL_SIZE;
                    float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                    float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                    
                    glowEffects->push_back(GlowEffect(baseWorldX + offsetX, baseWorldY + offsetY, blockColor));
                }
            }
        }
    }
    

    for (int row = GRID_HEIGHT - 1; row >= 0; row--) {
        bool isFullLine = true;
        for (int col = 0; col < GRID_WIDTH; col++) {
            if (!grid[row][col].occupied) {
                isFullLine = false;
                break;
            }
        }
        if (isFullLine) {
            for (int moveRow = row; moveRow > 0; moveRow--) {
                for (int col = 0; col < GRID_WIDTH; col++) {
                    grid[moveRow][col] = grid[moveRow - 1][col];
                }
            }
            for (int col = 0; col < GRID_WIDTH; col++) {
                grid[0][col] = Cell();
            }
            linesCleared++;
            row++;
        }
    }
    
    if (linesCleared > 0 && laserSound) { 
        laserSound->play(); 
    }
    

    if (linesCleared > 0 && shakeIntensity && shakeDuration && shakeTimer) {
        float intensity = 5.0f + (linesCleared * 2.5f);
        *shakeIntensity = intensity;
        *shakeDuration = 0.3f;
        *shakeTimer = 0.0f;
        std::cout << "Line clear shake! Lines: " << linesCleared << ", Intensity: " << intensity << std::endl;
    }
    
    if (linesCleared >= 4) {
        std::vector<int> availableWowSounds;
        for (int i = 0; i < wowSounds.size(); i++) {
            if (wowSounds[i]) {
                availableWowSounds.push_back(i);
            }
        }
        if (!availableWowSounds.empty()) {
            int randomIndex = availableWowSounds[rand() % availableWowSounds.size()];
            wowSounds[randomIndex]->play();
            std::cout << "AMAZING! " << linesCleared << " lines cleared! Playing wow_0" << (randomIndex + 1) << ".ogg" << std::endl;
        }
    }
    
    return linesCleared;
}

void updateAllVolumes(sf::Music& backgroundMusic, std::unique_ptr<sf::Sound>& spaceSound, std::unique_ptr<sf::Sound>& laserSound, std::unique_ptr<sf::Sound>& bombSound, std::vector<std::unique_ptr<sf::Sound>>& wowSounds, float masterVolume, float baseMusicVolume, float baseSpaceVolume, float baseLaserVolume, float baseBombVolume, float baseWowVolume) {
    backgroundMusic.setVolume((baseMusicVolume * masterVolume) / 100.0f);
    
    if (spaceSound) {
        spaceSound->setVolume((baseSpaceVolume * masterVolume) / 100.0f);
    }
    
    if (laserSound) {
        laserSound->setVolume((baseLaserVolume * masterVolume) / 100.0f);
    }
    
    if (bombSound) {
        bombSound->setVolume((baseBombVolume * masterVolume) / 100.0f);
    }
    
    for (auto& wowSound : wowSounds) {
        if (wowSound) {
            wowSound->setVolume((baseWowVolume * masterVolume) / 100.0f);
        }
    }
}

void insertNewScore(SaveData& saveData, int score, int lines, int level, ClassicDifficulty difficulty) {
    SaveData::ScoreEntry newEntry = {score, lines, level};
    

    SaveData::ScoreEntry* topScoresArray = nullptr;
    switch (difficulty) {
        case ClassicDifficulty::Easy:
            topScoresArray = saveData.topScoresEasy;
            break;
        case ClassicDifficulty::Medium:
            topScoresArray = saveData.topScoresMedium;
            break;
        case ClassicDifficulty::Hard:
            topScoresArray = saveData.topScoresHard;
            break;
    }
    

    int insertPos = -1;
    for (int i = 0; i < 3; i++) {
        if (score > topScoresArray[i].score) {
            insertPos = i;
            break;
        }
    }
    
    if (insertPos >= 0) {

        for (int i = 2; i > insertPos; i--) {
            topScoresArray[i] = topScoresArray[i-1];
        }
        topScoresArray[insertPos] = newEntry;
        
        std::cout << "NEW TOP SCORE #" << (insertPos + 1) << " for difficulty " << static_cast<int>(difficulty) << ": " << score << " points!" << std::endl;
    }
    

    int legacyInsertPos = -1;
    for (int i = 0; i < 3; i++) {
        if (score > saveData.topScores[i].score) {
            legacyInsertPos = i;
            break;
        }
    }
    
    if (legacyInsertPos >= 0) {
        for (int i = 2; i > legacyInsertPos; i--) {
            saveData.topScores[i] = saveData.topScores[i-1];
        }
        saveData.topScores[legacyInsertPos] = newEntry;
        
        if (legacyInsertPos == 0) {
            saveData.highScore = score;
        }
    }
    

    switch (difficulty) {
        case ClassicDifficulty::Easy:
            if (score > saveData.highScoreClassicEasy) {
                saveData.highScoreClassicEasy = score;
                std::cout << "New Classic Easy high score: " << score << std::endl;
            }
            break;
        case ClassicDifficulty::Medium:
            if (score > saveData.highScoreClassicMedium) {
                saveData.highScoreClassicMedium = score;
                std::cout << "New Classic Medium high score: " << score << std::endl;
            }
            break;
        case ClassicDifficulty::Hard:
            if (score > saveData.highScoreClassicHard) {
                saveData.highScoreClassicHard = score;
                std::cout << "New Classic Hard high score: " << score << std::endl;
            }
            break;
    }
    
    if (score > saveData.highScore) {
        saveData.highScore = score;
    }
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
        window.create(desktopMode, "Jigtriz", sf::Style::None);
    } else {
        window.create(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Jigtriz", sf::Style::Default);
    }
    window.setFramerateLimit(144);
    window.requestFocus();
    std::map<TextureType, sf::Texture> textures;
    std::vector<TextureInfo> textureList = {
        {TextureType::Empty, "Assets/Texture/Cells/cell_background.png", sf::Color(50, 50, 60)},
        {TextureType::GenericBlock, "Assets/Texture/Cells/cell_normal_block.png", sf::Color::White},
        {TextureType::MediumBlock, "Assets/Texture/Cells/cell_medium_block.png", sf::Color::White},
        {TextureType::HardBlock, "Assets/Texture/Cells/cell_hard_block.png", sf::Color::White},
        {TextureType::A_Bomb, "Assets/Texture/Cells/cell_bomb_block.png", sf::Color(255, 100, 100)},
        {TextureType::MuteIcon, "Assets/Texture/Icon/Mute.png", sf::Color::White}
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
        if (titleFont.openFromFile("Assets/Fonts/Jersey25-Regular.ttf")) {
            titleFontLoaded = true;
            std::cout << "Font loaded successfully: Jersey25-Regular.ttf" << std::endl;
        } else {
            std::cout << "Unable to load font: Assets/Fonts/Jersey25-Regular.ttf (attempt " << (attempt + 1) << "/3)" << std::endl;
            if (attempt < 2) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    
    sf::Font menuFont;
    bool menuFontLoaded = false;
    for (int attempt = 0; attempt < 3 && !menuFontLoaded; ++attempt) {
        if (menuFont.openFromFile("Assets/Fonts/Jersey15-Regular.ttf")) {
            menuFontLoaded = true;
            std::cout << "Font loaded successfully: Jersey15-Regular.ttf" << std::endl;
        } else {
            std::cout << "Unable to load font: Assets/Fonts/Jersey15-Regular.ttf (attempt " << (attempt + 1) << "/3)" << std::endl;
            if (attempt < 2) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    
    bool fontLoaded = titleFontLoaded && menuFontLoaded;
    
    sf::Music backgroundMusic;
    sf::SoundBuffer spaceSoundBuffer;
    sf::SoundBuffer laserSoundBuffer;
    sf::SoundBuffer bombSoundBuffer;
    std::vector<sf::SoundBuffer> wowSoundBuffers;
    std::vector<std::unique_ptr<sf::Sound>> wowSounds;
    std::unique_ptr<sf::Sound> spaceSound;
    std::unique_ptr<sf::Sound> laserSound;
    std::unique_ptr<sf::Sound> bombSound;
    
    float masterVolume = saveData.masterVolume;
    bool isMuted = saveData.isMuted;
    float lastMasterVolume = saveData.masterVolume;
    
    if (isMuted) {
        lastMasterVolume = masterVolume;
        masterVolume = 0.0f;
        std::cout << "Game started in MUTED mode" << std::endl;
    }
    
    const float baseMusicVolume = 60.0f;
    const float baseSpaceVolume = 8.0f;
    const float baseLaserVolume = 8.0f;
    const float baseBombVolume = 100.0f;
    const float baseWowVolume = 70.0f;
    
    if (backgroundMusic.openFromFile("Assets/Sound/Music/Jigtriz.ogg")) {
        backgroundMusic.setLooping(true);
        backgroundMusic.setVolume((baseMusicVolume * masterVolume) / 100.0f);
        backgroundMusic.play();
        std::cout << "Background music loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load background music (Assets/Sound/Music/Jigtriz.ogg) - continuing without music." << std::endl;
    }
    if (spaceSoundBuffer.loadFromFile("Assets/Sound/SFX/ground.ogg")) {
        spaceSound = std::make_unique<sf::Sound>(spaceSoundBuffer);
        spaceSound->setVolume((baseSpaceVolume * masterVolume) / 100.0f);
        std::cout << "Space sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load space sound (Assets/Sound/SFX/ground.ogg) - continuing without sound." << std::endl;
    }
    if (laserSoundBuffer.loadFromFile("Assets/Sound/SFX/laser.ogg")) {
        laserSound = std::make_unique<sf::Sound>(laserSoundBuffer);
        laserSound->setVolume((baseLaserVolume * masterVolume) / 100.0f);
        std::cout << "Laser sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load laser sound (Assets/Sound/SFX/laser.ogg) - continuing without sound." << std::endl;
    }
    if (bombSoundBuffer.loadFromFile("Assets/Sound/SFX/bomb.ogg")) {
        bombSound = std::make_unique<sf::Sound>(bombSoundBuffer);
        bombSound->setVolume((baseBombVolume * masterVolume) / 100.0f);
        std::cout << "Bomb sound loaded successfully! Master volume: " << masterVolume << ", Base volume: " << baseBombVolume << ", Final volume: " << bombSound->getVolume() << std::endl;
    } else {
        std::cout << "Unable to load bomb sound (Assets/Sound/SFX/bomb.ogg) - continuing without sound." << std::endl;
    }

    wowSoundBuffers.resize(3);
    wowSounds.resize(3);
    for (int i = 0; i < 3; i++) {
        std::string filename = "Assets/Sound/SFX/wow_0" + std::to_string(i + 1) + ".ogg";
        if (wowSoundBuffers[i].loadFromFile(filename)) {
            wowSounds[i] = std::make_unique<sf::Sound>(wowSoundBuffers[i]);
            wowSounds[i]->setVolume((baseWowVolume * masterVolume) / 100.0f);
            std::cout << "Wow sound loaded successfully: " << filename << std::endl;
        } else {
            std::cout << "Unable to load wow sound: " << filename << " - continuing without this sound." << std::endl;
        }
    }
    
    std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> grid;
    PieceBag jigtrizBag;
    
    std::vector<ExplosionEffect> explosionEffects;
    std::vector<GlowEffect> glowEffects;
    
    float shakeIntensity = 0.0f;
    float shakeDuration = 0.0f;
    float shakeTimer = 0.0f;

    GameState gameState = GameState::MainMenu;
    MenuOption selectedMenuOption = MenuOption::Start;
    GameModeOption selectedGameModeOption = GameModeOption::Classic;
    ClassicDifficulty selectedClassicDifficulty = ClassicDifficulty::Easy;
    SprintLines selectedSprintLines = SprintLines::Lines24;
    ChallengeMode selectedChallengeMode = ChallengeMode::TheForest;
    JigtrizopediaOption selectedJigtrizopediaOption = JigtrizopediaOption::JigtrizPieces;
    OptionsMenuOption selectedOptionsOption = OptionsMenuOption::ClearScores;
    PauseOption selectedPauseOption = PauseOption::Resume;
    ConfirmOption selectedConfirmOption = ConfirmOption::No;
    int hoveredAchievement = -1;
    
    KeyBindings keyBindings;

    keyBindings.moveLeft = static_cast<sf::Keyboard::Key>(saveData.moveLeft);
    keyBindings.moveRight = static_cast<sf::Keyboard::Key>(saveData.moveRight);
    keyBindings.rotateLeft = static_cast<sf::Keyboard::Key>(saveData.rotateLeft);
    keyBindings.rotateRight = static_cast<sf::Keyboard::Key>(saveData.rotateRight);
    keyBindings.quickFall = static_cast<sf::Keyboard::Key>(saveData.quickFall);
    keyBindings.drop = static_cast<sf::Keyboard::Key>(saveData.drop);
    keyBindings.hold = static_cast<sf::Keyboard::Key>(saveData.hold);
    keyBindings.bomb = static_cast<sf::Keyboard::Key>(saveData.bomb);
    keyBindings.mute = static_cast<sf::Keyboard::Key>(saveData.mute);
    keyBindings.volumeDown = static_cast<sf::Keyboard::Key>(saveData.volumeDown);
    keyBindings.volumeUp = static_cast<sf::Keyboard::Key>(saveData.volumeUp);
    keyBindings.menu = static_cast<sf::Keyboard::Key>(saveData.menu);
    
    bool debugMode = false;
    
    bool showVolumeIndicator = false;
    float volumeIndicatorTimer = 0.0f;
    const float VOLUME_INDICATOR_DURATION = 2.0f;
    
    int selectedRebindingIndex = 0;
    bool waitingForKeyPress = false;
    const int MAX_REBINDING_OPTIONS = 13;
    
    int totalLinesCleared = 0;
    int currentLevel = 0;
    int totalScore = 0;
    bool gameOver = false;
    

    float sessionPlayTime = 0.0f;
    int sessionPiecesPlaced = 0;
    

    #define RESET_SESSION_STATS() do { sessionPlayTime = 0.0f; sessionPiecesPlaced = 0; } while(0)
    

    const DifficultyConfig* currentConfig = nullptr;

    float sprintTimer = 0.0f;
    int sprintTargetLines = 0;
    bool sprintModeActive = false;
    bool sprintCompleted = false;
    bool challengeModeActive = false;
    
    int linesSinceLastAbility = 0;
    bool bombAbilityAvailable = false;
    const int LINES_FOR_ABILITY = 10;
    
    int currentCombo = 0;
    const int COMBO_BONUS_PER_LINE = 250;
    int lastMoveScore = 0;
    
    int totalHardDropScore = 0;
    int totalLineScore = 0;
    int totalComboScore = 0;
    const int HARD_DROP_POINTS_PER_CELL = 5;
    

    bool bombUsedThisGame = false;
    int maxComboThisGame = 0;
    

    PieceType heldPiece = PieceType::I_Basic;
    bool hasHeldPiece = false;
    bool canUseHold = true;
    

    float leftHoldTime = 0.0f;
    float rightHoldTime = 0.0f;
    const float DAS_DELAY = 0.3f;
    const float DAS_REPEAT = 0.05f;
    float dasTimer = 0.0f;
    bool leftPressed = false;
    bool rightPressed = false;
    
    PieceType initType = jigtrizBag.getNextPiece();
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
            
            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    if (gameState == GameState::MainMenu) {
                        if (selectedMenuOption == MenuOption::Start) {
                            gameState = GameState::GameModeSelect;
                            selectedGameModeOption = GameModeOption::Classic;
                            std::cout << "Entered GAME MODE selection (mouse)" << std::endl;
                        } else if (selectedMenuOption == MenuOption::Jigtrizopedia) {
                            gameState = GameState::Jigtrizopedia;
                            selectedJigtrizopediaOption = JigtrizopediaOption::JigtrizPieces;
                            std::cout << "Entered JIGTRIZOPEDIA menu (mouse)" << std::endl;
                        } else if (selectedMenuOption == MenuOption::Options) {
                            gameState = GameState::Options;
                            std::cout << "Entered OPTIONS menu (mouse)" << std::endl;
                        } else if (selectedMenuOption == MenuOption::Exit) {
                            window.close();
                        }
                    } else if (gameState == GameState::GameModeSelect) {
                        if (selectedGameModeOption == GameModeOption::Classic) {
                            gameState = GameState::ClassicDifficultySelect;
                            selectedClassicDifficulty = ClassicDifficulty::Easy;
                            std::cout << "Entered CLASSIC difficulty selection (mouse)" << std::endl;
                        } else if (selectedGameModeOption == GameModeOption::Sprint) {
                            gameState = GameState::SprintLinesSelect;
                            selectedSprintLines = SprintLines::Lines24;
                            std::cout << "Entered BLITZ lines selection (mouse)" << std::endl;
                        } else if (selectedGameModeOption == GameModeOption::Challenge) {
                            gameState = GameState::ChallengeSelect;
                            selectedChallengeMode = debugMode ? ChallengeMode::Debug : ChallengeMode::TheForest;
                            std::cout << "Entered CHALLENGE selection (mouse)" << std::endl;
                        }
                    } else if (gameState == GameState::ClassicDifficultySelect || 
                               gameState == GameState::SprintLinesSelect || 
                               gameState == GameState::ChallengeSelect) {

                        gameState = GameState::Playing;
                        gameOver = false;
                        

                        bombUsedThisGame = false;
                        maxComboThisGame = 0;
                        

                        if (selectedGameModeOption == GameModeOption::Sprint) {
                            sprintModeActive = true;
                            challengeModeActive = false;
                            sprintCompleted = false;
                            sprintTimer = 0.0f;
                            switch (selectedSprintLines) {
                                case SprintLines::Lines1:
                                    sprintTargetLines = 1;
                                    break;
                                case SprintLines::Lines24:
                                    sprintTargetLines = 24;
                                    break;
                                case SprintLines::Lines48:
                                    sprintTargetLines = 48;
                                    break;
                                case SprintLines::Lines96:
                                    sprintTargetLines = 96;
                                    break;
                            }
                        } else if (selectedGameModeOption == GameModeOption::Challenge) {
                            challengeModeActive = true;
                            sprintModeActive = false;
                            sprintCompleted = false;
                            sprintTimer = 0.0f;
                        } else {
                            sprintModeActive = false;
                            challengeModeActive = false;
                            sprintCompleted = false;
                        }
                        

                        const DifficultyConfig* config = getDifficultyConfig(
                            selectedGameModeOption,
                            selectedClassicDifficulty,
                            selectedSprintLines,
                            selectedChallengeMode
                        );
                        jigtrizBag.setDifficultyConfig(config);
                        currentConfig = config;
                        
                        for (int i = 0; i < GRID_HEIGHT; ++i) {
                            for (int j = 0; j < GRID_WIDTH; ++j) {
                                grid[i][j] = Cell();
                            }
                        }
                        totalLinesCleared = 0;
                        currentLevel = 0;
                        totalScore = 0;
                        currentCombo = 0;
                        lastMoveScore = 0;
                        totalHardDropScore = 0;
                        totalLineScore = 0;
                        totalComboScore = 0;
                        RESET_SESSION_STATS();
                        jigtrizBag.reset();
                        hasHeldPiece = false;
                        canUseHold = true;
                        linesSinceLastAbility = 0;
                        bombAbilityAvailable = debugMode;
                        explosionEffects.clear();
                        glowEffects.clear();
                        leftHoldTime = 0.0f;
                        rightHoldTime = 0.0f;
                        dasTimer = 0.0f;
                        leftPressed = false;
                        rightPressed = false;
                        
                        PieceType startType = jigtrizBag.getNextPiece();
                        PieceShape startShape = getPieceShape(startType);
                        int startX = (GRID_WIDTH - startShape.width) / 2;
                        int firstFilledRow = findFirstFilledRow(startShape);
                        int startY = -firstFilledRow;
                        activePiece = Piece(startX, startY, startType);
                        
                        std::cout << "Game started (mouse) with mode: " << config->modeName << std::endl;
                    } else if (gameState == GameState::Jigtrizopedia) {
                        if (selectedJigtrizopediaOption == JigtrizopediaOption::Achievements) {
                            gameState = GameState::AchievementsView;
                            std::cout << "Entered ACHIEVEMENTS view (mouse)" << std::endl;
                        } else if (selectedJigtrizopediaOption == JigtrizopediaOption::Statistics) {
                            gameState = GameState::StatisticsView;
                            std::cout << "Entered STATISTICS view (mouse)" << std::endl;
                        } else {
                            std::cout << "Other Jigtrizopedia buttons are currently disabled" << std::endl;
                        }
                    } else if (gameState == GameState::Options) {
                        if (selectedOptionsOption == OptionsMenuOption::ClearScores) {
                            gameState = GameState::ConfirmClearScores;
                            selectedConfirmOption = ConfirmOption::No;
                            std::cout << "Opening confirmation dialog (mouse)" << std::endl;
                        } else if (selectedOptionsOption == OptionsMenuOption::RebindKeys) {
                            gameState = GameState::Rebinding;
                            std::cout << "Entering REBIND KEYS menu (mouse)" << std::endl;
                        }
                    } else if (gameState == GameState::Paused) {
                        if (selectedPauseOption == PauseOption::Resume) {
                            gameState = GameState::Playing;
                            std::cout << "Game resumed (mouse)" << std::endl;
                        } else if (selectedPauseOption == PauseOption::Restart) {
                            gameState = GameState::Playing;
                            gameOver = false;
                            

                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            

                            if (sprintModeActive) {
                                sprintTimer = 0.0f;
                                sprintCompleted = false;
                            }
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            jigtrizBag.reset();
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = false;
                            explosionEffects.clear();
                            glowEffects.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            
                            PieceType startType = jigtrizBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            activePiece = Piece(startX, 0, startType);
                            
                            std::cout << "Game restarted (mouse)" << std::endl;
                        } else if (selectedPauseOption == PauseOption::QuitToMenu) {
                            gameState = GameState::MainMenu;
                            selectedMenuOption = MenuOption::Start;
                            std::cout << "Returned to main menu (mouse)" << std::endl;
                        }
                    }
                }
            }
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                
                if (keyPressed->code == keyBindings.mute) {
                    if (isMuted) {
                        isMuted = false;
                        masterVolume = lastMasterVolume;
                        updateAllVolumes(backgroundMusic, spaceSound, laserSound, bombSound, wowSounds, masterVolume, baseMusicVolume, baseSpaceVolume, baseLaserVolume, baseBombVolume, baseWowVolume);
                        std::cout << "Audio UNMUTED - Master volume restored to: " << masterVolume << "%" << std::endl;
                    } else {
                        isMuted = true;
                        lastMasterVolume = masterVolume;
                        masterVolume = 0.0f;
                        updateAllVolumes(backgroundMusic, spaceSound, laserSound, bombSound, wowSounds, masterVolume, baseMusicVolume, baseSpaceVolume, baseLaserVolume, baseBombVolume, baseWowVolume);
                        std::cout << "Audio MUTED" << std::endl;
                    }
                    showVolumeIndicator = true;
                    volumeIndicatorTimer = VOLUME_INDICATOR_DURATION;
                    saveData.masterVolume = isMuted ? lastMasterVolume : masterVolume;
                    saveData.isMuted = isMuted;
                    saveGameData(saveData);
                } else if (keyPressed->code == keyBindings.volumeDown) {
                    if (!isMuted && masterVolume > 0.0f) {
                        masterVolume = std::max(0.0f, masterVolume - 10.0f);
                        updateAllVolumes(backgroundMusic, spaceSound, laserSound, bombSound, wowSounds, masterVolume, baseMusicVolume, baseSpaceVolume, baseLaserVolume, baseBombVolume, baseWowVolume);
                        std::cout << "Master volume decreased to: " << masterVolume << "%" << std::endl;
                        showVolumeIndicator = true;
                        volumeIndicatorTimer = VOLUME_INDICATOR_DURATION;
                        saveData.masterVolume = masterVolume;
                        saveGameData(saveData);
                    } else if (isMuted) {
                        std::cout << "Cannot change volume while muted" << std::endl;
                    } else {
                        std::cout << "Master volume already at minimum (0%)" << std::endl;
                    }
                } else if (keyPressed->code == keyBindings.volumeUp) {
                    if (!isMuted && masterVolume < 100.0f) {
                        masterVolume = std::min(100.0f, masterVolume + 10.0f);
                        updateAllVolumes(backgroundMusic, spaceSound, laserSound, bombSound, wowSounds, masterVolume, baseMusicVolume, baseSpaceVolume, baseLaserVolume, baseBombVolume, baseWowVolume);
                        std::cout << "Master volume increased to: " << masterVolume << "%" << std::endl;
                        showVolumeIndicator = true;
                        volumeIndicatorTimer = VOLUME_INDICATOR_DURATION;
                        saveData.masterVolume = masterVolume;
                        saveGameData(saveData);
                    } else if (isMuted) {
                        std::cout << "Cannot change volume while muted" << std::endl;
                    } else {
                        std::cout << "Master volume already at maximum (100%)" << std::endl;
                    }
                }
                
                if (gameState == GameState::MainMenu) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape: 
                            window.close(); 
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
                            selectedMenuOption = static_cast<MenuOption>((static_cast<int>(selectedMenuOption) - 1 + 4) % 4);
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            selectedMenuOption = static_cast<MenuOption>((static_cast<int>(selectedMenuOption) + 1) % 4);
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedMenuOption == MenuOption::Start) {
                                gameState = GameState::GameModeSelect;
                                selectedGameModeOption = GameModeOption::Classic;
                                std::cout << "Entered GAME MODE selection" << std::endl;
                            } else if (selectedMenuOption == MenuOption::Jigtrizopedia) {
                                gameState = GameState::Jigtrizopedia;
                                selectedJigtrizopediaOption = JigtrizopediaOption::JigtrizPieces;
                                std::cout << "Entered JIGTRIZOPEDIA menu" << std::endl;
                            } else if (selectedMenuOption == MenuOption::Options) {
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
                            gameState = GameState::Playing;
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
                                gameState = GameState::Playing;
                                std::cout << "Game resumed from menu" << std::endl;
                            } else if (selectedPauseOption == PauseOption::Restart) {
                                gameState = GameState::Playing;
                                gameOver = false;
                                

                                bombUsedThisGame = false;
                                maxComboThisGame = 0;
                                

                                if (sprintModeActive) {
                                    sprintTimer = 0.0f;
                                    sprintCompleted = false;
                                }
                                
                                for (int i = 0; i < GRID_HEIGHT; ++i) {
                                    for (int j = 0; j < GRID_WIDTH; ++j) {
                                        grid[i][j] = Cell();
                                    }
                                }
                                totalLinesCleared = 0;
                                currentLevel = 0;
                                totalScore = 0;
                                currentCombo = 0;
                                lastMoveScore = 0;
                                totalHardDropScore = 0;
                                totalLineScore = 0;
                                totalComboScore = 0;
                                RESET_SESSION_STATS();
                                jigtrizBag.reset();
                                hasHeldPiece = false;
                                canUseHold = true;
                                linesSinceLastAbility = 0;
                                bombAbilityAvailable = false;
                                explosionEffects.clear();
                                glowEffects.clear();
                                leftHoldTime = 0.0f;
                                rightHoldTime = 0.0f;
                                dasTimer = 0.0f;
                                leftPressed = false;
                                rightPressed = false;
                                
                                PieceType startType = jigtrizBag.getNextPiece();
                                PieceShape startShape = getPieceShape(startType);
                                int startX = (GRID_WIDTH - startShape.width) / 2;
                                activePiece = Piece(startX, 0, startType);
                                
                                std::cout << "Game restarted from pause menu!" << std::endl;
                            } else if (selectedPauseOption == PauseOption::QuitToMenu) {
                                gameState = GameState::MainMenu;
                                selectedMenuOption = MenuOption::Start;
                                std::cout << "Returned to main menu" << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::Jigtrizopedia) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            gameState = GameState::MainMenu;
                            selectedMenuOption = MenuOption::Jigtrizopedia;
                            std::cout << "Returned to main menu from JIGTRIZOPEDIA" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            selectedJigtrizopediaOption = static_cast<JigtrizopediaOption>((static_cast<int>(selectedJigtrizopediaOption) + 2) % 3);
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            selectedJigtrizopediaOption = static_cast<JigtrizopediaOption>((static_cast<int>(selectedJigtrizopediaOption) + 1) % 3);
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedJigtrizopediaOption == JigtrizopediaOption::Achievements) {
                                gameState = GameState::AchievementsView;
                                std::cout << "Entered ACHIEVEMENTS view (keyboard)" << std::endl;
                            } else if (selectedJigtrizopediaOption == JigtrizopediaOption::Statistics) {
                                gameState = GameState::StatisticsView;
                                std::cout << "Entered STATISTICS view (keyboard)" << std::endl;
                            } else {
                                std::cout << "Other Jigtrizopedia buttons are currently disabled" << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::GameModeSelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            gameState = GameState::MainMenu;
                            std::cout << "Returned to main menu from GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            selectedGameModeOption = static_cast<GameModeOption>((static_cast<int>(selectedGameModeOption) + 2) % 3);
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            selectedGameModeOption = static_cast<GameModeOption>((static_cast<int>(selectedGameModeOption) + 1) % 3);
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedGameModeOption == GameModeOption::Classic) {
                                gameState = GameState::ClassicDifficultySelect;
                                selectedClassicDifficulty = ClassicDifficulty::Easy;
                                std::cout << "Entered CLASSIC difficulty selection" << std::endl;
                            } else if (selectedGameModeOption == GameModeOption::Sprint) {
                                gameState = GameState::SprintLinesSelect;
                                selectedSprintLines = SprintLines::Lines24;
                                std::cout << "Entered BLITZ lines selection" << std::endl;
                            } else if (selectedGameModeOption == GameModeOption::Challenge) {
                                gameState = GameState::ChallengeSelect;
                                selectedChallengeMode = debugMode ? ChallengeMode::Debug : ChallengeMode::TheForest;
                                std::cout << "Entered CHALLENGE selection" << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::ClassicDifficultySelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            gameState = GameState::GameModeSelect;
                            std::cout << "Returned to GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            selectedClassicDifficulty = static_cast<ClassicDifficulty>((static_cast<int>(selectedClassicDifficulty) + 2) % 3);
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            selectedClassicDifficulty = static_cast<ClassicDifficulty>((static_cast<int>(selectedClassicDifficulty) + 1) % 3);
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                        {

                            const DifficultyConfig* config = getDifficultyConfig(
                                selectedGameModeOption,
                                selectedClassicDifficulty,
                                selectedSprintLines,
                                selectedChallengeMode
                            );
                            jigtrizBag.setDifficultyConfig(config);
                            currentConfig = config;
                            
                            gameState = GameState::Playing;
                            gameOver = false;
                            sprintModeActive = false;
                            challengeModeActive = false;
                            sprintCompleted = false;
                            

                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            jigtrizBag.reset();
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = debugMode;
                            explosionEffects.clear();
                            glowEffects.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            PieceType startType = jigtrizBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(startShape);
                            int startY = -firstFilledRow;
                            activePiece = Piece(startX, startY, startType);
                            std::cout << "Game started (Classic) - Difficulty: " << config->modeName << std::endl;
                            break;
                        }
                        default: 
                            break;
                    }
                } else if (gameState == GameState::SprintLinesSelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            gameState = GameState::GameModeSelect;
                            std::cout << "Returned to GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W: {
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
                                selectedChallengeMode
                            );
                            jigtrizBag.setDifficultyConfig(config);
                            currentConfig = config;
                            
                            gameState = GameState::Playing;
                            gameOver = false;
                            

                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            

                            sprintModeActive = true;
                            challengeModeActive = false;
                            sprintCompleted = false;
                            sprintTimer = 0.0f;
                            switch (selectedSprintLines) {
                                case SprintLines::Lines1:
                                    sprintTargetLines = 1;
                                    break;
                                case SprintLines::Lines24:
                                    sprintTargetLines = 24;
                                    break;
                                case SprintLines::Lines48:
                                    sprintTargetLines = 48;
                                    break;
                                case SprintLines::Lines96:
                                    sprintTargetLines = 96;
                                    break;
                            }
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            jigtrizBag.reset();
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = debugMode;
                            explosionEffects.clear();
                            glowEffects.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            PieceType startType = jigtrizBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(startShape);
                            int startY = -firstFilledRow;
                            activePiece = Piece(startX, startY, startType);
                            std::cout << "Game started (Sprint) - Lines: " << config->modeName << std::endl;
                            break;
                        }
                        default: 
                            break;
                    }
                } else if (gameState == GameState::ChallengeSelect) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            gameState = GameState::GameModeSelect;
                            std::cout << "Returned to GAME MODE selection" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W: {
                            int numOptions = debugMode ? 4 : 3;
                            int current = static_cast<int>(selectedChallengeMode);
                            
                            if (debugMode) {

                                selectedChallengeMode = static_cast<ChallengeMode>((current + numOptions - 1) % numOptions);
                            } else {

                                if (current == 1) current = 3;
                                else current--;
                                selectedChallengeMode = static_cast<ChallengeMode>(current);
                            }
                            break;
                        }
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S: {
                            int numOptions = debugMode ? 4 : 3;
                            int current = static_cast<int>(selectedChallengeMode);
                            
                            if (debugMode) {

                                selectedChallengeMode = static_cast<ChallengeMode>((current + 1) % numOptions);
                            } else {

                                if (current == 3) current = 1;
                                else current++;
                                selectedChallengeMode = static_cast<ChallengeMode>(current);
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
                                selectedChallengeMode
                            );
                            jigtrizBag.setDifficultyConfig(config);
                            currentConfig = config;
                            
                            gameState = GameState::Playing;
                            gameOver = false;
                            

                            challengeModeActive = true;
                            sprintModeActive = false;
                            

                            sprintTimer = 0.0f;
                            sprintCompleted = false;
                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            jigtrizBag.reset();
                            hasHeldPiece = false;
                            canUseHold = true;
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = debugMode;
                            explosionEffects.clear();
                            glowEffects.clear();
                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            PieceType startType = jigtrizBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(startShape);
                            int startY = -firstFilledRow;
                            activePiece = Piece(startX, startY, startType);
                            std::cout << "Game started (Challenge) - Mode: " << config->modeName << std::endl;
                            break;
                        }
                        default: 
                            break;
                    }
                } else if (gameState == GameState::AchievementsView) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            gameState = GameState::Jigtrizopedia;
                            std::cout << "Returned to JIGTRIZOPEDIA from ACHIEVEMENTS" << std::endl;
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::StatisticsView) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            gameState = GameState::Jigtrizopedia;
                            std::cout << "Returned to JIGTRIZOPEDIA from STATISTICS" << std::endl;
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::Options) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
                            gameState = GameState::MainMenu;
                            selectedMenuOption = MenuOption::Options;
                            std::cout << "Returned to main menu from OPTIONS" << std::endl;
                            break;
                        case sf::Keyboard::Key::Up:
                        case sf::Keyboard::Key::W:
                            if (selectedOptionsOption == OptionsMenuOption::ClearScores) {
                                selectedOptionsOption = OptionsMenuOption::RebindKeys;
                            } else if (selectedOptionsOption == OptionsMenuOption::RebindKeys) {
                                selectedOptionsOption = OptionsMenuOption::ClearScores;
                            }
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            if (selectedOptionsOption == OptionsMenuOption::ClearScores) {
                                selectedOptionsOption = OptionsMenuOption::RebindKeys;
                            } else if (selectedOptionsOption == OptionsMenuOption::RebindKeys) {
                                selectedOptionsOption = OptionsMenuOption::ClearScores;
                            }
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedOptionsOption == OptionsMenuOption::ClearScores) {
                                gameState = GameState::ConfirmClearScores;
                                selectedConfirmOption = ConfirmOption::No;
                                std::cout << "Opening confirmation dialog for clearing scores" << std::endl;
                            } else if (selectedOptionsOption == OptionsMenuOption::RebindKeys) {
                                gameState = GameState::Rebinding;
                                std::cout << "Entering REBIND KEYS menu" << std::endl;
                            }
                            break;
                        default: 
                            break;
                    }
                } else if (gameState == GameState::Rebinding) {
                    if (waitingForKeyPress) {
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
                                case 8: keyBindings.mute = keyPressed->code; break;
                                case 9: keyBindings.volumeDown = keyPressed->code; break;
                                case 10: keyBindings.volumeUp = keyPressed->code; break;
                                case 11: keyBindings.menu = keyPressed->code; break;
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
                            saveData.mute = static_cast<int>(keyBindings.mute);
                            saveData.volumeDown = static_cast<int>(keyBindings.volumeDown);
                            saveData.volumeUp = static_cast<int>(keyBindings.volumeUp);
                            saveData.menu = static_cast<int>(keyBindings.menu);
                            saveGameData(saveData);
                            std::cout << "Key bound successfully and saved" << std::endl;
                        }
                    } else {
                        switch (keyPressed->code) {
                            case sf::Keyboard::Key::Escape:
                                gameState = GameState::Options;
                                selectedRebindingIndex = 0;
                                std::cout << "Returned to OPTIONS menu" << std::endl;
                                break;
                            case sf::Keyboard::Key::Up:
                            case sf::Keyboard::Key::W:
                                selectedRebindingIndex--;
                                if (selectedRebindingIndex < 0) selectedRebindingIndex = MAX_REBINDING_OPTIONS - 1;
                                break;
                            case sf::Keyboard::Key::Down:
                            case sf::Keyboard::Key::S:
                                selectedRebindingIndex++;
                                if (selectedRebindingIndex >= MAX_REBINDING_OPTIONS) selectedRebindingIndex = 0;
                                break;
                            case sf::Keyboard::Key::Enter:
                            case sf::Keyboard::Key::Space:
                                if (selectedRebindingIndex == 12) {

                                    keyBindings.moveLeft = sf::Keyboard::Key::A;
                                    keyBindings.moveRight = sf::Keyboard::Key::D;
                                    keyBindings.rotateLeft = sf::Keyboard::Key::J;
                                    keyBindings.rotateRight = sf::Keyboard::Key::K;
                                    keyBindings.quickFall = sf::Keyboard::Key::S;
                                    keyBindings.drop = sf::Keyboard::Key::Space;
                                    keyBindings.hold = sf::Keyboard::Key::L;
                                    keyBindings.bomb = sf::Keyboard::Key::I;
                                    keyBindings.mute = sf::Keyboard::Key::M;
                                    keyBindings.volumeDown = sf::Keyboard::Key::Comma;
                                    keyBindings.volumeUp = sf::Keyboard::Key::Period;
                                    keyBindings.menu = sf::Keyboard::Key::Escape;

                                    saveData.moveLeft = static_cast<int>(keyBindings.moveLeft);
                                    saveData.moveRight = static_cast<int>(keyBindings.moveRight);
                                    saveData.rotateLeft = static_cast<int>(keyBindings.rotateLeft);
                                    saveData.rotateRight = static_cast<int>(keyBindings.rotateRight);
                                    saveData.quickFall = static_cast<int>(keyBindings.quickFall);
                                    saveData.drop = static_cast<int>(keyBindings.drop);
                                    saveData.hold = static_cast<int>(keyBindings.hold);
                                    saveData.bomb = static_cast<int>(keyBindings.bomb);
                                    saveData.mute = static_cast<int>(keyBindings.mute);
                                    saveData.volumeDown = static_cast<int>(keyBindings.volumeDown);
                                    saveData.volumeUp = static_cast<int>(keyBindings.volumeUp);
                                    saveData.menu = static_cast<int>(keyBindings.menu);
                                    saveGameData(saveData);
                                    std::cout << "Key bindings restored to default and saved!" << std::endl;
                                } else if (selectedRebindingIndex < 12) {
                                    waitingForKeyPress = true;
                                    std::cout << "Waiting for key press..." << std::endl;
                                }
                                break;
                            default: 
                                break;
                        }
                    }
                } else if (gameState == GameState::ConfirmClearScores) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape:
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
                            if (selectedConfirmOption == ConfirmOption::Yes) {

                                saveData.highScore = 0;
                                saveData.highScoreClassicEasy = 0;
                                saveData.highScoreClassicMedium = 0;
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
                                

                                for (int i = 0; i < 6; i++) {
                                    saveData.achievements[i] = false;
                                }

                                for (int i = 0; i < 3; i++) {
                                    saveData.topScores[i].score = 0;
                                    saveData.topScores[i].lines = 0;
                                    saveData.topScores[i].level = 0;
                                    
                                    saveData.topScoresEasy[i].score = 0;
                                    saveData.topScoresEasy[i].lines = 0;
                                    saveData.topScoresEasy[i].level = 0;
                                    
                                    saveData.topScoresMedium[i].score = 0;
                                    saveData.topScoresMedium[i].lines = 0;
                                    saveData.topScoresMedium[i].level = 0;
                                    
                                    saveData.topScoresHard[i].score = 0;
                                    saveData.topScoresHard[i].lines = 0;
                                    saveData.topScoresHard[i].level = 0;
                                }
                                saveGameData(saveData);
                                std::cout << "All scores cleared!" << std::endl;
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
                
                if (keyPressed->code == keyBindings.rotateLeft) {
                    if (!gameOver) {
                        activePiece.rotateLeft(grid);
                        saveData.totalRotations++;
                    }
                } else if (keyPressed->code == keyBindings.rotateRight) {
                    if (!gameOver) {
                        activePiece.rotateRight(grid);
                        saveData.totalRotations++;
                    }
                } else if (keyPressed->code == keyBindings.hold) {
                    if (!canUseHold || gameOver) {
                        
                    } else {
                        if (!hasHeldPiece) {

                            heldPiece = activePiece.getType();
                            hasHeldPiece = true;
                            

                            saveData.totalHolds++;
                            

                            if (heldPiece == PieceType::A_Bomb) {
                                unlockAchievement(saveData, Achievement::HoldBomb);
                            }
                            
                            PieceType newType = jigtrizBag.getNextPiece();
                            std::cout << "HOLD: Stored " << pieceTypeToString(heldPiece) << ", spawning " << pieceTypeToString(newType) << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(newShape);
                            int spawnY = -firstFilledRow;
                            activePiece = Piece(spawnX, spawnY, newType);
                        } else {

                            PieceType currentType = activePiece.getType();
                            PieceType swapType = heldPiece;
                            heldPiece = currentType;
                            

                            saveData.totalHolds++;
                            

                            if (heldPiece == PieceType::A_Bomb) {
                                unlockAchievement(saveData, Achievement::HoldBomb);
                            }
                            
                            std::cout << "SWAP: " << pieceTypeToString(currentType) << " <-> " << pieceTypeToString(swapType) << std::endl;
                            PieceShape swapShape = getPieceShape(swapType);
                            int spawnX = (GRID_WIDTH - swapShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(swapShape);
                            int spawnY = -firstFilledRow;
                            activePiece = Piece(spawnX, spawnY, swapType);
                        }
                        
                        canUseHold = false;
                    }
                } else if (keyPressed->code == keyBindings.drop) {
                    if (!gameOver) {

                        if (activePiece.getAbility() == AbilityType::Bomb) {
                            std::cout << "BOMB EXPLOSION TRIGGERED!" << std::endl;
                            
                            int bombCenterX = activePiece.getX();
                            int bombCenterY = activePiece.getY();
                            

                            for (int dy = -2; dy <= 2; ++dy) {
                                for (int dx = -2; dx <= 2; ++dx) {
                                    int x = bombCenterX + dx;
                                    int y = bombCenterY + dy;
                                    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                                        sf::Color blockColor = grid[y][x].occupied ? grid[y][x].color : sf::Color(200, 200, 200);
                                        
                                        grid[y][x] = Cell();
                                        explosionEffects.push_back(ExplosionEffect(x, y));
                                        
                                        float baseWorldX = GRID_OFFSET_X + x * CELL_SIZE;
                                        float baseWorldY = GRID_OFFSET_Y + y * CELL_SIZE;
                                        float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                                        float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f;
                                        
                                        glowEffects.push_back(GlowEffect(baseWorldX + offsetX, baseWorldY + offsetY, blockColor));
                                    }
                                }
                            }
                            
                            shakeIntensity = 15.0f;
                            shakeDuration = 0.4f;
                            shakeTimer = 0.0f;
                            
                            if (bombSound) {
                                bombSound->play();
                            }
                            

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
                            
                            int clearedLines = clearFullLines(grid, laserSound, wowSounds, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer);
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
                                
                                std::cout << "Bomb cleared " << clearedLines << " lines! Base: " << baseScore << " | Combo: x" << (currentCombo - clearedLines) << " (+" << comboBonus << ") | Total: +" << bombLineScore << std::endl;
                                
                                linesSinceLastAbility += clearedLines;
                                if (linesSinceLastAbility >= LINES_FOR_ABILITY) {
                                    bombAbilityAvailable = true;
                                    std::cout << "BOMB ABILITY READY AGAIN!" << std::endl;
                                }
                            } else {
                                lastMoveScore = 0;
                            }
                            

                            PieceType newType = jigtrizBag.getNextPiece();
                            std::cout << "Spawning new piece after explosion: " << pieceTypeToString(newType) << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(newShape);
                            int spawnY = -firstFilledRow;
                            activePiece = Piece(spawnX, spawnY, newType);
                            canUseHold = true;
                        } else {

                            int dropDistance = activePiece.moveGround(grid);
                            int dropPoints = dropDistance * HARD_DROP_POINTS_PER_CELL;
                            totalScore += dropPoints;
                            totalHardDropScore += dropPoints;
                            std::cout << "Hard drop: " << dropDistance << " cells = +" << dropPoints << " points" << std::endl;
                            if (spaceSound) {
                                spaceSound->play();
                            }
                        }
                    }
                } else if (keyPressed->code == keyBindings.menu) {
                    if (gameOver) {

                    } else {

                        gameState = GameState::Paused;
                        selectedPauseOption = PauseOption::Resume;
                        std::cout << "Game paused" << std::endl;
                    }
                } else if (keyPressed->code == keyBindings.bomb) {
                    if ((bombAbilityAvailable || debugMode) && !gameOver) {
                        std::cout << "BOMB ABILITY ACTIVATED - Spawning bomb!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
                        

                        bombUsedThisGame = true;
                        

                        saveData.totalBombsUsed++;
                        

                        PieceType replacedPiece = activePiece.getType();
                        jigtrizBag.returnPieceToBag(replacedPiece);
                        std::cout << "Returned " << pieceTypeToString(replacedPiece) << " to bag" << std::endl;
                        
                        PieceType newType = PieceType::A_Bomb;
                        std::cout << "Spawning bomb piece!" << std::endl;
                        PieceShape newShape = getPieceShape(newType);
                        int spawnX = (GRID_WIDTH - newShape.width) / 2;
                        int firstFilledRow = findFirstFilledRow(newShape);
                        int spawnY = -firstFilledRow;
                        activePiece = Piece(spawnX, spawnY, newType);
                        
                        if (!debugMode) {
                            bombAbilityAvailable = false;
                            linesSinceLastAbility = 0;
                        }
                    } else if (!bombAbilityAvailable && !debugMode && !gameOver) {
                        int linesNeeded = LINES_FOR_ABILITY - linesSinceLastAbility;
                        std::cout << "Bomb ability not ready yet! Need " << linesNeeded << " more lines." << std::endl;
                    }
                } else if (keyPressed->code == sf::Keyboard::Key::R) {
                    if (!gameOver) {
                        std::cout << "R key pressed during game! Restarting..." << std::endl;
                        

                        bombUsedThisGame = false;
                        maxComboThisGame = 0;
                        
                        if (sprintModeActive) {
                            sprintTimer = 0.0f;
                            sprintCompleted = false;
                        }
                        
                        for (int i = 0; i < GRID_HEIGHT; ++i) {
                            for (int j = 0; j < GRID_WIDTH; ++j) {
                                grid[i][j] = Cell();
                            }
                        }
                        totalLinesCleared = 0;
                        currentLevel = 0;
                        totalScore = 0;
                        currentCombo = 0;
                        lastMoveScore = 0;
                        totalHardDropScore = 0;
                        totalLineScore = 0;
                        totalComboScore = 0;
                        RESET_SESSION_STATS();
                        
                        jigtrizBag.reset();
                        
                        hasHeldPiece = false;
                        canUseHold = true;
                        
                        leftHoldTime = 0.0f;
                        rightHoldTime = 0.0f;
                        dasTimer = 0.0f;
                        leftPressed = false;
                        rightPressed = false;
                        
                        linesSinceLastAbility = 0;
                        bombAbilityAvailable = debugMode;
                        explosionEffects.clear();
                        glowEffects.clear();
                        
                        PieceType startType = jigtrizBag.getNextPiece();
                        PieceShape startShape = getPieceShape(startType);
                        int startX = (GRID_WIDTH - startShape.width) / 2;
                        int firstFilledRow = findFirstFilledRow(startShape);
                        int startY = -firstFilledRow;
                        activePiece = Piece(startX, startY, startType);
                        
                        std::cout << "Game restarted during play!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
                    }
                }
                

                if (gameOver) {
                    switch (keyPressed->code) {
                        case sf::Keyboard::Key::Escape: {
                            std::cout << "ESC pressed! Returning to main menu..." << std::endl;
                            gameState = GameState::MainMenu;
                            gameOver = false;
                            selectedMenuOption = MenuOption::Start;
                            break;
                        }
                        case sf::Keyboard::Key::R: {
                            std::cout << "R key pressed! Restarting game..." << std::endl;
                            gameState = GameState::Playing;
                            gameOver = false;
                            

                            bombUsedThisGame = false;
                            maxComboThisGame = 0;
                            

                            if (sprintModeActive) {
                                sprintTimer = 0.0f;
                                sprintCompleted = false;
                            }
                            
                            for (int i = 0; i < GRID_HEIGHT; ++i) {
                                for (int j = 0; j < GRID_WIDTH; ++j) {
                                    grid[i][j] = Cell();
                                }
                            }
                            totalLinesCleared = 0;
                            currentLevel = 0;
                            totalScore = 0;
                            currentCombo = 0;
                            lastMoveScore = 0;
                            totalHardDropScore = 0;
                            totalLineScore = 0;
                            totalComboScore = 0;
                            RESET_SESSION_STATS();
                            
                            jigtrizBag.reset();
                            
                            hasHeldPiece = false;
                            canUseHold = true;

                            leftHoldTime = 0.0f;
                            rightHoldTime = 0.0f;
                            dasTimer = 0.0f;
                            leftPressed = false;
                            rightPressed = false;
                            
                            linesSinceLastAbility = 0;
                            bombAbilityAvailable = debugMode;
                            explosionEffects.clear();
                            glowEffects.clear();
                            

                            PieceType startType = jigtrizBag.getNextPiece();
                            PieceShape startShape = getPieceShape(startType);
                            int startX = (GRID_WIDTH - startShape.width) / 2;
                            int firstFilledRow = findFirstFilledRow(startShape);
                            int startY = -firstFilledRow;
                            activePiece = Piece(startX, startY, startType);
                            
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
                            window.create(desktopMode, "Jigtriz", sf::Style::None);
                            std::cout << "Switched to FULLSCREEN mode" << std::endl;
                        } else {
                            window.create(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Jigtriz", sf::Style::Default);
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
        
        if (gameState == GameState::AchievementsView) {
            auto mousePos = sf::Mouse::getPosition(window);
            

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
        

        if (!gameOver) {
            sessionPlayTime += deltaTime;
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
            activePiece.update(deltaTime, fastFall, grid);
            

            bool hasLineGoal = currentConfig && currentConfig->hasLineGoal;
            if (hasLineGoal) {
                sprintTimer += deltaTime;
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
        
        if (activePiece.hasStopped() && !gameOver) {
            activePiece.ChangeToStatic(grid, activePiece.getAbility(), &bombSound, &explosionEffects, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer);
            

            sessionPiecesPlaced++;
            
            int clearedLines = clearFullLines(grid, laserSound, wowSounds, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer);
            totalLinesCleared += clearedLines;
            

            bool hasLineGoal = currentConfig && currentConfig->hasLineGoal;
            int lineGoal = hasLineGoal ? currentConfig->lineGoal : 0;
            
            if (hasLineGoal && totalLinesCleared >= lineGoal) {
                gameOver = true;
                sprintCompleted = true;
                std::cout << "MODE COMPLETED! Time: " << sprintTimer << " seconds | Lines: " << totalLinesCleared << "/" << lineGoal << std::endl;
                

                if (sprintModeActive && lineGoal == 48 && sprintTimer < 120.0f) {
                    unlockAchievement(saveData, Achievement::Blitz48Under230);
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
                    switch (selectedChallengeMode) {
                        case ChallengeMode::Debug:
                            bestTimePtr = &saveData.bestTimeChallengeDebug;
                            break;
                        case ChallengeMode::TheForest:
                            bestTimePtr = &saveData.bestTimeChallengeTheForest;
                            break;
                        case ChallengeMode::Randomness:
                            bestTimePtr = &saveData.bestTimeChallengeRandomness;
                            break;
                        case ChallengeMode::NonStraight:
                            bestTimePtr = &saveData.bestTimeChallengeNonStraight;
                            break;
                    }
                    
                    if (bestTimePtr && (*bestTimePtr == 0.0f || sprintTimer < *bestTimePtr)) {
                        *bestTimePtr = sprintTimer;
                        std::cout << "NEW BEST TIME for challenge: " << sprintTimer << " seconds!" << std::endl;
                        saveGameData(saveData);
                    }
                }
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
                

                if (currentCombo >= 10) {
                    unlockAchievement(saveData, Achievement::Combo10);
                }
                

                if (clearedLines >= 5) {
                    unlockAchievement(saveData, Achievement::Combo5OneClear);
                }
                

                if (clearedLines >= 6) {
                    unlockAchievement(saveData, Achievement::Combo6OneClear);
                }
                

                if (currentCombo > maxComboThisGame) {
                    maxComboThisGame = currentCombo;
                }
                

                if (currentCombo > saveData.maxComboEver) {
                    saveData.maxComboEver = currentCombo;
                }
                
                std::cout << "Lines cleared: " << clearedLines << " | Base: " << baseScore << " | Combo before: x" << (currentCombo - clearedLines) << " (+" << comboBonus << ") | Total: +" << lastMoveScore << " | New combo: x" << currentCombo << " | Score: " << totalScore << std::endl;
                
                linesSinceLastAbility += clearedLines;
                
                if (linesSinceLastAbility >= LINES_FOR_ABILITY) {
                    bombAbilityAvailable = true;
                    std::cout << "BOMB ABILITY READY! Press 'I' to activate!" << std::endl;
                }
            } else {
                lastMoveScore = 0;
                currentCombo = std::max(0, currentCombo - 2);
                std::cout << "No lines cleared | Combo decreased to: " << currentCombo << std::endl;
            }
            
            int newLevel = calculateLevel(totalLinesCleared);
            if (newLevel != currentLevel) {
                currentLevel = newLevel;
                jigtrizBag.updateLevel(currentLevel);
                std::cout << "LEVEL UP! Level " << currentLevel << " reached! (Lines: " << totalLinesCleared << ")" << std::endl;
            }
            
            PieceType randomType = jigtrizBag.getNextPiece();
            std::cout << "Respawn: " << pieceTypeToString(randomType) << " (Bag System)" << std::endl;
            PieceShape newShape = getPieceShape(randomType);
            int spawnX = (GRID_WIDTH - newShape.width) / 2;
            

            int firstFilledRow = findFirstFilledRow(newShape);
            int spawnY = -firstFilledRow;
            
            activePiece = Piece(spawnX, spawnY, randomType);
            
            if (activePiece.collidesAt(grid, spawnX, spawnY)) {
                gameOver = true;

                std::cout << "GAME OVER! New piece overlaps with existing blocks at spawn position (" << spawnX << ", " << spawnY << ")" << std::endl;
                std::cout << "Final Score: " << totalScore << " | Lines: " << totalLinesCleared << " | Level: " << currentLevel << std::endl;
                

                if (!debugMode && !sprintModeActive) {
                    insertNewScore(saveData, totalScore, totalLinesCleared, currentLevel, selectedClassicDifficulty);
                    

                    if (totalScore >= 200000 && !bombUsedThisGame) {
                        unlockAchievement(saveData, Achievement::Score200kNoBomb);
                    }
                    

                    if (totalScore >= 400000 && (selectedClassicDifficulty == ClassicDifficulty::Medium || selectedClassicDifficulty == ClassicDifficulty::Hard)) {
                        unlockAchievement(saveData, Achievement::Score400kMedHard);
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
                    
                    saveData.masterVolume = masterVolume;
                    saveData.isMuted = isMuted;
                    
                    saveGameData(saveData);
                    
                    std::cout << "Game data saved with new scores!" << std::endl;
                } else {
                    std::cout << "Score not saved (DEBUG MODE)" << std::endl;
                }
            }
            
            canUseHold = true;
            

            leftHoldTime = 0.0f;
            rightHoldTime = 0.0f;
            dasTimer = 0.0f;
            leftPressed = false;
            rightPressed = false;
        }
        
        }
        
        window.clear(sf::Color::Black);
        

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        float mouseX = static_cast<float>(mousePos.x);
        float mouseY = static_cast<float>(mousePos.y);
        float centerX = WINDOW_WIDTH / 2.0f;
        float centerY = WINDOW_HEIGHT / 2.0f;
        
        if (gameState == GameState::MainMenu) {





            

            if (mouseX >= centerX - 150 && mouseX <= centerX + 150 &&
                mouseY >= centerY - 90 && mouseY <= centerY - 30) {
                selectedMenuOption = MenuOption::Start;
            }

            else if (mouseX >= centerX - 200 && mouseX <= centerX + 200 &&
                     mouseY >= centerY + 10 && mouseY <= centerY + 70) {
                selectedMenuOption = MenuOption::Jigtrizopedia;
            }

            else if (mouseX >= centerX - 150 && mouseX <= centerX + 150 &&
                     mouseY >= centerY + 110 && mouseY <= centerY + 170) {
                selectedMenuOption = MenuOption::Options;
            }

            else if (mouseX >= centerX - 150 && mouseX <= centerX + 150 &&
                     mouseY >= centerY + 210 && mouseY <= centerY + 270) {
                selectedMenuOption = MenuOption::Exit;
            }
        } else if (gameState == GameState::GameModeSelect) {

            float startY = centerY - 80.0f;
            float spacing = 90.0f;
            
            for (int i = 0; i < 3; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mouseX >= centerX - 200 && mouseX <= centerX + 200 &&
                    mouseY >= selectorY && mouseY <= selectorY + 60) {
                    selectedGameModeOption = static_cast<GameModeOption>(i);
                    break;
                }
            }
        } else if (gameState == GameState::ClassicDifficultySelect) {

            float startY = centerY - 80.0f;
            float spacing = 90.0f;
            
            for (int i = 0; i < 3; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mouseX >= centerX - 175 && mouseX <= centerX + 175 &&
                    mouseY >= selectorY && mouseY <= selectorY + 60) {
                    selectedClassicDifficulty = static_cast<ClassicDifficulty>(i);
                    break;
                }
            }
        } else if (gameState == GameState::SprintLinesSelect) {

            float startY = centerY - 80.0f;
            float spacing = 90.0f;
            int numOptions = debugMode ? 4 : 3;
            int startEnumValue = debugMode ? 0 : 1;
            
            for (int i = 0; i < numOptions; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mouseX >= centerX - 275 && mouseX <= centerX + 275 &&
                    mouseY >= selectorY && mouseY <= selectorY + 60) {
                    selectedSprintLines = static_cast<SprintLines>(i + startEnumValue);
                    break;
                }
            }
        } else if (gameState == GameState::ChallengeSelect) {

            float startY = centerY - 80.0f;
            float spacing = 90.0f;
            int numOptions = debugMode ? 4 : 3;
            int startEnumValue = debugMode ? 0 : 1;
            
            for (int i = 0; i < numOptions; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mouseX >= centerX - 250 && mouseX <= centerX + 250 &&
                    mouseY >= selectorY && mouseY <= selectorY + 60) {
                    selectedChallengeMode = static_cast<ChallengeMode>(i + startEnumValue);
                    break;
                }
            }
        } else if (gameState == GameState::Jigtrizopedia) {

            float startY = centerY - 200.0f;
            float spacing = 70.0f;
            
            for (int i = 0; i < 3; i++) {
                float selectorY = startY + i * spacing - 5;
                if (mouseX >= centerX - 225 && mouseX <= centerX + 225 &&
                    mouseY >= selectorY && mouseY <= selectorY + 55) {
                    selectedJigtrizopediaOption = static_cast<JigtrizopediaOption>(i);
                    break;
                }
            }
        } else if (gameState == GameState::Options) {



            

            if (mouseX >= centerX - 200 && mouseX <= centerX + 200 &&
                mouseY >= centerY - 85 && mouseY <= centerY - 25) {
                selectedOptionsOption = OptionsMenuOption::ClearScores;
            }

            else if (mouseX >= centerX - 200 && mouseX <= centerX + 200 &&
                     mouseY >= centerY + 15 && mouseY <= centerY + 75) {
                selectedOptionsOption = OptionsMenuOption::RebindKeys;
            }
        } else if (gameState == GameState::Paused) {




            

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
        
        sf::View view = window.getDefaultView();
        if (shakeTimer < shakeDuration && gameState == GameState::Playing) {
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
            window.setView(view);
        } else {
            view.setCenter(sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
            window.setView(view);
        }
        
        if (gameState == GameState::MainMenu) {
            drawMainMenu(window, titleFont, menuFont, fontLoaded, selectedMenuOption, debugMode);
            
            if (isMuted) {
                if (textures.find(TextureType::MuteIcon) != textures.end()) {
                    sf::Sprite muteSprite(textures.at(TextureType::MuteIcon));
                    sf::Vector2u textureSize = textures.at(TextureType::MuteIcon).getSize();
                    float iconSize = 48.0f;
                    float scale = iconSize / textureSize.x;
                    muteSprite.setScale(sf::Vector2f(scale, scale));
                    muteSprite.setPosition(sf::Vector2f(1920.0f - iconSize - 20.0f, 20.0f));
                    window.draw(muteSprite);
                } else if (fontLoaded) {
                    sf::Text muteText(menuFont);
                    muteText.setString("MUTED");
                    muteText.setCharacterSize(20);
                    muteText.setFillColor(sf::Color::Red);
                    muteText.setPosition(sf::Vector2f(1920.0f - 100.0f, 20.0f));
                    window.draw(muteText);
                }
            }
            
            if (showVolumeIndicator && fontLoaded) {
                float displayVolume = isMuted ? lastMasterVolume : masterVolume;
                std::string volumeStr = "Volume: " + std::to_string(static_cast<int>(displayVolume)) + "%";
                
                sf::Text volumeText(menuFont);
                volumeText.setString(volumeStr);
                volumeText.setCharacterSize(32);
                volumeText.setFillColor(sf::Color(255, 255, 255, 255));
                volumeText.setOutlineColor(sf::Color::Black);
                volumeText.setOutlineThickness(2);
                volumeText.setStyle(sf::Text::Bold);
                
                sf::FloatRect textBounds = volumeText.getLocalBounds();
                float textWidth = textBounds.size.x;
                

                float barWidth = 200.0f;
                float barHeight = 10.0f;
                

                float boxWidth = std::max(textWidth + 20.0f, barWidth + 20.0f);
                float boxHeight = 55.0f;
                
                float posX = 1920.0f - boxWidth - 20.0f;
                float posY = isMuted ? 80.0f : 20.0f;
                

                sf::RectangleShape volumeBg;
                volumeBg.setSize(sf::Vector2f(boxWidth, boxHeight));
                volumeBg.setPosition(sf::Vector2f(posX, posY));
                volumeBg.setFillColor(sf::Color(0, 0, 0, 180));
                volumeBg.setOutlineColor(sf::Color(100, 150, 255, 255));
                volumeBg.setOutlineThickness(2);
                window.draw(volumeBg);
                

                volumeText.setPosition(sf::Vector2f(posX + (boxWidth - textWidth) / 2.0f, posY + 5.0f));
                window.draw(volumeText);
                

                float barPosX = posX + (boxWidth - barWidth) / 2.0f;
                float barPosY = posY + 38.0f;
                
                sf::RectangleShape volumeBarBg;
                volumeBarBg.setSize(sf::Vector2f(barWidth, barHeight));
                volumeBarBg.setPosition(sf::Vector2f(barPosX, barPosY));
                volumeBarBg.setFillColor(sf::Color(50, 50, 50, 200));
                volumeBarBg.setOutlineColor(sf::Color(100, 100, 100, 255));
                volumeBarBg.setOutlineThickness(1);
                window.draw(volumeBarBg);
                
                float fillWidth = (displayVolume / 100.0f) * barWidth;
                sf::RectangleShape volumeBarFill;
                volumeBarFill.setSize(sf::Vector2f(fillWidth, barHeight));
                volumeBarFill.setPosition(sf::Vector2f(barPosX, barPosY));
                
                sf::Color barColor;
                if (displayVolume >= 70.0f) {
                    barColor = sf::Color(100, 255, 100);
                } else if (displayVolume >= 30.0f) {
                    barColor = sf::Color(255, 255, 100);
                } else {
                    barColor = sf::Color(255, 100, 100);
                }
                volumeBarFill.setFillColor(barColor);
                window.draw(volumeBarFill);
            }
        } else if (gameState == GameState::GameModeSelect) {
            drawGameModeMenu(window, titleFont, menuFont, fontLoaded, selectedGameModeOption);
        } else if (gameState == GameState::ClassicDifficultySelect) {
            drawClassicDifficultyMenu(window, titleFont, menuFont, fontLoaded, selectedClassicDifficulty, saveData);
        } else if (gameState == GameState::SprintLinesSelect) {
            drawSprintLinesMenu(window, titleFont, menuFont, fontLoaded, selectedSprintLines, saveData, debugMode);
        } else if (gameState == GameState::ChallengeSelect) {
            drawChallengeMenu(window, titleFont, menuFont, fontLoaded, selectedChallengeMode, debugMode, saveData);
        } else if (gameState == GameState::Jigtrizopedia) {
            drawJigtrizopediaMenu(window, titleFont, menuFont, fontLoaded, selectedJigtrizopediaOption);
        } else if (gameState == GameState::AchievementsView) {
            drawAchievementsScreen(window, titleFont, menuFont, fontLoaded, saveData, hoveredAchievement);
        } else if (gameState == GameState::StatisticsView) {
            drawStatisticsScreen(window, titleFont, menuFont, fontLoaded, saveData);
        } else if (gameState == GameState::Options) {
            drawOptionsMenu(window, menuFont, fontLoaded, debugMode, selectedOptionsOption);
            
            if (isMuted) {
                if (textures.find(TextureType::MuteIcon) != textures.end()) {
                    sf::Sprite muteSprite(textures.at(TextureType::MuteIcon));
                    sf::Vector2u textureSize = textures.at(TextureType::MuteIcon).getSize();
                    float iconSize = 48.0f;
                    float scale = iconSize / textureSize.x;
                    muteSprite.setScale(sf::Vector2f(scale, scale));
                    muteSprite.setPosition(sf::Vector2f(1920.0f - iconSize - 20.0f, 20.0f));
                    window.draw(muteSprite);
                } else {
                    if (fontLoaded) {
                        sf::Text muteText(menuFont);
                        muteText.setString("MUTED");
                        muteText.setCharacterSize(28);
                        muteText.setFillColor(sf::Color::Red);
                        muteText.setStyle(sf::Text::Bold);
                        muteText.setPosition(sf::Vector2f(1920.0f - 140.0f, 20.0f));
                        window.draw(muteText);
                    }
                }
            }

            if (showVolumeIndicator && volumeIndicatorTimer > 0.0f && fontLoaded) {
                float displayVolume = masterVolume;
                std::string volumeStr = "Volume: " + std::to_string((int)displayVolume) + "%";
                
                sf::Text volumeText(menuFont);
                volumeText.setString(volumeStr);
                volumeText.setCharacterSize(32);
                volumeText.setFillColor(sf::Color::White);
                volumeText.setStyle(sf::Text::Bold);

                sf::FloatRect volumeBounds = volumeText.getLocalBounds();
                float volumeWidth = volumeBounds.size.x + 40.0f;
                float volumeHeight = 100.0f;
                float volumeX = 1920.0f - volumeWidth - 20.0f;
                float volumeY = isMuted ? 80.0f : 20.0f;

                sf::RectangleShape volumeBox;
                volumeBox.setSize(sf::Vector2f(volumeWidth, volumeHeight));
                volumeBox.setFillColor(sf::Color(0, 0, 0, 180));
                volumeBox.setOutlineColor(sf::Color(100, 150, 255));
                volumeBox.setOutlineThickness(3.0f);
                volumeBox.setPosition(sf::Vector2f(volumeX, volumeY));
                window.draw(volumeBox);

                volumeText.setOrigin(sf::Vector2f(volumeBounds.size.x / 2.0f, volumeBounds.size.y / 2.0f));
                volumeText.setPosition(sf::Vector2f(volumeX + volumeWidth / 2.0f, volumeY + 25.0f));

                sf::Text volumeOutline(menuFont);
                volumeOutline.setString(volumeStr);
                volumeOutline.setCharacterSize(32);
                volumeOutline.setFillColor(sf::Color::Black);
                volumeOutline.setStyle(sf::Text::Bold);
                volumeOutline.setOrigin(sf::Vector2f(volumeBounds.size.x / 2.0f, volumeBounds.size.y / 2.0f));
                volumeOutline.setPosition(sf::Vector2f(volumeX + volumeWidth / 2.0f + 2.0f, volumeY + 27.0f));

                window.draw(volumeOutline);
                window.draw(volumeText);

                float barWidth = 200.0f;
                float barHeight = 20.0f;
                float barX = volumeX + (volumeWidth - barWidth) / 2.0f;
                float barY = volumeY + 60.0f;

                sf::RectangleShape volumeBarBg;
                volumeBarBg.setSize(sf::Vector2f(barWidth, barHeight));
                volumeBarBg.setFillColor(sf::Color(50, 50, 50));
                volumeBarBg.setOutlineColor(sf::Color::White);
                volumeBarBg.setOutlineThickness(2.0f);
                volumeBarBg.setPosition(sf::Vector2f(barX, barY));
                window.draw(volumeBarBg);

                float fillWidth = (displayVolume / 100.0f) * (barWidth - 4.0f);
                sf::RectangleShape volumeBarFill;
                volumeBarFill.setSize(sf::Vector2f(fillWidth, barHeight - 4.0f));
                volumeBarFill.setPosition(sf::Vector2f(barX + 2.0f, barY + 2.0f));

                sf::Color barColor;
                if (displayVolume >= 70.0f) {
                    barColor = sf::Color(100, 255, 100);
                } else if (displayVolume >= 30.0f) {
                    barColor = sf::Color(255, 255, 100);
                } else {
                    barColor = sf::Color(255, 100, 100);
                }
                volumeBarFill.setFillColor(barColor);
                window.draw(volumeBarFill);
            }
        } else if (gameState == GameState::Rebinding) {
            drawRebindingScreen(window, menuFont, fontLoaded, keyBindings, selectedRebindingIndex, waitingForKeyPress);
        } else if (gameState == GameState::ConfirmClearScores) {

            drawOptionsMenu(window, menuFont, fontLoaded, debugMode, selectedOptionsOption);

            drawConfirmClearScores(window, menuFont, fontLoaded, selectedConfirmOption);
        } else if (gameState == GameState::Playing || gameState == GameState::Paused) {
        drawGridBackground(window);
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                float worldX = GRID_OFFSET_X + j * CELL_SIZE;
                float worldY = GRID_OFFSET_Y + i * CELL_SIZE;
                TextureType cellTextureType = grid[i][j].occupied ? grid[i][j].getTextureType() : TextureType::Empty;
                

                if (grid[i][j].occupied && textures.find(TextureType::GenericBlock) != textures.end()) {
                    sf::Sprite cellSprite(textures.at(TextureType::GenericBlock));
                    cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                    cellSprite.setColor(grid[i][j].color);
                    sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
                    cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                    window.draw(cellSprite);
                } else if (textures.find(cellTextureType) != textures.end()) {

                    sf::Sprite cellSprite(textures.at(cellTextureType));
                    cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                    sf::Vector2u textureSize = textures.at(cellTextureType).getSize();
                    cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                    window.draw(cellSprite);
                } else {

                    sf::RectangleShape cellShape;
                    cellShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                    cellShape.setPosition(sf::Vector2f(worldX, worldY));
                    if (grid[i][j].occupied) {
                        cellShape.setFillColor(grid[i][j].color);
                    } else {
                        cellShape.setFillColor(sf::Color(50, 50, 60));
                    }
                    window.draw(cellShape);
                }
            }
        }

        activePiece.drawGhost(window, textures, useTextures, grid);
        activePiece.draw(window, textures, useTextures);
        drawExplosionEffects(window, explosionEffects);
        drawGlowEffects(window, glowEffects, textures);
        drawNextPieces(window, jigtrizBag.getNextQueue(), textures, useTextures);
        drawHeldPiece(window, heldPiece, hasHeldPiece, textures, useTextures, menuFont, fontLoaded);
        drawBombAbility(window, bombAbilityAvailable, linesSinceLastAbility, textures, useTextures, menuFont, fontLoaded);
        

        bool useSprintUI = sprintModeActive || (currentConfig && currentConfig->hasLineGoal);
        int targetLines = sprintModeActive ? sprintTargetLines : (currentConfig && currentConfig->hasLineGoal ? currentConfig->lineGoal : 0);
        drawLevelInfo(window, totalLinesCleared, currentLevel, totalScore, textures, useTextures, menuFont, fontLoaded, useSprintUI, sprintTimer, targetLines);
        if (!useSprintUI) {
            drawCombo(window, currentCombo, lastMoveScore, menuFont, fontLoaded);
        }
        drawGridBorder(window);
        drawJigtrizTitle(window, titleFont, fontLoaded);
        
        if (gameOver) {

            bool useLineGoalInterface = currentConfig && currentConfig->hasLineGoal;
            int lineTarget = useLineGoalInterface ? currentConfig->lineGoal : 0;
            
            drawGameOver(window, totalScore, totalLinesCleared, currentLevel, textures, useTextures, menuFont, fontLoaded, saveData, totalHardDropScore, totalLineScore, totalComboScore, selectedClassicDifficulty, useLineGoalInterface, sprintTimer, lineTarget, sprintCompleted, challengeModeActive);
        }
        
        if (gameState == GameState::Paused) {
            drawPauseMenu(window, menuFont, fontLoaded, selectedPauseOption);
        }
        
        if (debugMode && fontLoaded) {
            sf::Text debugText(menuFont);
            debugText.setString("DEBUG MODE");
            debugText.setCharacterSize(24);
            debugText.setFillColor(sf::Color::Yellow);
            debugText.setPosition(sf::Vector2f(1920.0f - 180.0f, 1080.0f - 40.0f));
            window.draw(debugText);
        }
        
        if (isMuted) {
            if (textures.find(TextureType::MuteIcon) != textures.end()) {
                sf::Sprite muteSprite(textures.at(TextureType::MuteIcon));
                sf::Vector2u textureSize = textures.at(TextureType::MuteIcon).getSize();
                float iconSize = 48.0f;
                float scale = iconSize / textureSize.x;
                muteSprite.setScale(sf::Vector2f(scale, scale));
                muteSprite.setPosition(sf::Vector2f(1920.0f - iconSize - 20.0f, 20.0f));
                window.draw(muteSprite);
            } else if (fontLoaded) {
                sf::Text muteText(menuFont);
                muteText.setString("MUTED");
                muteText.setCharacterSize(20);
                muteText.setFillColor(sf::Color::Red);
                muteText.setPosition(sf::Vector2f(1920.0f - 100.0f, 20.0f));
                window.draw(muteText);
            }
        }
        
        if (showVolumeIndicator && fontLoaded) {
            float displayVolume = isMuted ? lastMasterVolume : masterVolume;
            std::string volumeStr = "Volume: " + std::to_string(static_cast<int>(displayVolume)) + "%";
            
            sf::Text volumeText(menuFont);
            volumeText.setString(volumeStr);
            volumeText.setCharacterSize(32);
            volumeText.setFillColor(sf::Color(255, 255, 255, 255));
            volumeText.setOutlineColor(sf::Color::Black);
            volumeText.setOutlineThickness(2);
            volumeText.setStyle(sf::Text::Bold);
            
            sf::FloatRect textBounds = volumeText.getLocalBounds();
            float textWidth = textBounds.size.x;
            

            float barWidth = 200.0f;
            float barHeight = 10.0f;
            

            float boxWidth = std::max(textWidth + 20.0f, barWidth + 20.0f);
            float boxHeight = 55.0f;
            
            float posX = 1920.0f - boxWidth - 20.0f;
            float posY = isMuted ? 80.0f : 20.0f;
            

            sf::RectangleShape volumeBg;
            volumeBg.setSize(sf::Vector2f(boxWidth, boxHeight));
            volumeBg.setPosition(sf::Vector2f(posX, posY));
            volumeBg.setFillColor(sf::Color(0, 0, 0, 180));
            volumeBg.setOutlineColor(sf::Color(100, 150, 255, 255));
            volumeBg.setOutlineThickness(2);
            window.draw(volumeBg);
            

            volumeText.setPosition(sf::Vector2f(posX + (boxWidth - textWidth) / 2.0f, posY + 5.0f));
            window.draw(volumeText);
            

            float barPosX = posX + (boxWidth - barWidth) / 2.0f;
            float barPosY = posY + 38.0f;
            
            sf::RectangleShape volumeBarBg;
            volumeBarBg.setSize(sf::Vector2f(barWidth, barHeight));
            volumeBarBg.setPosition(sf::Vector2f(barPosX, barPosY));
            volumeBarBg.setFillColor(sf::Color(50, 50, 50, 200));
            volumeBarBg.setOutlineColor(sf::Color(100, 100, 100, 255));
            volumeBarBg.setOutlineThickness(1);
            window.draw(volumeBarBg);
            
            float fillWidth = (displayVolume / 100.0f) * barWidth;
            sf::RectangleShape volumeBarFill;
            volumeBarFill.setSize(sf::Vector2f(fillWidth, barHeight));
            volumeBarFill.setPosition(sf::Vector2f(barPosX, barPosY));
            
            sf::Color barColor;
            if (displayVolume >= 70.0f) {
                barColor = sf::Color(100, 255, 100);
            } else if (displayVolume >= 30.0f) {
                barColor = sf::Color(255, 255, 100);
            } else {
                barColor = sf::Color(255, 100, 100);
            }
            volumeBarFill.setFillColor(barColor);
            window.draw(volumeBarFill);
        }
        
        }
        
        window.display();
    }
    return 0;
}

