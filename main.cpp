#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <memory>
#include <map>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>

enum class PieceType;
struct PieceShape;
enum class TextureType;
PieceShape getPieceShape(PieceType type);
TextureType getTextureType(PieceType pieceType);

const int GRID_WIDTH = 11;
const int GRID_HEIGHT = 22;
const float CELL_SIZE = 32.0f;
const float BORDER_WIDTH = 16.0f;
const float GRID_OFFSET_X = (1920 - GRID_WIDTH * CELL_SIZE) / 2.0f;
const float GRID_OFFSET_Y = (1080 - GRID_HEIGHT * CELL_SIZE) / 2.0f;

const int LEVEL_THRESHOLDS[] = {0, 9, 25, 49, 81, 121};
const int MAX_LEVEL = 5;

struct SaveData {
    int highScore = 0;
    int bestLines = 0;
    int bestLevel = 0;
    float masterVolume = 80.0f;
    bool isMuted = false;
    
    struct ScoreEntry {
        int score = 0;
        int lines = 0;
        int level = 0;
    };
    ScoreEntry topScores[3];
};

std::string getSaveFilePath() {
    
    std::filesystem::path documentsPath;
    
#ifdef _WIN32
    
    char* appData = std::getenv("APPDATA");
    if (appData) {
        documentsPath = std::filesystem::path(appData) / "Jigtriz";
    } else {
        documentsPath = std::filesystem::current_path() / "Jigtriz_Saves";
    }
#else
    
    char* homePath = std::getenv("HOME");
    if (homePath) {
        documentsPath = std::filesystem::path(homePath) / ".jigz";
    } else {
        documentsPath = std::filesystem::current_path() / "Jigtriz_Saves";
    }
#endif
    
    
    std::filesystem::path gameFolder = documentsPath;
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
        file << "BEST_LINES=" << data.bestLines << std::endl;
        file << "BEST_LEVEL=" << data.bestLevel << std::endl;
        file << "MASTER_VOLUME=" << data.masterVolume << std::endl;
        file << "IS_MUTED=" << (data.isMuted ? 1 : 0) << std::endl;
        
        for (int i = 0; i < 3; i++) {
            file << "TOP" << (i+1) << "_SCORE=" << data.topScores[i].score << std::endl;
            file << "TOP" << (i+1) << "_LINES=" << data.topScores[i].lines << std::endl;
            file << "TOP" << (i+1) << "_LEVEL=" << data.topScores[i].level << std::endl;
        }
        
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
                } else if (key == "BEST_LINES") {
                    data.bestLines = std::stoi(value);
                } else if (key == "BEST_LEVEL") {
                    data.bestLevel = std::stoi(value);
                } else if (key == "MASTER_VOLUME") {
                    data.masterVolume = std::stof(value);
                } else if (key == "IS_MUTED") {
                    data.isMuted = (std::stoi(value) == 1);
                } else if (key.find("TOP") == 0) {
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
        file.close();
        std::cout << "Game data loaded from: " << filePath << std::endl;
        std::cout << "High Score: " << data.highScore << " | Best Lines: " << data.bestLines << " | Best Level: " << data.bestLevel << std::endl;
    } else {
        std::cout << "No save file found, using default values" << std::endl;
    }
    
    return data;
}

enum class DifficultyLevel {
    Basic = 0,
    Medium = 1,
    Hard = 2
};

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

enum class PieceType {
    I_Basic,
    T_Basic,
    L_Basic,
    J_Basic,
    O_Basic,
    S_Basic,
    Z_Basic,

    I_Medium,
    T_Medium,
    L_Medium,
    J_Medium,
    O_Medium,
    S_Medium,
    Z_Medium,

    I_Hard,
    T_Hard,
    L_Hard,
    J_Hard,
    O_Hard,
    S_Hard,
    Z_Hard,

    A_Bomb,
    A_Fill,
    A_Drill
};

enum class AbilityType {
    None,
    Bomb,
    Fill,
    Drill
};

AbilityType getAbilityType(PieceType type) {
    switch(type) {
        case PieceType::A_Bomb: return AbilityType::Bomb;
        case PieceType::A_Fill: return AbilityType::Fill;
        case PieceType::A_Drill: return AbilityType::Drill;
        default: return AbilityType::None;
    }
}

enum class TextureType {
    Empty,
    I_Basic,
    T_Basic,
    L_Basic,
    J_Basic,
    O_Basic,
    S_Basic,
    Z_Basic,

    I_Medium,
    T_Medium,
    L_Medium,
    J_Medium,
    O_Medium,
    S_Medium,
    Z_Medium,

    I_Hard,
    T_Hard,
    L_Hard,
    J_Hard,
    O_Hard,
    S_Hard,
    Z_Hard,

    A_Bomb,
    A_Fill,
    A_Drill,

    Digit0,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    Digit5,
    Digit6,
    Digit7,
    Digit8,
    Digit9
};

struct Cell {
    bool occupied = false;
    sf::Color color = sf::Color::Transparent;
    TextureType textureType = TextureType::Empty;
    Cell() = default;
    Cell(const sf::Color& col, TextureType texType = TextureType::Empty) : occupied(true), color(col), textureType(texType) {}
    sf::Color getColor() const { return color; }
    TextureType getTextureType() const { return textureType; }
};

struct PieceShape {
    std::vector<std::vector<bool>> blocks;
    sf::Color color;
    int width, height;
};

class PieceBag {
private:
    std::vector<PieceType> currentBag;
    std::vector<PieceType> nextBag;
    std::vector<PieceType> nextQueue;
    int bagIndex = 0;
    std::mt19937 rng;
    bool nextBagReady = false;
    int currentLevel = 0;
    
    std::vector<PieceType> createNewBag(int level) {
        std::cout << "Creating bag for level: " << level << std::endl;
        std::vector<PieceType> newBag;
        
        std::vector<PieceType> basicTypes = {
            PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
            PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic,
        };
        
        std::vector<PieceType> mediumTypes = {
            PieceType::I_Medium, PieceType::T_Medium, PieceType::L_Medium, 
            PieceType::J_Medium, PieceType::O_Medium, PieceType::S_Medium, PieceType::Z_Medium
        };
        
        std::vector<PieceType> hardTypes = {
            PieceType::I_Hard, PieceType::T_Hard, PieceType::L_Hard, 
            PieceType::J_Hard, PieceType::O_Hard, PieceType::S_Hard, PieceType::Z_Hard
        };
        
        if (level < 1) {
            std::cout << "Level 0 detected - using only basic pieces" << std::endl;
            newBag = basicTypes;
        } else if (level == 1) {
            newBag = basicTypes;
            newBag.push_back(mediumTypes[rng() % mediumTypes.size()]);
        } else if (level == 2) {
            newBag = basicTypes;
            newBag.push_back(mediumTypes[rng() % mediumTypes.size()]);
            newBag.push_back(hardTypes[rng() % hardTypes.size()]);
        } else if (level == 3) {
            newBag = basicTypes;
            for (int i = 0; i < 2; i++) newBag.push_back(mediumTypes[rng() % mediumTypes.size()]);
            newBag.push_back(hardTypes[rng() % hardTypes.size()]);
        } else {
            newBag = basicTypes;
            for (int i = 0; i < 2; i++) newBag.push_back(mediumTypes[rng() % mediumTypes.size()]);
            for (int i = 0; i < 2; i++) newBag.push_back(hardTypes[rng() % hardTypes.size()]);
        }
        
        std::shuffle(newBag.begin(), newBag.end(), rng);
        return newBag;
    }
    
    void ensureNextBagReady() {
        if (!nextBagReady) {
            nextBag = createNewBag(currentLevel);
            nextBagReady = true;
        }
    }
    
    void fillNextQueue() {
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
    
public:
    PieceBag() : rng(std::random_device{}()), currentLevel(0) {
        std::cout << "PieceBag constructor: currentLevel = " << currentLevel << std::endl;
        currentBag = createNewBag(currentLevel);
        bagIndex = 0;
        fillNextQueue();
    }
    
    PieceType getNextPiece() {
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
    
    void updateLevel(int newLevel) {
        if (newLevel != currentLevel) {
            currentLevel = newLevel;
            std::cout << "Bag difficulty updated to level " << currentLevel << "!" << std::endl;
        }
    }
    
    const std::vector<PieceType>& getNextQueue() const {
        return nextQueue;
    }
    
    const std::vector<PieceType>& getCurrentBag() const {
        return currentBag;
    }
    
    const std::vector<PieceType>& getNextBag() const {
        return nextBag;
    }
    
    int getBagIndex() const {
        return bagIndex;
    }
    
    bool isNextBagReady() const {
        return nextBagReady;
    }
    
    void reset() {
        currentLevel = 0;
        bagIndex = 0;
        nextBagReady = false;
        currentBag = createNewBag(currentLevel);
        nextBag.clear();
        nextQueue.clear();
        fillNextQueue();
        std::cout << "Bag system reset to level 0!" << std::endl;
    }
};

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
    panelBg.setSize(sf::Vector2f(120, 100));
    window.draw(panelBg);
    
    if (fontLoaded) {
        sf::Text titleText(font, "BOMB");
        titleText.setCharacterSize(12);
        titleText.setFillColor(isAvailable ? sf::Color(255, 100, 100) : sf::Color(100, 100, 100));
        titleText.setPosition(sf::Vector2f(panelX + 35, panelY + 5));
        window.draw(titleText);
    } else {
        sf::RectangleShape titleLabel;
        titleLabel.setFillColor(isAvailable ? sf::Color(255, 100, 100) : sf::Color(100, 100, 100));
        titleLabel.setSize(sf::Vector2f(60, 8));
        titleLabel.setPosition(sf::Vector2f(panelX + 20, panelY + 5));
        window.draw(titleLabel);
    }
    
    if (isAvailable) {
        PieceShape bombShape = getPieceShape(PieceType::A_Bomb);
        TextureType texType = TextureType::A_Bomb;
        
        float centerX = panelX + 50;
        float centerY = panelY + 40;
        float miniCellSize = 20.0f;
        
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
            readyText.setCharacterSize(10);
            readyText.setFillColor(sf::Color(100, 255, 100));
            readyText.setPosition(sf::Vector2f(panelX + 25, panelY + 65));
            window.draw(readyText);
            
            sf::Text keyText(font, "Press 'I'");
            keyText.setCharacterSize(8);
            keyText.setFillColor(sf::Color(200, 200, 200));
            keyText.setPosition(sf::Vector2f(panelX + 20, panelY + 78));
            window.draw(keyText);
        }
    } else {
        int linesNeeded = 10 - linesSinceLastAbility;
        
        if (fontLoaded) {
            sf::Text progressText(font, std::to_string(linesSinceLastAbility) + " / 10");
            progressText.setCharacterSize(14);
            progressText.setFillColor(sf::Color(200, 200, 200));
            progressText.setPosition(sf::Vector2f(panelX + 25, panelY + 35));
            window.draw(progressText);
            
            sf::Text needText(font, std::to_string(linesNeeded) + " more");
            needText.setCharacterSize(10);
            needText.setFillColor(sf::Color(150, 150, 150));
            needText.setPosition(sf::Vector2f(panelX + 22, panelY + 55));
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

void drawHeldPiece(sf::RenderWindow& window, PieceType heldType, bool hasHeld, const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y + 200;

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(30, 30, 40, 200));
    panelBg.setOutlineColor(sf::Color(255, 150, 100, 255));
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(120, 80));
    window.draw(panelBg);
    
    sf::RectangleShape holdLabel;
    holdLabel.setFillColor(sf::Color(255, 150, 100));
    holdLabel.setSize(sf::Vector2f(80, 8));
    holdLabel.setPosition(sf::Vector2f(panelX + 10, panelY + 5));
    window.draw(holdLabel);
    
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
                    
                    if (useTextures && textures.find(texType) != textures.end()) {
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

void drawLevelInfo(sf::RenderWindow& window, int totalLinesCleared, int currentLevel, int totalScore, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y + 50;
    

    sf::RectangleShape bg;
    bg.setFillColor(sf::Color(30, 30, 40, 200));
    bg.setOutlineColor(sf::Color(255, 200, 100, 255));
    bg.setOutlineThickness(2);
    bg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    bg.setSize(sf::Vector2f(120, 120));
    window.draw(bg);
    
    auto getDigitTexture = [](int digit) -> TextureType {
        switch(digit) {
            case 0: return TextureType::Digit0;
            case 1: return TextureType::Digit1;
            case 2: return TextureType::Digit2;
            case 3: return TextureType::Digit3;
            case 4: return TextureType::Digit4;
            case 5: return TextureType::Digit5;
            case 6: return TextureType::Digit6;
            case 7: return TextureType::Digit7;
            case 8: return TextureType::Digit8;
            case 9: return TextureType::Digit9;
            default: return TextureType::Digit0;
        }
    };
    
    float digitSize = 16.0f;
    
    if (fontLoaded) {
        sf::Text scoreText(font, "SCORE");
        scoreText.setCharacterSize(12);
        scoreText.setFillColor(sf::Color::Yellow);
        scoreText.setPosition(sf::Vector2f(panelX + 10, panelY + 5));
        window.draw(scoreText);
    } else {
        sf::RectangleShape scoreLabel;
        scoreLabel.setFillColor(sf::Color::Yellow);
        scoreLabel.setSize(sf::Vector2f(80, 6));
        scoreLabel.setPosition(sf::Vector2f(panelX + 10, panelY + 5));
        window.draw(scoreLabel);
    }
    
    int tempScore = totalScore;
    if (tempScore == 0) {
        TextureType digitTex = getDigitTexture(0);
        if (useTextures && textures.find(digitTex) != textures.end()) {
            sf::Sprite digitSprite(textures.at(digitTex));
            digitSprite.setPosition(sf::Vector2f(panelX + 50, panelY + 15));
            sf::Vector2u textureSize = textures.at(digitTex).getSize();
            digitSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
            window.draw(digitSprite);
        } else {
            sf::RectangleShape digit;
            digit.setFillColor(sf::Color::White);
            digit.setSize(sf::Vector2f(digitSize, digitSize));
            digit.setPosition(sf::Vector2f(panelX + 50, panelY + 15));
            window.draw(digit);
        }
    } else {
        std::vector<int> digits;
        while (tempScore > 0) {
            digits.push_back(tempScore % 10);
            tempScore /= 10;
        }
        
        for (int i = digits.size() - 1; i >= 0; i--) {
            float digitX = panelX + 15 + (digits.size() - 1 - i) * (digitSize + 1);
            TextureType digitTex = getDigitTexture(digits[i]);
            
            if (useTextures && textures.find(digitTex) != textures.end()) {
                sf::Sprite digitSprite(textures.at(digitTex));
                digitSprite.setPosition(sf::Vector2f(digitX, panelY + 15));
                sf::Vector2u textureSize = textures.at(digitTex).getSize();
                digitSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
                window.draw(digitSprite);
            } else {
                sf::RectangleShape digit;
                digit.setFillColor(sf::Color::White);
                digit.setSize(sf::Vector2f(digitSize, digitSize));
                digit.setPosition(sf::Vector2f(digitX, panelY + 15));
                window.draw(digit);
            }
        }
    }
    
    if (fontLoaded) {
        sf::Text linesText(font, "LINES");
        linesText.setCharacterSize(12);
        linesText.setFillColor(sf::Color::Green);
        linesText.setPosition(sf::Vector2f(panelX + 10, panelY + 45));
        window.draw(linesText);
    } else {
        sf::RectangleShape linesLabel;
        linesLabel.setFillColor(sf::Color::Green);
        linesLabel.setSize(sf::Vector2f(80, 6));
        linesLabel.setPosition(sf::Vector2f(panelX + 10, panelY + 45));
        window.draw(linesLabel);
    }
    
    int tempLines = totalLinesCleared;
    if (tempLines == 0) {
        TextureType digitTex = getDigitTexture(0);
        if (useTextures && textures.find(digitTex) != textures.end()) {
            sf::Sprite digitSprite(textures.at(digitTex));
            digitSprite.setPosition(sf::Vector2f(panelX + 50, panelY + 55));
            sf::Vector2u textureSize = textures.at(digitTex).getSize();
            digitSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
            window.draw(digitSprite);
        } else {
            sf::RectangleShape digit;
            digit.setFillColor(sf::Color::White);
            digit.setSize(sf::Vector2f(digitSize, digitSize));
            digit.setPosition(sf::Vector2f(panelX + 50, panelY + 55));
            window.draw(digit);
        }
    } else {
        std::vector<int> digits;
        while (tempLines > 0) {
            digits.push_back(tempLines % 10);
            tempLines /= 10;
        }
        
        for (int i = digits.size() - 1; i >= 0; i--) {
            float digitX = panelX + 15 + (digits.size() - 1 - i) * (digitSize + 1);
            TextureType digitTex = getDigitTexture(digits[i]);
            
            if (useTextures && textures.find(digitTex) != textures.end()) {
                sf::Sprite digitSprite(textures.at(digitTex));
                digitSprite.setPosition(sf::Vector2f(digitX, panelY + 55));
                sf::Vector2u textureSize = textures.at(digitTex).getSize();
                digitSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
                window.draw(digitSprite);
            } else {
                sf::RectangleShape digit;
                digit.setFillColor(sf::Color::White);
                digit.setSize(sf::Vector2f(digitSize, digitSize));
                digit.setPosition(sf::Vector2f(digitX, panelY + 55));
                window.draw(digit);
            }
        }
    }
    
    if (fontLoaded) {
        sf::Text levelText(font, "Level");
        levelText.setCharacterSize(12);
        levelText.setFillColor(sf::Color::Red);
        levelText.setPosition(sf::Vector2f(panelX + 10, panelY + 85));
        window.draw(levelText);
    } else {
        sf::RectangleShape levelLabel;
        levelLabel.setFillColor(sf::Color::Red);
        levelLabel.setSize(sf::Vector2f(80, 6));
        levelLabel.setPosition(sf::Vector2f(panelX + 10, panelY + 85));
        window.draw(levelLabel);
    }
    
    TextureType levelTex = getDigitTexture(currentLevel);
    if (useTextures && textures.find(levelTex) != textures.end()) {
        sf::Sprite levelSprite(textures.at(levelTex));
        levelSprite.setPosition(sf::Vector2f(panelX + 50, panelY + 95));
        sf::Vector2u textureSize = textures.at(levelTex).getSize();
        levelSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
        window.draw(levelSprite);
    } else {
        sf::RectangleShape levelDigit;
        levelDigit.setFillColor(sf::Color::Cyan);
        levelDigit.setSize(sf::Vector2f(digitSize, digitSize));
        levelDigit.setPosition(sf::Vector2f(panelX + 50, panelY + 95));
        window.draw(levelDigit);
    }
}

PieceShape getPieceShape(PieceType type) {
    PieceShape shape;
    switch(type){
        case PieceType::I_Basic:
            shape.blocks = {{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color::Cyan;
            break;
        case PieceType::I_Medium:
            shape.blocks = {{true, true, true, true, true},{false, false, false, false, false}};
            shape.color = sf::Color(0, 200, 255);
            break;
        case PieceType::I_Hard:
            shape.blocks = {{true, true, true, true, true},{false, false, true, false, false}};
            shape.color = sf::Color(0, 150, 255);
            break;
            
        case PieceType::T_Basic:
            shape.blocks = {{false, true, false},{true, true, true},{false, false, false}};
            shape.color = sf::Color::Magenta;
            break;
        case PieceType::T_Medium:
            shape.blocks = {{false, true, false},{false, true, false},{true, true, true}};
            shape.color = sf::Color(255, 100, 255);
            break;
        case PieceType::T_Hard:
            shape.blocks = {{false, true, false},{false, true, false},{true, true, true},{false, true, false},{false, false, false}};
            shape.color = sf::Color(200, 50, 200);
            break;

        case PieceType::L_Basic:
            shape.blocks = {{true, false, false},{true, true, true},{false, false, false}};
            shape.color = sf::Color(255, 165, 0);
            break;
        case PieceType::L_Medium:
            shape.blocks = {{false, false, false, false},{true, false, false, false},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(255, 200, 50);
            break;
        case PieceType::L_Hard:
            shape.blocks = {{false, false, false, false},{true, true, false, false},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(200, 120, 0);
            break;

        case PieceType::J_Basic:
            shape.blocks = {{false, false, true},{true, true, true},{false, false, false}};
            shape.color = sf::Color::Blue;
            break;
        case PieceType::J_Medium:
            shape.blocks = {{false, false, false, true},{true, true, true, true}};
            shape.color = sf::Color(100, 100, 255);
            break;
        case PieceType::J_Hard:
            shape.blocks = {{false, false, false, false},{false, false, true, true},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(0, 0, 200);
            break;

        case PieceType::O_Basic:
            shape.blocks = {{true, true},{true, true}};
            shape.color = sf::Color::Yellow;
            break;
        case PieceType::O_Medium:
            shape.blocks = {{false, false, false},{true, true, true},{true, true, true}};
            shape.color = sf::Color(255, 255, 100);
            break;
        case PieceType::O_Hard:
            shape.blocks = {{false, true, true},{true, true, true},{true, true, false}};
            shape.color = sf::Color(200, 200, 0);
            break;

        case PieceType::S_Basic:
            shape.blocks = {{false, true, true},{true, true, false}};
            shape.color = sf::Color::Green;
            break;
        case PieceType::S_Medium:
            shape.blocks = {{false, true, false},{false, true, true},{true, true, false}};
            shape.color = sf::Color(100, 255, 100);
            break;
        case PieceType::S_Hard:
            shape.blocks = {{false, true, false},{false, true, true},{true, true, false},{false, true, false}};
            shape.color = sf::Color(0, 200, 0);
            break;

        case PieceType::Z_Basic:
            shape.blocks = {{true, true, false},{false, true, true}};
            shape.color = sf::Color::Red;
            break;
        case PieceType::Z_Medium:
            shape.blocks = {{false, true, false},{true, true, false},{false, true, true}};
            shape.color = sf::Color(255, 100, 100);
            break;
        case PieceType::Z_Hard:
            shape.blocks = {{false, true, false},{true, true, false},{false, true, true},{false, true, false}};
            shape.color = sf::Color(200, 0, 0);
            break;

        case PieceType::A_Bomb:
            shape.blocks = {{true}};
            shape.color = sf::Color(255, 255, 255);
            break;
        case PieceType::A_Drill:
            shape.blocks = {{true}};
            shape.color = sf::Color(128, 0, 128);
            break;
    }
    shape.height = static_cast<int>(shape.blocks.size());
    shape.width = (shape.height > 0) ? static_cast<int>(shape.blocks[0].size()) : 0;
    return shape;
}

TextureType getTextureType(PieceType pieceType) {
    switch(pieceType) {
        case PieceType::I_Basic: return TextureType::I_Basic;
        case PieceType::T_Basic: return TextureType::T_Basic;
        case PieceType::L_Basic: return TextureType::L_Basic;
        case PieceType::J_Basic: return TextureType::J_Basic;
        case PieceType::O_Basic: return TextureType::O_Basic;
        case PieceType::S_Basic: return TextureType::S_Basic;
        case PieceType::Z_Basic: return TextureType::Z_Basic;

        case PieceType::I_Medium: return TextureType::I_Medium;
        case PieceType::T_Medium: return TextureType::T_Medium;
        case PieceType::L_Medium: return TextureType::L_Medium;
        case PieceType::J_Medium: return TextureType::J_Medium;
        case PieceType::O_Medium: return TextureType::O_Medium;
        case PieceType::S_Medium: return TextureType::S_Medium;
        case PieceType::Z_Medium: return TextureType::Z_Medium;

        case PieceType::I_Hard: return TextureType::I_Hard;
        case PieceType::T_Hard: return TextureType::T_Hard;
        case PieceType::L_Hard: return TextureType::L_Hard;
        case PieceType::J_Hard: return TextureType::J_Hard;
        case PieceType::O_Hard: return TextureType::O_Hard;
        case PieceType::S_Hard: return TextureType::S_Hard;
        case PieceType::Z_Hard: return TextureType::Z_Hard;
        
        case PieceType::A_Bomb: return TextureType::A_Bomb;
        case PieceType::A_Drill: return TextureType::A_Drill;
        case PieceType::A_Fill: return TextureType::A_Fill;

        default: return TextureType::Empty;
    }
}

std::string pieceTypeToString(PieceType type) {
    switch(type) {
        case PieceType::I_Basic: return "I";
        case PieceType::T_Basic: return "T";
        case PieceType::L_Basic: return "L";
        case PieceType::J_Basic: return "J";
        case PieceType::O_Basic: return "O";
        case PieceType::S_Basic: return "S";
        case PieceType::Z_Basic: return "Z";

        case PieceType::I_Medium: return "I++";
        case PieceType::T_Medium: return "T++";
        case PieceType::L_Medium: return "L++";
        case PieceType::J_Medium: return "J++";
        case PieceType::O_Medium: return "O++";
        case PieceType::S_Medium: return "S++";
        case PieceType::Z_Medium: return "Z++";

        case PieceType::I_Hard: return "I#";
        case PieceType::T_Hard: return "T#";
        case PieceType::L_Hard: return "L#";
        case PieceType::J_Hard: return "J#";
        case PieceType::O_Hard: return "O#";
        case PieceType::S_Hard: return "S#";
        case PieceType::Z_Hard: return "Z#";
        
        default: return "Unknown";
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
                    
                    if (useTextures && textures.find(texType) != textures.end()) {
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

            if (fallTimer >= (fastFall ? 0.1f : 0.5f)) {
                y++;
                fallTimer = 0.0f;
                updatePosition();
            }
        }
    }
    void ChangeToStatic(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, AbilityType ability = AbilityType::None, std::unique_ptr<sf::Sound>* bombSound = nullptr) {
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
            AbilityEffect(grid, bombSound);
        }
    }
    void BombEffect(int centerX, int centerY, std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, std::unique_ptr<sf::Sound>& bombSound) {
        if (bombSound) {
            std::cout << "Playing bomb sound! Volume: " << bombSound->getVolume() << std::endl;
            bombSound->play();
            std::cout << "Bomb sound status: " << static_cast<int>(bombSound->getStatus()) << std::endl;
        } else {
            std::cout << "ERROR: bombSound is null!" << std::endl;
        }
        
        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -2; dx <= 2; ++dx) {
                int x = centerX + dx;
                int y = centerY + dy;
                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                    grid[y][x] = Cell();
                }
            }
        }
    }
    void AbilityEffect(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, std::unique_ptr<sf::Sound>* bombSound = nullptr)
    {
        switch(ability)
        {
            case AbilityType::Bomb:
                if (bombSound) {
                    BombEffect(x, y, grid, *bombSound);
                } else {
                    std::cout << "Bomb ability activated (no sound)!" << std::endl;
                }
                std::cout << "Bomb ability activated!" << std::endl;
                break;
            case AbilityType::Drill:
                std::cout << "Drill ability activated!" << std::endl;
                break;
            default:
                break;
        }
    }
    bool hasStopped() const { return isStatic; }
    void makeStatic() { isStatic = true; }
    void draw(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, bool useTextures) const {
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float worldX = GRID_OFFSET_X + (x + j) * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + (y + i) * CELL_SIZE;
                    if (useTextures && textures.find(getTextureType(type)) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(getTextureType(type)));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        sf::Vector2u textureSize = textures.at(getTextureType(type)).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                        window.draw(cellSprite);
                    } else {
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
    void moveGround(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) {
        if (isStatic) return;
        while (!collidesAt(grid, x, y + 1)) {
            ++y;
        }
        isStatic = true;
        ChangeToStatic(grid, ability);
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
        
        int ghostY = getGhostY(grid);
        if (ghostY == y) return;
        
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float worldX = GRID_OFFSET_X + (x + j) * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + (ghostY + i) * CELL_SIZE;
                    
                    if (useTextures && textures.find(getTextureType(type)) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(getTextureType(type)));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        sf::Vector2u textureSize = textures.at(getTextureType(type)).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                        cellSprite.setColor(sf::Color(255, 255, 255, 80));
                        window.draw(cellSprite);
                    } else {
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

int clearFullLines(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid, std::unique_ptr<sf::Sound>& laserSound, std::vector<std::unique_ptr<sf::Sound>>& wowSounds) {
    int linesCleared = 0;
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

void drawGameOver(sf::RenderWindow& window, int finalScore, int finalLines, int finalLevel, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, const SaveData& saveData) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    sf::RectangleShape gameOverBg;
    gameOverBg.setFillColor(sf::Color(40, 40, 50, 220));
    gameOverBg.setOutlineColor(sf::Color(255, 100, 100, 255));
    gameOverBg.setOutlineThickness(4);
    gameOverBg.setSize(sf::Vector2f(500, 400));
    gameOverBg.setPosition(sf::Vector2f(centerX - 250, centerY - 200));
    window.draw(gameOverBg);
    
    if (fontLoaded) {
        sf::Text gameOverText(font, "GAME OVER");
        gameOverText.setCharacterSize(28);
        gameOverText.setFillColor(sf::Color(255, 100, 100));
        gameOverText.setStyle(sf::Text::Bold);
        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setPosition(sf::Vector2f(centerX - textBounds.size.x/2, centerY - 170));
        window.draw(gameOverText);
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(255, 100, 100));
        titleBar.setSize(sf::Vector2f(300, 40));
        titleBar.setPosition(sf::Vector2f(centerX - 150, centerY - 170));
        window.draw(titleBar);
    }
    
    bool newHighScore = finalScore > saveData.highScore;
    bool newBestLines = finalLines > saveData.bestLines;
    bool newBestLevel = finalLevel > saveData.bestLevel;
    
    if (newHighScore && fontLoaded) {
        sf::Text newRecordText(font, "NEW HIGH SCORE!");
        newRecordText.setCharacterSize(16);
        newRecordText.setFillColor(sf::Color(255, 215, 0));
        newRecordText.setStyle(sf::Text::Bold);
        sf::FloatRect textBounds = newRecordText.getLocalBounds();
        newRecordText.setPosition(sf::Vector2f(centerX - textBounds.size.x/2, centerY - 140));
        window.draw(newRecordText);
    }
    
    auto getDigitTexture = [](int digit) -> TextureType {
        switch(digit) {
            case 0: return TextureType::Digit0;
            case 1: return TextureType::Digit1;
            case 2: return TextureType::Digit2;
            case 3: return TextureType::Digit3;
            case 4: return TextureType::Digit4;
            case 5: return TextureType::Digit5;
            case 6: return TextureType::Digit6;
            case 7: return TextureType::Digit7;
            case 8: return TextureType::Digit8;
            case 9: return TextureType::Digit9;
            default: return TextureType::Digit0;
        }
    };
    
    float digitSize = 24.0f;
    
    if (fontLoaded) {
        sf::Text scoreText(font, "SCORE");
        scoreText.setCharacterSize(16);
        scoreText.setFillColor(newHighScore ? sf::Color(255, 215, 0) : sf::Color::Yellow);
        scoreText.setPosition(sf::Vector2f(centerX - 75, centerY - 100));
        window.draw(scoreText);
    } else {
        sf::RectangleShape scoreLabel;
        scoreLabel.setFillColor(sf::Color::Yellow);
        scoreLabel.setSize(sf::Vector2f(150, 8));
        scoreLabel.setPosition(sf::Vector2f(centerX - 75, centerY - 100));
        window.draw(scoreLabel);
    }
    
    int tempScore = finalScore;
    if (tempScore == 0) {
        TextureType digitTex = getDigitTexture(0);
        if (useTextures && textures.find(digitTex) != textures.end()) {
            sf::Sprite digitSprite(textures.at(digitTex));
            digitSprite.setPosition(sf::Vector2f(centerX - digitSize/2, centerY - 80));
            sf::Vector2u textureSize = textures.at(digitTex).getSize();
            digitSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
            window.draw(digitSprite);
        } else {
            sf::RectangleShape digit;
            digit.setFillColor(sf::Color::White);
            digit.setSize(sf::Vector2f(digitSize, digitSize));
            digit.setPosition(sf::Vector2f(centerX - digitSize/2, centerY - 80));
            window.draw(digit);
        }
    } else {
        std::vector<int> digits;
        while (tempScore > 0) {
            digits.push_back(tempScore % 10);
            tempScore /= 10;
        }
        
        float totalWidth = digits.size() * (digitSize + 2);
        float startX = centerX - totalWidth / 2;
        
        for (int i = digits.size() - 1; i >= 0; i--) {
            float digitX = startX + (digits.size() - 1 - i) * (digitSize + 2);
            TextureType digitTex = getDigitTexture(digits[i]);
            
            if (useTextures && textures.find(digitTex) != textures.end()) {
                sf::Sprite digitSprite(textures.at(digitTex));
                digitSprite.setPosition(sf::Vector2f(digitX, centerY - 80));
                sf::Vector2u textureSize = textures.at(digitTex).getSize();
                digitSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
                window.draw(digitSprite);
            } else {
                sf::RectangleShape digit;
                digit.setFillColor(sf::Color::White);
                digit.setSize(sf::Vector2f(digitSize, digitSize));
                digit.setPosition(sf::Vector2f(digitX, centerY - 80));
                window.draw(digit);
            }
        }
    }
    
    if (fontLoaded) {
        sf::Text linesText(font, "LINES");
        linesText.setCharacterSize(16);
        linesText.setFillColor(sf::Color::Green);
        linesText.setPosition(sf::Vector2f(centerX - 75, centerY + 10));
        window.draw(linesText);
    } else {
        sf::RectangleShape linesLabel;
        linesLabel.setFillColor(sf::Color::Green);
        linesLabel.setSize(sf::Vector2f(150, 8));
        linesLabel.setPosition(sf::Vector2f(centerX - 75, centerY + 10));
        window.draw(linesLabel);
    }
    
    int tempLines = finalLines;
    if (tempLines == 0) {
        TextureType digitTex = getDigitTexture(0);
        if (useTextures && textures.find(digitTex) != textures.end()) {
            sf::Sprite digitSprite(textures.at(digitTex));
            digitSprite.setPosition(sf::Vector2f(centerX - digitSize/2, centerY + 25));
            sf::Vector2u textureSize = textures.at(digitTex).getSize();
            digitSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
            window.draw(digitSprite);
        } else {
            sf::RectangleShape digit;
            digit.setFillColor(sf::Color::White);
            digit.setSize(sf::Vector2f(digitSize, digitSize));
            digit.setPosition(sf::Vector2f(centerX - digitSize/2, centerY + 25));
            window.draw(digit);
        }
    } else {
        std::vector<int> digits;
        while (tempLines > 0) {
            digits.push_back(tempLines % 10);
            tempLines /= 10;
        }
        
        float totalWidth = digits.size() * (digitSize + 2);
        float startX = centerX - totalWidth / 2;
        
        for (int i = digits.size() - 1; i >= 0; i--) {
            float digitX = startX + (digits.size() - 1 - i) * (digitSize + 2);
            TextureType digitTex = getDigitTexture(digits[i]);
            
            if (useTextures && textures.find(digitTex) != textures.end()) {
                sf::Sprite digitSprite(textures.at(digitTex));
                digitSprite.setPosition(sf::Vector2f(digitX, centerY + 25));
                sf::Vector2u textureSize = textures.at(digitTex).getSize();
                digitSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
                window.draw(digitSprite);
            } else {
                sf::RectangleShape digit;
                digit.setFillColor(sf::Color::White);
                digit.setSize(sf::Vector2f(digitSize, digitSize));
                digit.setPosition(sf::Vector2f(digitX, centerY + 25));
                window.draw(digit);
            }
        }
    }
    
    if (fontLoaded) {
        sf::Text levelText(font, "Level");
        levelText.setCharacterSize(16);
        levelText.setFillColor(sf::Color::Red);
        levelText.setPosition(sf::Vector2f(centerX - 75, centerY + 60));
        window.draw(levelText);
    } else {
        sf::RectangleShape levelLabel;
        levelLabel.setFillColor(sf::Color::Red);
        levelLabel.setSize(sf::Vector2f(150, 8));
        levelLabel.setPosition(sf::Vector2f(centerX - 75, centerY + 60));
        window.draw(levelLabel);
    }
    
    TextureType levelTex = getDigitTexture(finalLevel);
    if (useTextures && textures.find(levelTex) != textures.end()) {
        sf::Sprite levelSprite(textures.at(levelTex));
        levelSprite.setPosition(sf::Vector2f(centerX - digitSize/2, centerY + 75));
        sf::Vector2u textureSize = textures.at(levelTex).getSize();
        levelSprite.setScale(sf::Vector2f(digitSize / textureSize.x, digitSize / textureSize.y));
        window.draw(levelSprite);
    } else {
        sf::RectangleShape levelDigit;
        levelDigit.setFillColor(sf::Color::Cyan);
        levelDigit.setSize(sf::Vector2f(digitSize, digitSize));
        levelDigit.setPosition(sf::Vector2f(centerX - digitSize/2, centerY + 75));
        window.draw(levelDigit);
    }
    
    if (fontLoaded) {
        sf::Text restartText(font, "Press R to restart");
        restartText.setCharacterSize(14);
        restartText.setFillColor(sf::Color(150, 150, 255));
        sf::FloatRect textBounds = restartText.getLocalBounds();
        restartText.setPosition(sf::Vector2f(centerX - textBounds.size.x/2, centerY + 160));
        window.draw(restartText);
        
        sf::Text topScoresTitle(font, "TOP 3 SCORES");
        topScoresTitle.setCharacterSize(14);
        topScoresTitle.setFillColor(sf::Color(100, 255, 150));
        topScoresTitle.setPosition(sf::Vector2f(centerX + 120, centerY - 100));
        window.draw(topScoresTitle);
        
        for (int i = 0; i < 3; i++) {
            if (saveData.topScores[i].score > 0) {
                sf::Color scoreColor = sf::Color::White;
                if (i == 0) scoreColor = sf::Color(255, 215, 0);
                else if (i == 1) scoreColor = sf::Color(192, 192, 192);
                else if (i == 2) scoreColor = sf::Color(205, 127, 50);
                
                sf::Text rankText(font, "#" + std::to_string(i + 1) + ": " + std::to_string(saveData.topScores[i].score));
                rankText.setCharacterSize(12);
                rankText.setFillColor(scoreColor);
                rankText.setPosition(sf::Vector2f(centerX + 120, centerY - 75 + i * 35));
                window.draw(rankText);
                
                sf::Text detailText(font, "L" + std::to_string(saveData.topScores[i].lines) + " Lv" + std::to_string(saveData.topScores[i].level));
                detailText.setCharacterSize(10);
                detailText.setFillColor(sf::Color(180, 180, 180));
                detailText.setPosition(sf::Vector2f(centerX + 130, centerY - 55 + i * 35));
                window.draw(detailText);
            } else {
                sf::Text emptyText(font, "#" + std::to_string(i + 1) + ": ---");
                emptyText.setCharacterSize(12);
                emptyText.setFillColor(sf::Color(100, 100, 100));
                emptyText.setPosition(sf::Vector2f(centerX + 120, centerY - 75 + i * 35));
                window.draw(emptyText);
            }
        }
    } else {
        sf::RectangleShape restartHint;
        restartHint.setFillColor(sf::Color(150, 150, 255));
        restartHint.setSize(sf::Vector2f(200, 20));
        restartHint.setPosition(sf::Vector2f(centerX - 100, centerY + 160));
        window.draw(restartHint);
    }
}

void drawJigtrizTitle(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded) {
    if (!fontLoaded) return;
    
    sf::Text titleText(font, "Jigtriz 0.1.1");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color(100, 255, 150));
    titleText.setStyle(sf::Text::Bold);
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(2);
    
    titleText.setPosition(sf::Vector2f(50, 1080 - 80));
    
    window.draw(titleText);
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

void insertNewScore(SaveData& saveData, int score, int lines, int level) {
    SaveData::ScoreEntry newEntry = {score, lines, level};
    
    int insertPos = -1;
    for (int i = 0; i < 3; i++) {
        if (score > saveData.topScores[i].score) {
            insertPos = i;
            break;
        }
    }
    
    if (insertPos >= 0) {
        for (int i = 2; i > insertPos; i--) {
            saveData.topScores[i] = saveData.topScores[i-1];
        }
        saveData.topScores[insertPos] = newEntry;
        
        std::cout << "NEW TOP SCORE #" << (insertPos + 1) << ": " << score << " points!" << std::endl;
        
        if (insertPos == 0) {
            saveData.highScore = score;
        }
        
        return;
    }
    
    if (score > saveData.highScore) {
        saveData.highScore = score;
    }
}

int main() {
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
    struct TextureInfo { TextureType type; std::string filename; sf::Color fallbackColor; };
    std::vector<TextureInfo> textureList = {
        {TextureType::Empty, "assets/cell_background.png", sf::Color(50, 50, 60)},
        

        {TextureType::I_Basic, "assets/piece_i.png", sf::Color::Cyan},
        {TextureType::T_Basic, "assets/piece_t.png", sf::Color::Magenta},
        {TextureType::L_Basic, "assets/piece_l.png", sf::Color(255, 165, 0)},
        {TextureType::J_Basic, "assets/piece_j.png", sf::Color::Blue},
        {TextureType::O_Basic, "assets/piece_o.png", sf::Color::Yellow},
        {TextureType::S_Basic, "assets/piece_s.png", sf::Color::Green},
        {TextureType::Z_Basic, "assets/piece_z.png", sf::Color::Red},
        

        {TextureType::I_Medium, "assets/piece_i++.png", sf::Color(0, 200, 255)},
        {TextureType::T_Medium, "assets/piece_t++.png", sf::Color(255, 100, 255)},
        {TextureType::L_Medium, "assets/piece_l++.png", sf::Color(255, 200, 50)},
        {TextureType::J_Medium, "assets/piece_j++.png", sf::Color(100, 100, 255)},
        {TextureType::O_Medium, "assets/piece_o++.png", sf::Color(255, 255, 100)},
        {TextureType::S_Medium, "assets/piece_s++.png", sf::Color(100, 255, 100)},
        {TextureType::Z_Medium, "assets/piece_z++.png", sf::Color(255, 100, 100)},
        

        {TextureType::I_Hard, "assets/piece_i#.png", sf::Color(0, 150, 255)},
        {TextureType::T_Hard, "assets/piece_t#.png", sf::Color(200, 50, 200)},
        {TextureType::L_Hard, "assets/piece_l#.png", sf::Color(200, 120, 0)},
        {TextureType::J_Hard, "assets/piece_j#.png", sf::Color(0, 0, 200)},
        {TextureType::O_Hard, "assets/piece_o#.png", sf::Color(200, 200, 0)},
        {TextureType::S_Hard, "assets/piece_s#.png", sf::Color(0, 200, 0)},
        {TextureType::Z_Hard, "assets/piece_z#.png", sf::Color(200, 0, 0)},
        

        {TextureType::A_Bomb, "assets/bomb.png", sf::Color(255, 100, 100)},
        {TextureType::A_Fill, "assets/fill.png", sf::Color(100, 255, 100)},
        {TextureType::A_Drill, "assets/drill.png", sf::Color(255, 255, 100)},
        

        {TextureType::Digit0, "assets/0.png", sf::Color::White},
        {TextureType::Digit1, "assets/1.png", sf::Color::White},
        {TextureType::Digit2, "assets/2.png", sf::Color::White},
        {TextureType::Digit3, "assets/3.png", sf::Color::White},
        {TextureType::Digit4, "assets/4.png", sf::Color::White},
        {TextureType::Digit5, "assets/5.png", sf::Color::White},
        {TextureType::Digit6, "assets/6.png", sf::Color::White},
        {TextureType::Digit7, "assets/7.png", sf::Color::White},
        {TextureType::Digit8, "assets/8.png", sf::Color::White},
        {TextureType::Digit9, "assets/9.png", sf::Color::White}
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
    
    sf::Font gameFont;
    bool fontLoaded = false;
    if (gameFont.openFromFile("assets/PixelifySans-VariableFont_wght.ttf")) {
        fontLoaded = true;
        std::cout << "Font loaded successfully: PixelifySans-VariableFont_wght.ttf" << std::endl;
    } else {
        std::cout << "Unable to load font: assets/PixelifySans-VariableFont_wght.ttf - text will not be displayed" << std::endl;
    }
    
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
    
    const float baseMusicVolume = 60.0f;
    const float baseSpaceVolume = 8.0f;
    const float baseLaserVolume = 8.0f;
    const float baseBombVolume = 100.0f;
    const float baseWowVolume = 70.0f;
    
    if (backgroundMusic.openFromFile("assets/Jigtriz.ogg")) {
        backgroundMusic.setLooping(true);
        backgroundMusic.setVolume((baseMusicVolume * masterVolume) / 100.0f);
        backgroundMusic.play();
        std::cout << "Background music loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load background music (assets/Jigtriz.ogg) - continuing without music." << std::endl;
    }
    if (spaceSoundBuffer.loadFromFile("assets/ground.ogg")) {
        spaceSound = std::make_unique<sf::Sound>(spaceSoundBuffer);
        spaceSound->setVolume((baseSpaceVolume * masterVolume) / 100.0f);
        std::cout << "Space sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load space sound (assets/ground.ogg) - continuing without sound." << std::endl;
    }
    if (laserSoundBuffer.loadFromFile("assets/laser.ogg")) {
        laserSound = std::make_unique<sf::Sound>(laserSoundBuffer);
        laserSound->setVolume((baseLaserVolume * masterVolume) / 100.0f);
        std::cout << "Laser sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load laser sound (assets/laser.ogg) - continuing without sound." << std::endl;
    }
    if (bombSoundBuffer.loadFromFile("assets/bomb.ogg")) {
        bombSound = std::make_unique<sf::Sound>(bombSoundBuffer);
        bombSound->setVolume((baseBombVolume * masterVolume) / 100.0f);
        std::cout << "Bomb sound loaded successfully! Master volume: " << masterVolume << ", Base volume: " << baseBombVolume << ", Final volume: " << bombSound->getVolume() << std::endl;
    } else {
        std::cout << "Unable to load bomb sound (assets/bomb.ogg) - continuing without sound." << std::endl;
    }

    wowSoundBuffers.resize(3);
    wowSounds.resize(3);
    for (int i = 0; i < 3; i++) {
        std::string filename = "assets/wow_0" + std::to_string(i + 1) + ".ogg";
        if (wowSoundBuffers[i].loadFromFile(filename)) {
            wowSounds[i] = std::make_unique<sf::Sound>(wowSoundBuffers[i]);
            wowSounds[i]->setVolume((baseWowVolume * masterVolume) / 100.0f);
            std::cout << "Wow sound loaded successfully: " << filename << std::endl;
        } else {
            std::cout << "Unable to load wow sound: " << filename << " - continuing without this sound." << std::endl;
        }
    }
    
    std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> grid;
    PieceBag tetrisBag;
    

    int totalLinesCleared = 0;
    int currentLevel = 0;
    int totalScore = 0;
    bool gameOver = false;
    
    int linesSinceLastAbility = 0;
    bool bombAbilityAvailable = false;
    const int LINES_FOR_ABILITY = 10;
    

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
    
    PieceType initType = tetrisBag.getNextPiece();
    std::cout << "Initial spawn: " << pieceTypeToString(initType) << " (Bag System)" << std::endl;
    PieceShape initShape = getPieceShape(initType);
    int startX = (GRID_WIDTH - initShape.width) / 2;
    Piece activePiece(startX, 0, initType);
    sf::Clock clock;
    bool firstFrame = true;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        if (firstFrame) { window.requestFocus(); firstFrame = false; }
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) { window.close(); }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Escape: window.close(); break;
                    case sf::Keyboard::Key::Up:
                    case sf::Keyboard::Key::W:
                    case sf::Keyboard::Key::J: if (!gameOver) activePiece.rotateRight(grid); break;
                    case sf::Keyboard::Key::K: if (!gameOver) activePiece.rotateLeft(grid); break;
                    case sf::Keyboard::Key::Tab:
                    case sf::Keyboard::Key::L: {
                        if (!canUseHold || gameOver) break;
                        
                        if (!hasHeldPiece) {

                            heldPiece = activePiece.getType();
                            hasHeldPiece = true;
                            
                            PieceType newType = tetrisBag.getNextPiece();
                            std::cout << "HOLD: Stored " << pieceTypeToString(heldPiece) << ", spawning " << pieceTypeToString(newType) << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            activePiece = Piece(spawnX, 0, newType);
                        } else {

                            PieceType currentType = activePiece.getType();
                            PieceType swapType = heldPiece;
                            heldPiece = currentType;
                            
                            std::cout << "SWAP: " << pieceTypeToString(currentType) << " <-> " << pieceTypeToString(swapType) << std::endl;
                            PieceShape swapShape = getPieceShape(swapType);
                            int spawnX = (GRID_WIDTH - swapShape.width) / 2;
                            activePiece = Piece(spawnX, 0, swapType);
                        }
                        
                        canUseHold = false;
                        break;
                    }
                    case sf::Keyboard::Key::Space: if (!gameOver) { activePiece.moveGround(grid); if (spaceSound) { spaceSound->play(); } } break;
                    case sf::Keyboard::Key::R: {

                        for (int i = 0; i < GRID_HEIGHT; ++i) {
                            for (int j = 0; j < GRID_WIDTH; ++j) {
                                grid[i][j] = Cell();
                            }
                        }
                        totalLinesCleared = 0;
                        currentLevel = 0;
                        totalScore = 0;
                        gameOver = false;
                        
                        tetrisBag.reset();
                        
                        hasHeldPiece = false;
                        canUseHold = true;

                        leftHoldTime = 0.0f;
                        rightHoldTime = 0.0f;
                        dasTimer = 0.0f;
                        leftPressed = false;
                        rightPressed = false;
                        
                        PieceType resetType = tetrisBag.getNextPiece();
                        std::cout << "Game Reset! New piece: " << pieceTypeToString(resetType) << " - Level reset!" << std::endl;
                        PieceShape resetShape = getPieceShape(resetType);
                        int resetX = (GRID_WIDTH - resetShape.width) / 2;
                        activePiece = Piece(resetX, 0, resetType);
                        
                        linesSinceLastAbility = 0;
                        bombAbilityAvailable = false;
                        
                        break;
                    }
                    case sf::Keyboard::Key::I: {
                        if (bombAbilityAvailable && !gameOver) {
                            std::cout << "BOMB ABILITY ACTIVATED!" << std::endl;
                            
                            PieceType newType = PieceType::A_Bomb;
                            std::cout << "Spawning bomb piece!" << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            activePiece = Piece(spawnX, 0, newType);
                            
                            bombAbilityAvailable = false;
                            linesSinceLastAbility = 0;
                        } else if (!bombAbilityAvailable && !gameOver) {
                            int linesNeeded = LINES_FOR_ABILITY - linesSinceLastAbility;
                            std::cout << "Bomb ability not ready yet! Need " << linesNeeded << " more lines." << std::endl;
                        }
                        break;
                    }
                    case sf::Keyboard::Key::M: {
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
                        saveData.masterVolume = isMuted ? lastMasterVolume : masterVolume;
                        saveData.isMuted = isMuted;
                        saveGameData(saveData);
                        break;
                    }
                    case sf::Keyboard::Key::Comma: {
                        if (!isMuted && masterVolume > 0.0f) {
                            masterVolume = std::max(0.0f, masterVolume - 10.0f);
                            updateAllVolumes(backgroundMusic, spaceSound, laserSound, bombSound, wowSounds, masterVolume, baseMusicVolume, baseSpaceVolume, baseLaserVolume, baseBombVolume, baseWowVolume);
                            std::cout << "Master volume decreased to: " << masterVolume << "%" << std::endl;
                            saveData.masterVolume = masterVolume;
                            saveGameData(saveData);
                        } else if (isMuted) {
                            std::cout << "Cannot change volume while muted" << std::endl;
                        } else {
                            std::cout << "Master volume already at minimum (0%)" << std::endl;
                        }
                        break;
                    }
                    case sf::Keyboard::Key::Period: {
                        if (!isMuted && masterVolume < 100.0f) {
                            masterVolume = std::min(100.0f, masterVolume + 10.0f);
                            updateAllVolumes(backgroundMusic, spaceSound, laserSound, bombSound, wowSounds, masterVolume, baseMusicVolume, baseSpaceVolume, baseLaserVolume, baseBombVolume, baseWowVolume);
                            std::cout << "Master volume increased to: " << masterVolume << "%" << std::endl;
                            saveData.masterVolume = masterVolume;
                            saveGameData(saveData);
                        } else if (isMuted) {
                            std::cout << "Cannot change volume while muted" << std::endl;
                        } else {
                            std::cout << "Master volume already at maximum (100%)" << std::endl;
                        }
                        break;
                    }
                    case sf::Keyboard::Key::P: {
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
        

        bool currentLeftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
        bool currentRightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
        

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
        
        bool fastFall = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
        if (!gameOver) {
            activePiece.update(deltaTime, fastFall, grid);
        }
        if (activePiece.hasStopped() && !gameOver) {
            activePiece.ChangeToStatic(grid, activePiece.getAbility(), &bombSound);
            int clearedLines = clearFullLines(grid, laserSound, wowSounds);
            totalLinesCleared += clearedLines;
            
            if (clearedLines > 0) {
                linesSinceLastAbility += clearedLines;
                
                if (linesSinceLastAbility >= LINES_FOR_ABILITY) {
                    bombAbilityAvailable = true;
                    std::cout << "BOMB ABILITY READY! Press 'I' to activate!" << std::endl;
                }
            }
            

            if (clearedLines > 0) {
                int earnedScore = calculateScore(clearedLines);
                totalScore += earnedScore;
                std::cout << "Lines cleared: " << clearedLines << " | Score earned: " << earnedScore << " | Total score: " << totalScore << std::endl;
            }
            
            int newLevel = calculateLevel(totalLinesCleared);
            if (newLevel != currentLevel) {
                currentLevel = newLevel;
                tetrisBag.updateLevel(currentLevel);
                std::cout << "LEVEL UP! Level " << currentLevel << " reached! (Lines: " << totalLinesCleared << ")" << std::endl;
            }
            
            PieceType randomType = tetrisBag.getNextPiece();
            std::cout << "Respawn: " << pieceTypeToString(randomType) << " (Bag System)" << std::endl;
            PieceShape newShape = getPieceShape(randomType);
            int spawnX = (GRID_WIDTH - newShape.width) / 2;
            activePiece = Piece(spawnX, -1, randomType);
            
            if (activePiece.collidesAt(grid, spawnX, 0)) {
                gameOver = true;
                std::cout << "GAME OVER! Cannot spawn new piece at position (" << spawnX << ", 0)" << std::endl;
                std::cout << "Final Score: " << totalScore << " | Lines: " << totalLinesCleared << " | Level: " << currentLevel << std::endl;
                
                insertNewScore(saveData, totalScore, totalLinesCleared, currentLevel);
                
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
                
                saveData.masterVolume = masterVolume;
                saveData.isMuted = isMuted;
                
                saveGameData(saveData);
                
                std::cout << "Game data saved with new scores!" << std::endl;
            }
            
            canUseHold = true;
            

            leftHoldTime = 0.0f;
            rightHoldTime = 0.0f;
            dasTimer = 0.0f;
            leftPressed = false;
            rightPressed = false;
        }
        window.clear(sf::Color::Black);
        drawGridBackground(window);
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                float worldX = GRID_OFFSET_X + j * CELL_SIZE;
                float worldY = GRID_OFFSET_Y + i * CELL_SIZE;
                TextureType cellTextureType = grid[i][j].occupied ? grid[i][j].getTextureType() : TextureType::Empty;
                

                if (textures.find(cellTextureType) != textures.end()) {

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
        drawNextPieces(window, tetrisBag.getNextQueue(), textures, useTextures);
        drawHeldPiece(window, heldPiece, hasHeldPiece, textures, useTextures);
        drawBombAbility(window, bombAbilityAvailable, linesSinceLastAbility, textures, useTextures, gameFont, fontLoaded);
        drawLevelInfo(window, totalLinesCleared, currentLevel, totalScore, textures, useTextures, gameFont, fontLoaded);
        drawGridBorder(window);
        drawJigtrizTitle(window, gameFont, fontLoaded);
        
        if (gameOver) {
            drawGameOver(window, totalScore, totalLinesCleared, currentLevel, textures, useTextures, gameFont, fontLoaded, saveData);
        }
        
        window.display();
    }
    return 0;
}
