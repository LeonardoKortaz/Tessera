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
enum class Achievement;


constexpr int GRID_WIDTH = 11;
constexpr int GRID_HEIGHT = 22;
constexpr float CELL_SIZE = 32.0f;
constexpr int MAX_LEVEL = 10;
constexpr int LEVEL_THRESHOLDS[MAX_LEVEL + 1] = {
    0,
    9,
    25,
    49,
    72,
    97,
    121,
    145,
    193,
    241,
    289
};



constexpr float LEVEL_GRAVITY_EASY[MAX_LEVEL + 1] = {
    1.0f,
    1.2f,
    1.4f,
    1.8f,
    2.25f,
    2.75f,
    3.25f,
    4.0f,
    5.0f,
    6.5f,
    7.0f
};


constexpr float LEVEL_GRAVITY_MEDIUM[MAX_LEVEL + 1] = {
    1.5f,
    1.75f,
    2.0f,
    2.5f,
    3.0f,
    4.0f,
    5.0f,
    6.0f,
    7.0f,
    8.5f,
    10.0f
};


constexpr float LEVEL_GRAVITY_HARD[MAX_LEVEL + 1] = {
    2.0f,
    2.5f,
    3.0f,
    4.0f,
    5.0f,
    6.0f,
    7.0f,
    8.5f,
    10.0f,
    12.0f,
    14.0f
};


constexpr float LEVEL_GRAVITY[MAX_LEVEL + 1] = {
    1.0f,
    1.25f,
    1.5f,
    2.0f,
    2.5f,
    3.5f,
    5.0f,
    6.5f,
    8.0f,
    10.0f,
    12.0f
};

constexpr float BORDER_WIDTH = 16.0f;
constexpr float GRID_OFFSET_X = (1920 - GRID_WIDTH * CELL_SIZE) / 2.0f;
constexpr float GRID_OFFSET_Y = (1080 - GRID_HEIGHT * CELL_SIZE) / 2.0f;


enum class PieceType;
struct PieceShape;
enum class TextureType;
PieceShape getPieceShape(PieceType type);
TextureType getTextureType(PieceType type);


enum class GameState {
    SplashScreen,
    MainMenu,
    GameModeSelect,
    ClassicDifficultySelect,
    SprintLinesSelect,
    ChallengeSelect,
    PracticeSelect,
    Extras,
    AchievementsView,
    StatisticsView,
    BestScoresView,
    Options,
    Rebinding,
    ConfirmClearScores,
    Playing,
    Paused,
    GameOver
};

enum class MenuOption {
    Start = 0,
    Extras = 1,
    Options = 2,
    Exit = 3
};

enum class GameModeOption {
    Classic = 0,
    Sprint = 1,
    Challenge = 2,
    Practice = 3
};

enum class ClassicDifficulty {
    Easy = 0,
    Medium = 1,
    Hard = 2
};


inline const float* getGravityTable(ClassicDifficulty difficulty) {
    switch (difficulty) {
        case ClassicDifficulty::Easy:
            return LEVEL_GRAVITY_EASY;
        case ClassicDifficulty::Medium:
            return LEVEL_GRAVITY_MEDIUM;
        case ClassicDifficulty::Hard:
            return LEVEL_GRAVITY_HARD;
        default:
            return LEVEL_GRAVITY_MEDIUM;
    }
}

enum class PracticeDifficulty {
    VeryEasy = 0,
    Easy = 1,
    Medium = 2,
    Hard = 3
};

enum class PracticeLineGoal {
    Infinite = 0,
    Lines24 = 1,
    Lines48 = 2,
    Lines96 = 3
};

enum class PracticeStartLevel {
    Level0 = 0,
    Level1 = 1,
    Level2 = 2,
    Level3 = 3,
    Level4 = 4,
    Level5 = 5,
    Level6 = 6,
    Level7 = 7,
    Level8 = 8,
    Level9 = 9,
    Level10 = 10
};

enum class SprintLines {
    Lines1 = 0,
    Lines24 = 1,
    Lines48 = 2,
    Lines96 = 3
};

enum class ChallengeMode {
    Debug = 0,
    TheForest = 1,
    Randomness = 2,
    NonStraight = 3,
    OneRot = 4,
    ChristopherCurse = 5,
    Vanishing = 6,
    AutoDrop = 7
};

enum class ExtrasOption {
    TesseraPieces = 0,
    Achievements = 1,
    Statistics = 2,
    BestScores = 3,
    Back = 4
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
    

    float bestTimeSprint1 = 0.0f;
    float bestTimeSprint24 = 0.0f;
    float bestTimeSprint48 = 0.0f;
    float bestTimeSprint96 = 0.0f;
    

    float bestTimeChallengeDebug = 0.0f;
    float bestTimeChallengeTheForest = 0.0f;
    float bestTimeChallengeRandomness = 0.0f;
    float bestTimeChallengeNonStraight = 0.0f;
    float bestTimeChallengeOneRot = 0.0f;
    float bestTimeChallengeChristopherCurse = 0.0f;
    float bestTimeChallengeVanishing = 0.0f;
    float bestTimeChallengeAutoDrop = 0.0f;
    

    bool achievements[25] = {
        false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false, false,
        false, false, false, false, false
    };
    

    int totalLinesCleared = 0;
    int totalPiecesPlaced = 0;
    int totalGamesPlayed = 0;
    int totalScore = 0;
    int maxComboEver = 0;
    int totalBombsUsed = 0;
    float totalPlayTimeSeconds = 0.0f;
    int totalRotations = 0;
    int totalHolds = 0;
    int totalPerfectClears = 0;
    

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

    MuteIcon,
    TesseraLogo
    ,
    Button,
    ButtonActive
};


struct Cell {
    bool occupied = false;
    sf::Color color = sf::Color::Transparent;
    TextureType textureType = TextureType::Empty;
    float vanishTimer = 0.0f;
    bool isVanishing = false;
    
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
    float x, y;
    float timer;
    float rotation;
    float zDepth;
    ExplosionEffect(float posX, float posY, float rot = 0.0f, float z = 0.0f) 
        : x(posX), y(posY), timer(0.5f), rotation(rot), zDepth(z) {}
};


struct GlowEffect {
    float worldX, worldY;
    sf::Color color;
    float timer;
    float alpha;
    float rotation;
    
    GlowEffect(float wx, float wy, sf::Color col, float rot = 0.0f) 
        : worldX(wx), worldY(wy), color(col), timer(0.75f), alpha(128), rotation(rot) {}
    
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


struct AchievementPopup {
    Achievement achievement;
    std::string title;
    float timer;
    float slideProgress;
    bool sliding;
    
    AchievementPopup(Achievement ach, const std::string& achTitle)
        : achievement(ach), title(achTitle), timer(5.0f), slideProgress(0.0f), sliding(true) {}
    
    void update(float deltaTime) {
        if (sliding && slideProgress < 1.0f) {
            slideProgress += deltaTime * 2.0f;
            if (slideProgress > 1.0f) slideProgress = 1.0f;
        }
        timer -= deltaTime;
        if (timer < 0.5f && sliding) {
            sliding = false;
        }
    }
    
    bool isFinished() const {
        return timer <= 0.0f;
    }
    
    float getOffsetX(float windowWidth) const {
        const float popupWidth = 400.0f;
        const float rightMargin = 20.0f;
        float targetX = windowWidth - popupWidth - rightMargin;
        float startX = windowWidth;
        
        if (sliding) {
            return startX + (targetX - startX) * slideProgress;
        } else {
            float slideOut = (0.5f - timer) / 0.5f;
            return targetX + (startX - targetX) * slideOut;
        }
    }
};


struct BackgroundPiece {
    float x, y;
    float speed;
    float rotation;
    float rotationSpeed;
    PieceType type;
    float opacity;
    float brightness;
    
    BackgroundPiece(float startX, float startY, float fallSpeed, PieceType pieceType)
        : x(startX), y(startY), speed(fallSpeed), rotation(0.0f), 
          rotationSpeed((rand() % 4096 - 2048) / 100.0f), type(pieceType), opacity(1.0f) {


        float minSpeed = 32.0f;
        float maxSpeed = 288.0f;
        float minBrightness = 0.05f;
        float maxBrightness = 0.8f;
        float normalizedSpeed = (fallSpeed - minSpeed) / (maxSpeed - minSpeed);
        brightness = minBrightness + normalizedSpeed * (maxBrightness - minBrightness);
    }
    
    void update(float deltaTime) {
        y += speed * deltaTime;
        rotation += rotationSpeed * deltaTime;
    }
    
    bool isOffScreen() const {
        return y > 1080.0f + 100.0f;
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
    void reset(int startLevel);
    void returnPieceToBag(PieceType piece);
};


std::string getSaveFilePath();
void saveGameData(const SaveData& data);
SaveData loadGameData();
AbilityType getAbilityType(PieceType type);
int calculateLevel(int linesCleared);
