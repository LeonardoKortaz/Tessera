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

enum class GameState {
    MainMenu,
    Options,
    Rebinding,
    ConfirmClearScores,
    Playing,
    Paused,
    GameOver
};

enum class MenuOption {
    Start = 0,
    Options = 1,
    Exit = 2
};

enum class OptionsMenuOption {
    ClearScores = 0,
    RebindKeys = 1
};

enum class PauseOption {
    Resume = 0,
    Restart = 1,
    QuitToMenu = 2
};

enum class ConfirmOption {
    Yes = 0,
    No = 1
};

struct KeyBindings {
    sf::Keyboard::Key moveLeft = sf::Keyboard::Key::A;
    sf::Keyboard::Key moveRight = sf::Keyboard::Key::D;
    sf::Keyboard::Key rotateLeft = sf::Keyboard::Key::J;
    sf::Keyboard::Key rotateRight = sf::Keyboard::Key::K;
    sf::Keyboard::Key quickFall = sf::Keyboard::Key::S;
    sf::Keyboard::Key drop = sf::Keyboard::Key::Space;
    sf::Keyboard::Key hold = sf::Keyboard::Key::L;
    sf::Keyboard::Key mute = sf::Keyboard::Key::M;
    sf::Keyboard::Key volumeDown = sf::Keyboard::Key::Comma;
    sf::Keyboard::Key volumeUp = sf::Keyboard::Key::Period;
    sf::Keyboard::Key menu = sf::Keyboard::Key::Escape;
    sf::Keyboard::Key bomb = sf::Keyboard::Key::I;
};

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
    

    int moveLeft = static_cast<int>(sf::Keyboard::Key::A);
    int moveRight = static_cast<int>(sf::Keyboard::Key::D);
    int rotateLeft = static_cast<int>(sf::Keyboard::Key::J);
    int rotateRight = static_cast<int>(sf::Keyboard::Key::K);
    int quickFall = static_cast<int>(sf::Keyboard::Key::S);
    int drop = static_cast<int>(sf::Keyboard::Key::Space);
    int hold = static_cast<int>(sf::Keyboard::Key::L);
    int bomb = static_cast<int>(sf::Keyboard::Key::I);
    int mute = static_cast<int>(sf::Keyboard::Key::M);
    int volumeDown = static_cast<int>(sf::Keyboard::Key::Comma);
    int volumeUp = static_cast<int>(sf::Keyboard::Key::Period);
    int menu = static_cast<int>(sf::Keyboard::Key::Escape);
};

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
        file << "BEST_LINES=" << data.bestLines << std::endl;
        file << "BEST_LEVEL=" << data.bestLevel << std::endl;
        file << "MASTER_VOLUME=" << data.masterVolume << std::endl;
        file << "IS_MUTED=" << (data.isMuted ? 1 : 0) << std::endl;
        
        for (int i = 0; i < 3; i++) {
            file << "TOP" << (i+1) << "_SCORE=" << data.topScores[i].score << std::endl;
            file << "TOP" << (i+1) << "_LINES=" << data.topScores[i].lines << std::endl;
            file << "TOP" << (i+1) << "_LEVEL=" << data.topScores[i].level << std::endl;
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
                } else if (key == "BEST_LINES") {
                    data.bestLines = std::stoi(value);
                } else if (key == "BEST_LEVEL") {
                    data.bestLevel = std::stoi(value);
                } else if (key == "MASTER_VOLUME") {
                    data.masterVolume = std::stof(value);
                } else if (key == "IS_MUTED") {
                    data.isMuted = (std::stoi(value) == 1);
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
    GenericBlock,
    
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
    Digit9,

    MuteIcon
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

struct ExplosionEffect {
    int x, y;
    float timer;
    ExplosionEffect(int posX, int posY) : x(posX), y(posY), timer(0.5f) {}
};

struct GlowEffect {
    float worldX, worldY;
    sf::Color color;
    float timer;
    float alpha;
    
    GlowEffect(float wx, float wy, sf::Color col) 
        : worldX(wx), worldY(wy), color(col), timer(0.75f), alpha(128) {}
    
    void update(float deltaTime) {
        timer -= deltaTime;
        if (timer > 0) {
            alpha = (timer / 0.75f) * 128.0f;
        } else {
            alpha = 0;
        }
    }
    
    bool isFinished() const {
        return timer <= 0.0f;
    }
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
    
    void returnPieceToBag(PieceType piece) {


        std::cout << "[DEBUG] Returning piece to bag at position " << bagIndex << " (will be next piece)" << std::endl;
        currentBag.insert(currentBag.begin() + bagIndex, piece);
        fillNextQueue();
        std::cout << "[DEBUG] Bag size after return: " << currentBag.size() << std::endl;
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

void drawLevelInfo(sf::RenderWindow& window, int totalLinesCleared, int currentLevel, int totalScore, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y + 50;
    
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

PieceShape getPieceShape(PieceType type) {
    PieceShape shape;
    switch(type){
        case PieceType::I_Basic:
            shape.blocks = {{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color::Cyan;
            break;
        case PieceType::I_Medium:
            shape.blocks = {{false, false, false, false, false},{true, true, true, true, true},{false, false, false, false, false},{false, false, false, false, false}};
            shape.color = sf::Color(0, 200, 255);
            break;
        case PieceType::I_Hard:
            shape.blocks = {{false, false, false, false, false},{true, true, true, true, true},{false, false, true, false, false},{false, false, false, false, false}};
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
                    

                    if (useTextures && textures.find(TextureType::GenericBlock) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(TextureType::GenericBlock));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        cellSprite.setColor(shape.color);
                        sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));
                        window.draw(cellSprite);
                    } else if (useTextures && textures.find(getTextureType(type)) != textures.end()) {

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
        

        if (ability == AbilityType::Bomb) return;
        
        int ghostY = getGhostY(grid);
        if (ghostY == y) return;
        
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float worldX = GRID_OFFSET_X + (x + j) * CELL_SIZE;
                    float worldY = GRID_OFFSET_Y + (ghostY + i) * CELL_SIZE;
                    

                    if (useTextures && textures.find(TextureType::GenericBlock) != textures.end()) {
                        sf::Sprite cellSprite(textures.at(TextureType::GenericBlock));
                        cellSprite.setPosition(sf::Vector2f(worldX, worldY));
                        sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
                        cellSprite.setScale(sf::Vector2f(CELL_SIZE / textureSize.x, CELL_SIZE / textureSize.y));

                        sf::Color ghostColor = shape.color;
                        ghostColor.a = 80;
                        cellSprite.setColor(ghostColor);
                        window.draw(cellSprite);
                    } else if (useTextures && textures.find(getTextureType(type)) != textures.end()) {

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
    
    bool newHighScore = finalScore > saveData.highScore;
    bool newBestLines = finalLines > saveData.bestLines;
    bool newBestLevel = finalLevel > saveData.bestLevel;
    
    if (fontLoaded) {
        sf::Text gameOverText(font, "GAME OVER");
        gameOverText.setCharacterSize(32);
        gameOverText.setFillColor(sf::Color(255, 100, 100));
        gameOverText.setStyle(sf::Text::Bold);
        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setPosition(sf::Vector2f(centerX - textBounds.size.x/2, centerY - 170));
        window.draw(gameOverText);
        
        if (newHighScore) {
            sf::Text newRecordText(font, "NEW HIGH SCORE!");
            newRecordText.setCharacterSize(18);
            newRecordText.setFillColor(sf::Color(255, 215, 0));
            newRecordText.setStyle(sf::Text::Bold);
            sf::FloatRect recordBounds = newRecordText.getLocalBounds();
            newRecordText.setPosition(sf::Vector2f(centerX - recordBounds.size.x/2, centerY - 130));
            window.draw(newRecordText);
        }
        

        sf::Text scoreLabel(font, "SCORE");
        scoreLabel.setCharacterSize(18);
        scoreLabel.setFillColor(newHighScore ? sf::Color(255, 215, 0) : sf::Color::Yellow);
        scoreLabel.setStyle(sf::Text::Bold);
        scoreLabel.setPosition(sf::Vector2f(centerX - 100, centerY - 90));
        window.draw(scoreLabel);
        
        sf::Text scoreValue(font, std::to_string(finalScore));
        scoreValue.setCharacterSize(28);
        scoreValue.setFillColor(sf::Color::White);
        sf::FloatRect scoreBounds = scoreValue.getLocalBounds();
        scoreValue.setPosition(sf::Vector2f(centerX - scoreBounds.size.x/2, centerY - 60));
        window.draw(scoreValue);
        
        sf::Text linesLabel(font, "LINES");
        linesLabel.setCharacterSize(18);
        linesLabel.setFillColor(newBestLines ? sf::Color(255, 215, 0) : sf::Color::Green);
        linesLabel.setStyle(sf::Text::Bold);
        linesLabel.setPosition(sf::Vector2f(centerX - 100, centerY + 10));
        window.draw(linesLabel);
        
        sf::Text linesValue(font, std::to_string(finalLines));
        linesValue.setCharacterSize(28);
        linesValue.setFillColor(sf::Color::White);
        sf::FloatRect linesBounds = linesValue.getLocalBounds();
        linesValue.setPosition(sf::Vector2f(centerX - linesBounds.size.x/2, centerY + 40));
        window.draw(linesValue);
        
        sf::Text levelLabel(font, "LEVEL");
        levelLabel.setCharacterSize(18);
        levelLabel.setFillColor(newBestLevel ? sf::Color(255, 215, 0) : sf::Color::Cyan);
        levelLabel.setStyle(sf::Text::Bold);
        levelLabel.setPosition(sf::Vector2f(centerX - 100, centerY + 90));
        window.draw(levelLabel);
        
        sf::Text levelValue(font, std::to_string(finalLevel));
        levelValue.setCharacterSize(28);
        levelValue.setFillColor(sf::Color::White);
        sf::FloatRect levelBounds = levelValue.getLocalBounds();
        levelValue.setPosition(sf::Vector2f(centerX - levelBounds.size.x/2, centerY + 120));
        window.draw(levelValue);
        
        sf::Text restartText(font, "Press R to return to menu");
        restartText.setCharacterSize(14);
        restartText.setFillColor(sf::Color(150, 150, 255));
        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setPosition(sf::Vector2f(centerX - restartBounds.size.x/2, centerY + 165));
        window.draw(restartText);
        
        sf::Text topScoresTitle(font, "TOP 3 SCORES");
        topScoresTitle.setCharacterSize(16);
        topScoresTitle.setFillColor(sf::Color(100, 255, 150));
        topScoresTitle.setStyle(sf::Text::Bold);
        topScoresTitle.setPosition(sf::Vector2f(centerX + 120, centerY - 100));
        window.draw(topScoresTitle);
        
        for (int i = 0; i < 3; i++) {
            if (saveData.topScores[i].score > 0) {
                sf::Color scoreColor = sf::Color::White;
                if (i == 0) scoreColor = sf::Color(255, 215, 0);
                else if (i == 1) scoreColor = sf::Color(192, 192, 192);
                else if (i == 2) scoreColor = sf::Color(205, 127, 50);
                
                sf::Text rankText(font, "#" + std::to_string(i + 1) + ": " + std::to_string(saveData.topScores[i].score));
                rankText.setCharacterSize(14);
                rankText.setFillColor(scoreColor);
                rankText.setPosition(sf::Vector2f(centerX + 120, centerY - 70 + i * 40));
                window.draw(rankText);
                
                sf::Text detailText(font, "L" + std::to_string(saveData.topScores[i].lines) + " Lv" + std::to_string(saveData.topScores[i].level));
                detailText.setCharacterSize(11);
                detailText.setFillColor(sf::Color(180, 180, 180));
                detailText.setPosition(sf::Vector2f(centerX + 130, centerY - 50 + i * 40));
                window.draw(detailText);
            } else {
                sf::Text emptyText(font, "#" + std::to_string(i + 1) + ": ---");
                emptyText.setCharacterSize(14);
                emptyText.setFillColor(sf::Color(100, 100, 100));
                emptyText.setPosition(sf::Vector2f(centerX + 120, centerY - 70 + i * 40));
                window.draw(emptyText);
            }
        }
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(255, 100, 100));
        titleBar.setSize(sf::Vector2f(300, 40));
        titleBar.setPosition(sf::Vector2f(centerX - 150, centerY - 170));
        window.draw(titleBar);
        
        sf::RectangleShape restartHint;
        restartHint.setFillColor(sf::Color(150, 150, 255));
        restartHint.setSize(sf::Vector2f(200, 20));
        restartHint.setPosition(sf::Vector2f(centerX - 100, centerY + 160));
        window.draw(restartHint);
    }
}

void drawJigtrizTitle(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded) {
    if (!fontLoaded) return;
    
    sf::Text titleText(font, "Jigtriz 0.2.0");
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

void drawPauseMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, PauseOption selectedOption) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    sf::RectangleShape menuBg;
    menuBg.setFillColor(sf::Color(40, 40, 50, 240));
    menuBg.setOutlineColor(sf::Color(100, 150, 255, 255));
    menuBg.setOutlineThickness(4);
    menuBg.setSize(sf::Vector2f(400, 350));
    menuBg.setPosition(sf::Vector2f(centerX - 200, centerY - 175));
    window.draw(menuBg);
    
    if (fontLoaded) {
        sf::Text pausedText(menuFont, "Jigtriz");
        pausedText.setCharacterSize(48);
        pausedText.setFillColor(sf::Color(100, 150, 255));
        pausedText.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = pausedText.getLocalBounds();
        pausedText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 140));
        window.draw(pausedText);
        
        sf::Text resumeText(menuFont, "RESUME");
        resumeText.setCharacterSize(32);
        if (selectedOption == PauseOption::Resume) {
            resumeText.setFillColor(sf::Color::Yellow);
            resumeText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 50));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(2);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY - 50));
            window.draw(selector);
        } else {
            resumeText.setFillColor(sf::Color::White);
        }
        sf::FloatRect resumeBounds = resumeText.getLocalBounds();
        resumeText.setPosition(sf::Vector2f(centerX - resumeBounds.size.x/2, centerY - 40));
        window.draw(resumeText);
        
        sf::Text restartText(menuFont, "RESTART");
        restartText.setCharacterSize(32);
        if (selectedOption == PauseOption::Restart) {
            restartText.setFillColor(sf::Color::Yellow);
            restartText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 50));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(2);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY + 20));
            window.draw(selector);
        } else {
            restartText.setFillColor(sf::Color::White);
        }
        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setPosition(sf::Vector2f(centerX - restartBounds.size.x/2, centerY + 30));
        window.draw(restartText);
        
        sf::Text quitText(menuFont, "QUIT TO MENU");
        quitText.setCharacterSize(32);
        if (selectedOption == PauseOption::QuitToMenu) {
            quitText.setFillColor(sf::Color::Yellow);
            quitText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 50));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(2);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY + 90));
            window.draw(selector);
        } else {
            quitText.setFillColor(sf::Color::White);
        }
        sf::FloatRect quitBounds = quitText.getLocalBounds();
        quitText.setPosition(sf::Vector2f(centerX - quitBounds.size.x/2, centerY + 100));
        window.draw(quitText);
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(14);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 155));
        window.draw(controlsText);
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(100, 150, 255));
        titleBar.setSize(sf::Vector2f(300, 60));
        titleBar.setPosition(sf::Vector2f(centerX - 150, centerY - 140));
        window.draw(titleBar);
        
        float optionY = centerY - 40;
        for (int i = 0; i < 3; i++) {
            sf::RectangleShape option;
            option.setFillColor(i == static_cast<int>(selectedOption) ? sf::Color::Yellow : sf::Color::White);
            option.setSize(sf::Vector2f(280, 40));
            option.setPosition(sf::Vector2f(centerX - 140, optionY + i * 70));
            window.draw(option);
        }
    }
}

void drawConfirmClearScores(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, ConfirmOption selectedOption) {

    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    

    sf::RectangleShape dialogBg;
    dialogBg.setFillColor(sf::Color(40, 40, 50, 240));
    dialogBg.setOutlineColor(sf::Color(100, 150, 255, 255));
    dialogBg.setOutlineThickness(4);
    dialogBg.setSize(sf::Vector2f(600, 280));
    dialogBg.setPosition(sf::Vector2f(centerX - 300, centerY - 140));
    window.draw(dialogBg);
    
    if (fontLoaded) {

        sf::Text titleText(menuFont, "Confirm");
        titleText.setCharacterSize(42);
        titleText.setFillColor(sf::Color(255, 100, 100));
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 110));
        window.draw(titleText);
        

        sf::Text questionText(menuFont, "Are you sure you want to clear");
        questionText.setCharacterSize(26);
        questionText.setFillColor(sf::Color::White);
        sf::FloatRect q1Bounds = questionText.getLocalBounds();
        questionText.setPosition(sf::Vector2f(centerX - q1Bounds.size.x/2, centerY - 50));
        window.draw(questionText);
        
        sf::Text questionText2(menuFont, "all your scores?");
        questionText2.setCharacterSize(26);
        questionText2.setFillColor(sf::Color::White);
        sf::FloatRect q2Bounds = questionText2.getLocalBounds();
        questionText2.setPosition(sf::Vector2f(centerX - q2Bounds.size.x/2, centerY - 20));
        window.draw(questionText2);
        

        sf::Text yesText(menuFont, "YES");
        yesText.setCharacterSize(36);
        if (selectedOption == ConfirmOption::Yes) {
            yesText.setFillColor(sf::Color::Yellow);
            yesText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape yesSelector;
            yesSelector.setSize(sf::Vector2f(200, 60));
            yesSelector.setFillColor(sf::Color(255, 255, 0, 50));
            yesSelector.setOutlineColor(sf::Color::Yellow);
            yesSelector.setOutlineThickness(3);
            yesSelector.setPosition(sf::Vector2f(centerX - 250, centerY + 40));
            window.draw(yesSelector);
        } else {
            yesText.setFillColor(sf::Color::White);
        }
        sf::FloatRect yesBounds = yesText.getLocalBounds();
        yesText.setPosition(sf::Vector2f(centerX - 150 - yesBounds.size.x/2, centerY + 55));
        window.draw(yesText);
        

        sf::Text noText(menuFont, "NO");
        noText.setCharacterSize(36);
        if (selectedOption == ConfirmOption::No) {
            noText.setFillColor(sf::Color::Yellow);
            noText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape noSelector;
            noSelector.setSize(sf::Vector2f(200, 60));
            noSelector.setFillColor(sf::Color(255, 255, 0, 50));
            noSelector.setOutlineColor(sf::Color::Yellow);
            noSelector.setOutlineThickness(3);
            noSelector.setPosition(sf::Vector2f(centerX + 50, centerY + 40));
            window.draw(noSelector);
        } else {
            noText.setFillColor(sf::Color::White);
        }
        sf::FloatRect noBounds = noText.getLocalBounds();
        noText.setPosition(sf::Vector2f(centerX + 150 - noBounds.size.x/2, centerY + 55));
        window.draw(noText);
        

        sf::Text instructText(menuFont, "Arrow keys to select, SPACE/ENTER to confirm");
        instructText.setCharacterSize(18);
        instructText.setFillColor(sf::Color(180, 180, 180));
        sf::FloatRect instructBounds = instructText.getLocalBounds();
        instructText.setPosition(sf::Vector2f(centerX - instructBounds.size.x/2, centerY + 120));
        window.draw(instructText);
    }
}

void drawMainMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, MenuOption selectedOption, bool debugMode) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "JIGTRIZ");
        titleText.setCharacterSize(128);
        titleText.setFillColor(sf::Color(100, 255, 150));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 320));
        window.draw(titleText);
        
        sf::Text versionText(menuFont, "v0.2.0");
        versionText.setCharacterSize(24);
        versionText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect versionBounds = versionText.getLocalBounds();
        versionText.setPosition(sf::Vector2f(centerX - versionBounds.size.x/2, centerY - 170));
        window.draw(versionText);
        
        sf::Text startText(menuFont, "START");
        startText.setCharacterSize(48);
        if (selectedOption == MenuOption::Start) {
            startText.setFillColor(sf::Color::Yellow);
            startText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY - 85));
            window.draw(selector);
        } else {
            startText.setFillColor(sf::Color::White);
        }
        sf::FloatRect startBounds = startText.getLocalBounds();
        startText.setPosition(sf::Vector2f(centerX - startBounds.size.x/2, centerY - 90));
        window.draw(startText);
        
        sf::Text optionsText(menuFont, "OPTIONS");
        optionsText.setCharacterSize(48);
        if (selectedOption == MenuOption::Options) {
            optionsText.setFillColor(sf::Color::Yellow);
            optionsText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY + 15));
            window.draw(selector);
        } else {
            optionsText.setFillColor(sf::Color::White);
        }
        sf::FloatRect optionsBounds = optionsText.getLocalBounds();
        optionsText.setPosition(sf::Vector2f(centerX - optionsBounds.size.x/2, centerY + 10));
        window.draw(optionsText);
        
        sf::Text exitText(menuFont, "EXIT");
        exitText.setCharacterSize(48);
        if (selectedOption == MenuOption::Exit) {
            exitText.setFillColor(sf::Color::Yellow);
            exitText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY + 115));
            window.draw(selector);
        } else {
            exitText.setFillColor(sf::Color::White);
        }
        sf::FloatRect exitBounds = exitText.getLocalBounds();
        exitText.setPosition(sf::Vector2f(centerX - exitBounds.size.x/2, centerY + 110));
        window.draw(exitText);
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 230));
        window.draw(controlsText);
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(100, 255, 150));
        titleBar.setSize(sf::Vector2f(400, 80));
        titleBar.setPosition(sf::Vector2f(centerX - 200, centerY - 250));
        window.draw(titleBar);
        
        float optionY = centerY - 90;
        for (int i = 0; i < 3; i++) {
            sf::RectangleShape option;
            option.setSize(sf::Vector2f(300, 60));
            if ((i == 0 && selectedOption == MenuOption::Start) || 
                (i == 1 && selectedOption == MenuOption::Options) || 
                (i == 2 && selectedOption == MenuOption::Exit)) {
                option.setFillColor(sf::Color::Yellow);
            } else {
                option.setFillColor(sf::Color::White);
            }
            option.setPosition(sf::Vector2f(centerX - 150, optionY + i * 100));
            window.draw(option);
        }
    }
    
    if (debugMode && fontLoaded) {
        sf::Text debugText(menuFont);
        debugText.setString("DEBUG MODE");
        debugText.setCharacterSize(24);
        debugText.setFillColor(sf::Color::Yellow);
        debugText.setPosition(sf::Vector2f(1920.0f - 180.0f, 1080.0f - 40.0f));
        window.draw(debugText);
    }
}

void drawOptionsMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, bool debugMode, OptionsMenuOption selectedOption) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(menuFont, "OPTIONS");
        titleText.setCharacterSize(128);
        titleText.setFillColor(sf::Color(100, 255, 150));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 320));
        window.draw(titleText);
        
        sf::Text versionText(menuFont, "Settings");
        versionText.setCharacterSize(24);
        versionText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect versionBounds = versionText.getLocalBounds();
        versionText.setPosition(sf::Vector2f(centerX - versionBounds.size.x/2, centerY - 170));
        window.draw(versionText);
        
        sf::Text clearText(menuFont, "CLEAR SCORES");
        clearText.setCharacterSize(48);
        if (selectedOption == OptionsMenuOption::ClearScores) {
            clearText.setFillColor(sf::Color::Yellow);
            clearText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(400, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 200, centerY - 85));
            window.draw(selector);
        } else {
            clearText.setFillColor(sf::Color::White);
        }
        sf::FloatRect clearBounds = clearText.getLocalBounds();
        clearText.setPosition(sf::Vector2f(centerX - clearBounds.size.x/2, centerY - 90));
        window.draw(clearText);
        
        sf::Text rebindText(menuFont, "REBIND KEYS");
        rebindText.setCharacterSize(48);
        if (selectedOption == OptionsMenuOption::RebindKeys) {
            rebindText.setFillColor(sf::Color::Yellow);
            rebindText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(400, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 200, centerY + 15));
            window.draw(selector);
        } else {
            rebindText.setFillColor(sf::Color::White);
        }
        sf::FloatRect rebindBounds = rebindText.getLocalBounds();
        rebindText.setPosition(sf::Vector2f(centerX - rebindBounds.size.x/2, centerY + 10));
        window.draw(rebindText);
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 230));
        window.draw(controlsText);
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(100, 255, 150));
        titleBar.setSize(sf::Vector2f(400, 80));
        titleBar.setPosition(sf::Vector2f(centerX - 200, centerY - 250));
        window.draw(titleBar);
        
        float optionY = centerY - 90;
        for (int i = 0; i < 2; i++) {
            sf::RectangleShape option;
            option.setSize(sf::Vector2f(400, 60));
            if ((i == 0 && selectedOption == OptionsMenuOption::ClearScores) || 
                (i == 1 && selectedOption == OptionsMenuOption::RebindKeys)) {
                option.setFillColor(sf::Color::Yellow);
            } else {
                option.setFillColor(sf::Color::White);
            }
            option.setPosition(sf::Vector2f(centerX - 200, optionY + i * 100));
            window.draw(option);
        }
    }
    
    if (debugMode && fontLoaded) {
        sf::Text debugText(menuFont);
        debugText.setString("DEBUG MODE");
        debugText.setCharacterSize(24);
        debugText.setFillColor(sf::Color::Yellow);
        debugText.setPosition(sf::Vector2f(1920.0f - 180.0f, 1080.0f - 40.0f));
        window.draw(debugText);
    }
}

std::string getKeyName(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::A: return "A";
        case sf::Keyboard::Key::B: return "B";
        case sf::Keyboard::Key::C: return "C";
        case sf::Keyboard::Key::D: return "D";
        case sf::Keyboard::Key::E: return "E";
        case sf::Keyboard::Key::F: return "F";
        case sf::Keyboard::Key::G: return "G";
        case sf::Keyboard::Key::H: return "H";
        case sf::Keyboard::Key::I: return "I";
        case sf::Keyboard::Key::J: return "J";
        case sf::Keyboard::Key::K: return "K";
        case sf::Keyboard::Key::L: return "L";
        case sf::Keyboard::Key::M: return "M";
        case sf::Keyboard::Key::N: return "N";
        case sf::Keyboard::Key::O: return "O";
        case sf::Keyboard::Key::P: return "P";
        case sf::Keyboard::Key::Q: return "Q";
        case sf::Keyboard::Key::R: return "R";
        case sf::Keyboard::Key::S: return "S";
        case sf::Keyboard::Key::T: return "T";
        case sf::Keyboard::Key::U: return "U";
        case sf::Keyboard::Key::V: return "V";
        case sf::Keyboard::Key::W: return "W";
        case sf::Keyboard::Key::X: return "X";
        case sf::Keyboard::Key::Y: return "Y";
        case sf::Keyboard::Key::Z: return "Z";
        case sf::Keyboard::Key::Space: return "SPACE";
        case sf::Keyboard::Key::Left: return "LEFT";
        case sf::Keyboard::Key::Right: return "RIGHT";
        case sf::Keyboard::Key::Up: return "UP";
        case sf::Keyboard::Key::Down: return "DOWN";
        case sf::Keyboard::Key::Tab: return "TAB";
        case sf::Keyboard::Key::LShift: return "LSHIFT";
        case sf::Keyboard::Key::RShift: return "RSHIFT";
        case sf::Keyboard::Key::LControl: return "LCTRL";
        case sf::Keyboard::Key::RControl: return "RCTRL";
        case sf::Keyboard::Key::Escape: return "ESC";
        case sf::Keyboard::Key::Comma: return "COMMA";
        case sf::Keyboard::Key::Period: return "PERIOD";
        case sf::Keyboard::Key::Slash: return "SLASH";
        case sf::Keyboard::Key::Semicolon: return "SEMICOLON";
        case sf::Keyboard::Key::Enter: return "ENTER";
        case sf::Keyboard::Key::Backspace: return "BACKSPACE";
        default: return "UNKNOWN";
    }
}

void drawRebindingScreen(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, const KeyBindings& bindings, int selectedBinding, bool waitingForKey) {
    const float centerX = 1920.0f / 2.0f;
    const float startY = 150.0f;
    const float lineHeight = 50.0f;
    
    if (fontLoaded) {
        sf::Text titleText(menuFont);
        titleText.setString("REBIND KEYS");
        titleText.setCharacterSize(56);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
        titleText.setPosition(sf::Vector2f(centerX, 80.0f));
        window.draw(titleText);
        
        std::vector<std::pair<std::string, std::string>> keyList = {
            {"Move Left", getKeyName(bindings.moveLeft)},
            {"Move Right", getKeyName(bindings.moveRight)},
            {"Rotate Left", getKeyName(bindings.rotateLeft)},
            {"Rotate Right", getKeyName(bindings.rotateRight)},
            {"Soft Drop (hold)", getKeyName(bindings.quickFall)},
            {"Hard Drop (instant)", getKeyName(bindings.drop)},
            {"Hold", getKeyName(bindings.hold)},
            {"Bomb", getKeyName(bindings.bomb)},
            {"Mute", getKeyName(bindings.mute)},
            {"Volume Down", getKeyName(bindings.volumeDown)},
            {"Volume Up", getKeyName(bindings.volumeUp)},
            {"Menu", getKeyName(bindings.menu)},
            {"RESTORE DEFAULT", ""},
        };
        
        for (size_t i = 0; i < keyList.size(); i++) {

            float extraSpace = (i == 12) ? 30.0f : 0.0f;
            float yPos = startY + i * lineHeight + extraSpace;
            

            if (static_cast<int>(i) == selectedBinding) {
                sf::RectangleShape selector;
                selector.setSize(sf::Vector2f(800, 45));

                if (i == 12) {
                    selector.setFillColor(sf::Color(255, 165, 0, 150));
                    selector.setOutlineColor(sf::Color(255, 215, 0));
                    selector.setOutlineThickness(4);
                } else {
                    selector.setFillColor(sf::Color(100, 200, 255, 128));
                }
                selector.setPosition(sf::Vector2f(centerX - 400, yPos - 5));
                window.draw(selector);
            }
            
            sf::Text labelText(menuFont);
            labelText.setString(keyList[i].first);
            labelText.setCharacterSize(28);
            
            if (i == 12) {

                if (static_cast<int>(i) == selectedBinding) {
                    labelText.setFillColor(sf::Color::Yellow);
                    labelText.setStyle(sf::Text::Bold);
                } else {
                    labelText.setFillColor(sf::Color(255, 200, 100));
                    labelText.setStyle(sf::Text::Bold);
                }
            } else {
                labelText.setFillColor(sf::Color::White);
            }
            
            labelText.setPosition(sf::Vector2f(centerX - 380, yPos));
            window.draw(labelText);
            
            if (!keyList[i].second.empty()) {
                sf::Text keyText(menuFont);
                if (static_cast<int>(i) == selectedBinding && waitingForKey) {
                    keyText.setString("Press any key...");
                    keyText.setFillColor(sf::Color::Yellow);
                } else {
                    keyText.setString(keyList[i].second);
                    keyText.setFillColor(sf::Color(100, 255, 100));
                }
                keyText.setCharacterSize(28);
                keyText.setStyle(sf::Text::Bold);
                keyText.setPosition(sf::Vector2f(centerX + 150, yPos));
                window.draw(keyText);
            }
        }
        
        sf::Text instructionsText(menuFont);
        if (waitingForKey) {
            instructionsText.setString("Press a key to bind | ESC to cancel");
        } else {
            instructionsText.setString("W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        }
        instructionsText.setCharacterSize(22);
        instructionsText.setFillColor(sf::Color(200, 200, 200));
        sf::FloatRect instrBounds = instructionsText.getLocalBounds();
        instructionsText.setOrigin(sf::Vector2f(instrBounds.size.x / 2.0f, instrBounds.size.y / 2.0f));
        instructionsText.setPosition(sf::Vector2f(centerX, 1020.0f));
        window.draw(instructionsText);
    }
}

void drawExplosionEffects(sf::RenderWindow& window, const std::vector<ExplosionEffect>& explosions) {
    for (const auto& explosion : explosions) {
        float worldX = GRID_OFFSET_X + explosion.x * CELL_SIZE;
        float worldY = GRID_OFFSET_Y + explosion.y * CELL_SIZE;
        
        sf::RectangleShape explosionBox;
        explosionBox.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        explosionBox.setPosition(sf::Vector2f(worldX, worldY));
        explosionBox.setFillColor(sf::Color::White);
        
        window.draw(explosionBox);
    }
}

void drawGlowEffects(sf::RenderWindow& window, const std::vector<GlowEffect>& glowEffects, const std::map<TextureType, sf::Texture>& textures) {
    for (const auto& glow : glowEffects) {
        if (textures.find(TextureType::GenericBlock) != textures.end()) {
            sf::Sprite glowSprite(textures.at(TextureType::GenericBlock));
            glowSprite.setPosition(sf::Vector2f(glow.worldX, glow.worldY));
            
            sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
            float scale = (CELL_SIZE * 1.0f) / textureSize.x;
            glowSprite.setScale(sf::Vector2f(scale, scale));
            
            sf::Color glowColor = glow.color;
            glowColor.r = std::min(255, static_cast<int>(glowColor.r + (255 - glowColor.r) * 0.66f));
            glowColor.g = std::min(255, static_cast<int>(glowColor.g + (255 - glowColor.g) * 0.66f));
            glowColor.b = std::min(255, static_cast<int>(glowColor.b + (255 - glowColor.b) * 0.66f));
            glowColor.a = static_cast<std::uint8_t>(glow.alpha);
            
            glowSprite.setColor(glowColor);
            window.draw(glowSprite);
        } else {
            sf::RectangleShape glowBox;
            glowBox.setSize(sf::Vector2f(CELL_SIZE * 1.0f, CELL_SIZE * 1.0f));
            glowBox.setPosition(sf::Vector2f(glow.worldX, glow.worldY));
            
            sf::Color glowColor = glow.color;
            glowColor.a = static_cast<std::uint8_t>(glow.alpha);
            glowBox.setFillColor(glowColor);
            
            window.draw(glowBox);
        }
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
        {TextureType::Empty, "Assets/Texture/Cells/cell_background.png", sf::Color(50, 50, 60)},
        {TextureType::GenericBlock, "Assets/Texture/Cells/cell_normal_block.png", sf::Color::White},
        

        {TextureType::I_Basic, "Assets/Texture/Cells/piece_i.png", sf::Color::Cyan},
        {TextureType::T_Basic, "Assets/Texture/Cells/piece_t.png", sf::Color::Magenta},
        {TextureType::L_Basic, "Assets/Texture/Cells/piece_l.png", sf::Color(255, 165, 0)},
        {TextureType::J_Basic, "Assets/Texture/Cells/piece_j.png", sf::Color::Blue},
        {TextureType::O_Basic, "Assets/Texture/Cells/piece_o.png", sf::Color::Yellow},
        {TextureType::S_Basic, "Assets/Texture/Cells/piece_s.png", sf::Color::Green},
        {TextureType::Z_Basic, "Assets/Texture/Cells/piece_z.png", sf::Color::Red},
        

        {TextureType::I_Medium, "Assets/Texture/Cells/piece_i++.png", sf::Color(0, 200, 255)},
        {TextureType::T_Medium, "Assets/Texture/Cells/piece_t++.png", sf::Color(255, 100, 255)},
        {TextureType::L_Medium, "Assets/Texture/Cells/piece_l++.png", sf::Color(255, 200, 50)},
        {TextureType::J_Medium, "Assets/Texture/Cells/piece_j++.png", sf::Color(100, 100, 255)},
        {TextureType::O_Medium, "Assets/Texture/Cells/piece_o++.png", sf::Color(255, 255, 100)},
        {TextureType::S_Medium, "Assets/Texture/Cells/piece_s++.png", sf::Color(100, 255, 100)},
        {TextureType::Z_Medium, "Assets/Texture/Cells/piece_z++.png", sf::Color(255, 100, 100)},
        

        {TextureType::I_Hard, "Assets/Texture/Cells/piece_i#.png", sf::Color(0, 150, 255)},
        {TextureType::T_Hard, "Assets/Texture/Cells/piece_t#.png", sf::Color(200, 50, 200)},
        {TextureType::L_Hard, "Assets/Texture/Cells/piece_l#.png", sf::Color(200, 120, 0)},
        {TextureType::J_Hard, "Assets/Texture/Cells/piece_j#.png", sf::Color(0, 0, 200)},
        {TextureType::O_Hard, "Assets/Texture/Cells/piece_o#.png", sf::Color(200, 200, 0)},
        {TextureType::S_Hard, "Assets/Texture/Cells/piece_s#.png", sf::Color(0, 200, 0)},
        {TextureType::Z_Hard, "Assets/Texture/Cells/piece_z#.png", sf::Color(200, 0, 0)},
        

        {TextureType::A_Bomb, "Assets/Texture/Cells/bomb.png", sf::Color(255, 100, 100)},
        {TextureType::A_Fill, "Assets/Texture/Cells/fill.png", sf::Color(100, 255, 100)},
        {TextureType::A_Drill, "Assets/Texture/Cells/drill.png", sf::Color(255, 255, 100)},
        

        {TextureType::Digit0, "Assets/Texture/Cells/0.png", sf::Color::White},
        {TextureType::Digit1, "Assets/Texture/Cells/1.png", sf::Color::White},
        {TextureType::Digit2, "Assets/Texture/Cells/2.png", sf::Color::White},
        {TextureType::Digit3, "Assets/Texture/Cells/3.png", sf::Color::White},
        {TextureType::Digit4, "Assets/Texture/Cells/4.png", sf::Color::White},
        {TextureType::Digit5, "Assets/Texture/Cells/5.png", sf::Color::White},
        {TextureType::Digit6, "Assets/Texture/Cells/6.png", sf::Color::White},
        {TextureType::Digit7, "Assets/Texture/Cells/7.png", sf::Color::White},
        {TextureType::Digit8, "Assets/Texture/Cells/8.png", sf::Color::White},
        {TextureType::Digit9, "Assets/Texture/Cells/9.png", sf::Color::White},

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
    if (titleFont.openFromFile("Assets/Fonts/Jersey25-Regular.ttf")) {
        titleFontLoaded = true;
        std::cout << "Font loaded successfully: Jersey25-Regular.ttf" << std::endl;
    } else {
        std::cout << "Unable to load font: Assets/Fonts/Jersey25-Regular.ttf" << std::endl;
    }
    
    sf::Font menuFont;
    bool menuFontLoaded = false;
    if (menuFont.openFromFile("Assets/Fonts/Jersey15-Regular.ttf")) {
        menuFontLoaded = true;
        std::cout << "Font loaded successfully: Jersey15-Regular.ttf" << std::endl;
    } else {
        std::cout << "Unable to load font: Assets/Fonts/Jersey15-Regular.ttf" << std::endl;
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
    OptionsMenuOption selectedOptionsOption = OptionsMenuOption::ClearScores;
    PauseOption selectedPauseOption = PauseOption::Resume;
    ConfirmOption selectedConfirmOption = ConfirmOption::No;
    
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
    
    PieceType initType = jigtrizBag.getNextPiece();
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
                            if (selectedMenuOption == MenuOption::Start) {
                                selectedMenuOption = MenuOption::Exit;
                            } else if (selectedMenuOption == MenuOption::Options) {
                                selectedMenuOption = MenuOption::Start;
                            } else if (selectedMenuOption == MenuOption::Exit) {
                                selectedMenuOption = MenuOption::Options;
                            }
                            break;
                        case sf::Keyboard::Key::Down:
                        case sf::Keyboard::Key::S:
                            if (selectedMenuOption == MenuOption::Start) {
                                selectedMenuOption = MenuOption::Options;
                            } else if (selectedMenuOption == MenuOption::Options) {
                                selectedMenuOption = MenuOption::Exit;
                            } else if (selectedMenuOption == MenuOption::Exit) {
                                selectedMenuOption = MenuOption::Start;
                            }
                            break;
                        case sf::Keyboard::Key::Enter:
                        case sf::Keyboard::Key::Space:
                            if (selectedMenuOption == MenuOption::Start) {
                                gameState = GameState::Playing;
                                gameOver = false;
                                
                                for (int i = 0; i < GRID_HEIGHT; ++i) {
                                    for (int j = 0; j < GRID_WIDTH; ++j) {
                                        grid[i][j] = Cell();
                                    }
                                }
                                totalLinesCleared = 0;
                                currentLevel = 0;
                                totalScore = 0;
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
                                activePiece = Piece(startX, 0, startType);
                                
                                std::cout << "Game started from menu!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
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
                                
                                for (int i = 0; i < GRID_HEIGHT; ++i) {
                                    for (int j = 0; j < GRID_WIDTH; ++j) {
                                        grid[i][j] = Cell();
                                    }
                                }
                                totalLinesCleared = 0;
                                currentLevel = 0;
                                totalScore = 0;
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
                                saveData.bestLines = 0;
                                saveData.bestLevel = 0;
                                for (int i = 0; i < 3; i++) {
                                    saveData.topScores[i].score = 0;
                                    saveData.topScores[i].lines = 0;
                                    saveData.topScores[i].level = 0;
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
                    if (!gameOver) activePiece.rotateLeft(grid);
                } else if (keyPressed->code == keyBindings.rotateRight) {
                    if (!gameOver) activePiece.rotateRight(grid);
                } else if (keyPressed->code == keyBindings.hold) {
                    if (!canUseHold || gameOver) {
                        
                    } else {
                        if (!hasHeldPiece) {

                            heldPiece = activePiece.getType();
                            hasHeldPiece = true;
                            
                            PieceType newType = jigtrizBag.getNextPiece();
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
                            

                            PieceType newType = jigtrizBag.getNextPiece();
                            std::cout << "Spawning new piece after explosion: " << pieceTypeToString(newType) << std::endl;
                            PieceShape newShape = getPieceShape(newType);
                            int spawnX = (GRID_WIDTH - newShape.width) / 2;
                            activePiece = Piece(spawnX, 0, newType);
                            canUseHold = true;
                        } else {

                            activePiece.moveGround(grid);
                            if (spaceSound) {
                                spaceSound->play();
                            }
                        }
                    }
                } else if (keyPressed->code == keyBindings.menu) {
                    if (!gameOver) {
                        gameState = GameState::Paused;
                        selectedPauseOption = PauseOption::Resume;
                        std::cout << "Game paused" << std::endl;
                    }
                } else if (keyPressed->code == keyBindings.bomb) {
                    if ((bombAbilityAvailable || debugMode) && !gameOver) {
                        std::cout << "BOMB ABILITY ACTIVATED - Spawning bomb!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
                        

                        PieceType replacedPiece = activePiece.getType();
                        jigtrizBag.returnPieceToBag(replacedPiece);
                        std::cout << "Returned " << pieceTypeToString(replacedPiece) << " to bag" << std::endl;
                        
                        PieceType newType = PieceType::A_Bomb;
                        std::cout << "Spawning bomb piece!" << std::endl;
                        PieceShape newShape = getPieceShape(newType);
                        int spawnX = (GRID_WIDTH - newShape.width) / 2;
                        activePiece = Piece(spawnX, 0, newType);
                        
                        if (!debugMode) {
                            bombAbilityAvailable = false;
                            linesSinceLastAbility = 0;
                        }
                    } else if (!bombAbilityAvailable && !debugMode && !gameOver) {
                        int linesNeeded = LINES_FOR_ABILITY - linesSinceLastAbility;
                        std::cout << "Bomb ability not ready yet! Need " << linesNeeded << " more lines." << std::endl;
                    }
                }
                
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::R: {
                        std::cout << "R key pressed! Restarting game..." << std::endl;
                        gameState = GameState::Playing;
                        gameOver = false;
                        
                        for (int i = 0; i < GRID_HEIGHT; ++i) {
                            for (int j = 0; j < GRID_WIDTH; ++j) {
                                grid[i][j] = Cell();
                            }
                        }
                        totalLinesCleared = 0;
                        currentLevel = 0;
                        totalScore = 0;
                        
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
                        activePiece = Piece(startX, 0, startType);
                        
                        std::cout << "Game restarted!" << (debugMode ? " (DEBUG MODE)" : "") << std::endl;
                        
                        break;
                    }
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
        
        if (gameState == GameState::Playing) {

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
            int clearedLines = clearFullLines(grid, laserSound, wowSounds, &glowEffects, &shakeIntensity, &shakeDuration, &shakeTimer);
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
                jigtrizBag.updateLevel(currentLevel);
                std::cout << "LEVEL UP! Level " << currentLevel << " reached! (Lines: " << totalLinesCleared << ")" << std::endl;
            }
            
            PieceType randomType = jigtrizBag.getNextPiece();
            std::cout << "Respawn: " << pieceTypeToString(randomType) << " (Bag System)" << std::endl;
            PieceShape newShape = getPieceShape(randomType);
            int spawnX = (GRID_WIDTH - newShape.width) / 2;
            activePiece = Piece(spawnX, 0, randomType);
            
            if (activePiece.collidesAt(grid, spawnX, 0)) {
                gameOver = true;
                std::cout << "GAME OVER! New piece overlaps with existing blocks at spawn position (" << spawnX << ", 0)" << std::endl;
                std::cout << "Final Score: " << totalScore << " | Lines: " << totalLinesCleared << " | Level: " << currentLevel << std::endl;
                
                if (!debugMode) {
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
        drawLevelInfo(window, totalLinesCleared, currentLevel, totalScore, textures, useTextures, menuFont, fontLoaded);
        drawGridBorder(window);
        drawJigtrizTitle(window, titleFont, fontLoaded);
        
        if (gameOver) {
            drawGameOver(window, totalScore, totalLinesCleared, currentLevel, textures, useTextures, menuFont, fontLoaded, saveData);
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
