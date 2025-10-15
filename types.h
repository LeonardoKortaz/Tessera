#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <random>
#include <map>


struct DifficultyConfig;
struct BagLevelConfig;


constexpr int GRID_WIDTH = 11;
constexpr int GRID_HEIGHT = 22;
constexpr float CELL_SIZE = 32.0f;
constexpr int MAX_LEVEL = 5;
constexpr int LEVEL_THRESHOLDS[MAX_LEVEL + 1] = {0, 9, 25, 49, 81, 121};
constexpr float BORDER_WIDTH = 16.0f;
constexpr float GRID_OFFSET_X = (1920 - GRID_WIDTH * CELL_SIZE) / 2.0f;
constexpr float GRID_OFFSET_Y = (1080 - GRID_HEIGHT * CELL_SIZE) / 2.0f;


enum class PieceType;
struct PieceShape;
enum class TextureType;
PieceShape getPieceShape(PieceType type);
TextureType getTextureType(PieceType type);


enum class GameState {
    MainMenu,
    GameModeSelect,
    ClassicDifficultySelect,
    SprintLinesSelect,
    ChallengeSelect,
    Jigtrizopedia,
    AchievementsView,
    Options,
    Rebinding,
    ConfirmClearScores,
    Playing,
    Paused,
    GameOver
};

enum class MenuOption {
    Start = 0,
    Jigtrizopedia = 1,
    Options = 2,
    Exit = 3
};

enum class GameModeOption {
    Classic = 0,
    Sprint = 1,
    Challenge = 2
};

enum class ClassicDifficulty {
    Easy = 0,
    Medium = 1,
    Hard = 2
};

enum class SprintLines {
    Lines12 = 0,
    Lines24 = 1,
    Lines48 = 2
};

enum class ChallengeMode {
    TheForest = 0,
    Randomness = 1,
    NonStraight = 2
};

enum class JigtrizopediaOption {
    JigtrizPieces = 0,
    Achievements = 1,
    Statistics = 2,
    Back = 3
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

    int highScoreClassicEasy = 0;
    int highScoreClassicMedium = 0;
    int highScoreClassicHard = 0;
    

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
    

    ScoreEntry topScoresEasy[3];
    ScoreEntry topScoresMedium[3];
    ScoreEntry topScoresHard[3];
    

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


enum class DifficultyLevel {
    Basic = 0,
    Medium = 1,
    Hard = 2
};


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

    A_Bomb
};


enum class AbilityType {
    None,
    Bomb
};


enum class TextureType {
    Empty,
    GenericBlock,
    MediumBlock,
    HardBlock,
    
    A_Bomb,

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
    
    std::vector<PieceType> mediumBag;
    std::vector<PieceType> hardBag;
    int mediumBagIndex = 0;
    int hardBagIndex = 0;
    
    const DifficultyConfig* difficultyConfig = nullptr;
    
    std::vector<PieceType> createNewBag(int level);
    void ensureNextBagReady();
    void fillNextQueue();
    void refillMediumBag();
    void refillHardBag();
    
public:
    PieceBag();
    PieceType getNextPiece();
    void updateLevel(int newLevel);
    void setDifficultyConfig(const DifficultyConfig* config);
    const std::vector<PieceType>& getNextQueue() const;
    const std::vector<PieceType>& getCurrentBag() const;
    const std::vector<PieceType>& getNextBag() const;
    int getBagIndex() const;
    bool isNextBagReady() const;
    void reset();
    void returnPieceToBag(PieceType piece);
};


std::string getSaveFilePath();
void saveGameData(const SaveData& data);
SaveData loadGameData();
AbilityType getAbilityType(PieceType type);
int calculateLevel(int linesCleared);
