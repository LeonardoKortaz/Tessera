#pragma once

#include "types.h"
#include <vector>
#include <string>


struct BagLevelConfig {
    int basicPieces = 7;
    int mediumPieces = 0;
    int hardPieces = 0;
};


struct LevelThreshold {
    int lines;
    BagLevelConfig bagConfig;
};


struct DifficultyConfig {
    std::string modeName;
    int maxLevels;
    std::vector<LevelThreshold> levelThresholds;
    

    bool hasLineGoal;
    int lineGoal;
    

    bool bombEnabled;
    int bombInterval;
    bool holdEnabled;
    

    bool useCustomPieceFilter;
    std::vector<PieceType> allowedBasicPieces;
    std::vector<PieceType> allowedMediumPieces;
    std::vector<PieceType> allowedHardPieces;
    

    bool useRandomColorPalette;
    bool useTypeBasedColors;
    std::vector<sf::Color> colorPalette;
    

    std::vector<float> gravityTable;
    

    DifficultyConfig() 
        : modeName("Unknown"), maxLevels(0), hasLineGoal(false), 
          lineGoal(0), bombEnabled(false), bombInterval(10), holdEnabled(true),
          useCustomPieceFilter(false), useRandomColorPalette(false), useTypeBasedColors(false) {}
};


namespace GravityPresets {
    const std::vector<float> NORMAL = {1.5f, 1.75f, 2.0f, 2.5f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.5f, 10.0f};
    const std::vector<float> HARD = {2.0f, 2.5f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.5f, 10.0f, 12.0f, 14.0f};
    const std::vector<float> DEFAULT = {1.5f, 1.75f, 2.0f, 2.5f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.5f, 10.0f};
}



inline DifficultyConfig getPracticeConfig(PracticeDifficulty difficulty, PracticeLineGoal lineGoal, bool infiniteBombs) {
    DifficultyConfig config;
    config.modeName = "Practice";
    config.maxLevels = 10;
    config.hasLineGoal = (lineGoal != PracticeLineGoal::Infinite);
    

    switch (lineGoal) {
        case PracticeLineGoal::Infinite:
            config.lineGoal = 0;
            break;
        case PracticeLineGoal::Lines24:
            config.lineGoal = 24;
            break;
        case PracticeLineGoal::Lines48:
            config.lineGoal = 48;
            break;
        case PracticeLineGoal::Lines96:
            config.lineGoal = 96;
            break;
    }
    
    config.bombEnabled = true;
    config.bombInterval = infiniteBombs ? 0 : 10;
    config.holdEnabled = true;
    

    switch (difficulty) {
        case PracticeDifficulty::VeryEasy:
        case PracticeDifficulty::Easy:
        case PracticeDifficulty::Medium:
            config.gravityTable = GravityPresets::NORMAL;
            break;
        case PracticeDifficulty::Hard:
            config.gravityTable = GravityPresets::HARD;
            break;
    }

    switch (difficulty) {
        case PracticeDifficulty::VeryEasy:
            config.levelThresholds = {
                {0,   {7, 0, 0}},
                {9,   {7, 0, 0}},
                {25,  {7, 0, 0}},
                {49,  {7, 0, 0}},
                {81,  {7, 0, 0}},
                {121, {7, 0, 0}},
                {169, {7, 0, 0}},
                {225, {7, 0, 0}},
                {289, {7, 0, 0}},
                {361, {7, 0, 0}},
                {441, {7, 0, 0}}
            };
            break;
        case PracticeDifficulty::Easy:
            config.levelThresholds = {
                {0,   {7, 0, 0}},
                {9,   {7, 0, 0}},
                {25,  {7, 1, 0}},
                {49,  {7, 1, 0}},
                {81,  {7, 0, 1}},
                {121, {7, 1, 1}},
                {169, {7, 1, 1}},
                {225, {7, 1, 1}},
                {289, {7, 1, 1}},
                {361, {7, 1, 1}},
                {441, {7, 1, 1}}
            };
            break;
        case PracticeDifficulty::Medium:
            config.levelThresholds = {
                {0,   {7, 0, 0}},
                {9,   {7, 1, 0}},
                {25,  {7, 0, 1}},
                {49,  {7, 1, 1}},
                {81,  {7, 2, 1}},
                {121, {7, 2, 2}},
                {169, {7, 2, 2}},
                {225, {7, 2, 2}},
                {289, {7, 2, 2}},
                {361, {7, 2, 2}},
                {441, {7, 2, 2}}
            };
            break;
        case PracticeDifficulty::Hard:
            config.levelThresholds = {
                {0,   {7, 0, 0}},
                {9,   {7, 1, 0}},
                {25,  {7, 1, 1}},
                {49,  {7, 2, 1}},
                {81,  {7, 2, 2}},
                {121, {7, 3, 3}},
                {169, {7, 3, 3}},
                {225, {7, 3, 3}},
                {289, {7, 3, 3}},
                {361, {7, 3, 3}},
                {441, {7, 3, 3}}
            };
            break;
    }
    
    return config;
}

inline DifficultyConfig getClassicNormalConfig() {
    DifficultyConfig config;
    config.modeName = "Classic Normal";
    config.maxLevels = 10;
    config.hasLineGoal = false;
    config.lineGoal = 0;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    
    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 1, 0}},
        {25,  {7, 0, 1}},
        {49,  {7, 1, 1}},
        {81,  {7, 2, 1}},
        {121, {7, 1, 2}},
        {169, {7, 2, 2}},
        {225, {7, 2, 2}},
        {289, {7, 2, 2}},
        {361, {7, 2, 2}},
        {441, {7, 2, 2}}
    };
    
    return config;
}

inline DifficultyConfig getClassicHardConfig() {
    DifficultyConfig config;
    config.modeName = "Classic Hard";
    config.maxLevels = 10;
    config.hasLineGoal = false;
    config.lineGoal = 0;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::HARD;
    
    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 1, 0}},
        {25,  {7, 1, 1}},
        {49,  {7, 2, 1}},
        {81,  {7, 2, 2}},
        {121, {7, 3, 2}},
        {169, {7, 3, 3}},
        {225, {7, 3, 3}},
        {289, {7, 3, 3}},
        {361, {7, 3, 3}},
        {441, {7, 3, 3}}
    };
    
    return config;
}



inline DifficultyConfig getSprint24Config() {
    DifficultyConfig config;
    config.modeName = "Sprint 24 Lines";
    config.maxLevels = 1;
    config.hasLineGoal = true;
    config.lineGoal = 24;
    config.bombEnabled = false;
    config.bombInterval = 0;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    
    config.levelThresholds = {
        {0,  {7, 0, 0}},
        {12, {7, 1, 0}}
    };
    
    return config;
}

inline DifficultyConfig getSprint24ConfigOld() {
    DifficultyConfig config;
    config.modeName = "Sprint 24 Lines";
    config.maxLevels = 1;
    config.hasLineGoal = true;
    config.lineGoal = 24;
    config.bombEnabled = false;
    config.bombInterval = 0;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    
    config.levelThresholds = {
        {0,  {7, 0, 0}},
        {12, {7, 1, 0}}
    };
    
    return config;
}

inline DifficultyConfig getSprint48Config() {
    DifficultyConfig config;
    config.modeName = "Sprint 48 Lines";
    config.maxLevels = 2;
    config.hasLineGoal = true;
    config.lineGoal = 48;
    config.bombEnabled = false;
    config.bombInterval = 0;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    
    config.levelThresholds = {
        {0,  {7, 0, 0}},
        {16, {7, 1, 0}},
        {32, {7, 1, 1}}
    };
    
    return config;
}

inline DifficultyConfig getSprint96Config() {
    DifficultyConfig config;
    config.modeName = "Sprint 96 Lines";
    config.maxLevels = 3;
    config.hasLineGoal = true;
    config.lineGoal = 96;
    config.bombEnabled = false;
    config.bombInterval = 0;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    
    config.levelThresholds = {
        {0,  {7, 0, 0}},
        {24, {7, 1, 0}},
        {48, {7, 1, 1}},
        {72, {7, 2, 1}}
    };
    
    return config;
}


inline DifficultyConfig getChallengeDebugConfig() {
    DifficultyConfig config;
    config.modeName = "DEBUG";
    config.maxLevels = 1;
    config.hasLineGoal = true;
    config.lineGoal = 1;
    config.bombEnabled = true;
    config.bombInterval = 1;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic
    };
    config.allowedMediumPieces = {};
    config.allowedHardPieces = {};

    config.levelThresholds = {
        {0, {7, 0, 0}}
    };
    
    return config;
}

inline DifficultyConfig getChallengeTheForestConfig() {
    DifficultyConfig config;
    config.modeName = "The Forest";
    config.maxLevels = 10;
    config.hasLineGoal = true;
    config.lineGoal = 96;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    


    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::L_Basic, 
        PieceType::J_Basic, 
        PieceType::I_Basic
    };
    config.allowedMediumPieces = {
        PieceType::I_Medium,
        PieceType::T_Medium
    };
    config.allowedHardPieces = {
        PieceType::L_Hard, 
        PieceType::J_Hard, 
        PieceType::I_Hard
    };




    config.useRandomColorPalette = false;
    config.useTypeBasedColors = true;
    config.colorPalette = {
        sf::Color(0x38, 0xb7, 0x55),
        sf::Color(0x4b, 0x82, 0x15),
        sf::Color(0xb4, 0x82, 0x2b),
        sf::Color(0x86, 0x3c, 0x00),
        sf::Color(0x27, 0x4f, 0x00),
        sf::Color(0xd3, 0xe5, 0x3e),
        sf::Color(0x5a, 0x6f, 0x28)
    };


    config.levelThresholds = {
        {0,   {3, 2, 3}},
        {9,   {3, 2, 3}},
        {25,  {3, 2, 3}},
        {49,  {3, 2, 3}},
        {81,  {3, 2, 3}},
        {121, {3, 2, 3}},
        {169, {3, 2, 3}},
        {225, {3, 2, 3}},
        {289, {3, 2, 3}},
        {361, {3, 2, 3}},
        {441, {3, 2, 3}}
    };
    
    return config;
}

inline DifficultyConfig getChallengeRandomnessConfig() {
    DifficultyConfig config;
    config.modeName = "Randomness";
    config.maxLevels = 5;
    config.hasLineGoal = true;
    config.lineGoal = 96;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic
    };
    config.allowedMediumPieces = {
        PieceType::I_Medium, PieceType::T_Medium, PieceType::L_Medium, 
        PieceType::J_Medium, PieceType::O_Medium, PieceType::S_Medium, PieceType::Z_Medium
    };
    config.allowedHardPieces = {
        PieceType::I_Hard, PieceType::T_Hard, PieceType::L_Hard, 
        PieceType::J_Hard, PieceType::O_Hard, PieceType::S_Hard, PieceType::Z_Hard,
        PieceType::A_Bomb
    };

    config.levelThresholds = {
        {0,   {5, 2, 2}},
        {9,   {5, 2, 2}},
        {25,  {5, 3, 2}},
        {49,  {5, 3, 3}},
        {81,  {5, 4, 3}},
        {121, {5, 4, 4}},
        {169, {5, 4, 4}},
        {225, {5, 4, 4}},
        {289, {5, 4, 4}},
        {361, {5, 4, 4}},
        {441, {5, 4, 4}}
    };
    
    return config;
}

inline DifficultyConfig getChallengeNonStraightConfig() {
    DifficultyConfig config;
    config.modeName = "Non Straight";
    config.maxLevels = 4;
    config.hasLineGoal = true;
    config.lineGoal = 24;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::T_Basic, 
        PieceType::O_Basic, 
        PieceType::S_Basic, 
        PieceType::Z_Basic
    };
    config.allowedMediumPieces = {
        PieceType::T_Medium, 
        PieceType::O_Medium, 
        PieceType::S_Medium, 
        PieceType::Z_Medium
    };
    config.allowedHardPieces = {
        PieceType::T_Hard, 
        PieceType::O_Hard, 
        PieceType::S_Hard, 
        PieceType::Z_Hard
    };

    config.levelThresholds = {
        {0,   {4, 0, 0}},
        {9,   {4, 0, 0}},
        {25,  {4, 1, 0}},
        {49,  {4, 1, 0}},
        {81,  {4, 1, 0}},
        {121, {4, 1, 0}},
        {169, {4, 1, 0}},
        {225, {4, 1, 0}},
        {289, {4, 1, 0}},
        {361, {4, 1, 0}},
        {441, {4, 1, 0}}
    };
    
    return config;
}

inline DifficultyConfig getChallengeOneRotConfig() {
    DifficultyConfig config;
    config.modeName = "OneRot";
    config.maxLevels = 5;
    config.hasLineGoal = true;
    config.lineGoal = 48;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic
    };
    config.allowedMediumPieces = {};
    config.allowedHardPieces = {};

    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 0, 0}},
        {25,  {7, 0, 0}},
        {49,  {7, 0, 0}},
        {81,  {7, 0, 0}},
        {121, {7, 0, 0}},
        {169, {7, 0, 0}},
        {225, {7, 0, 0}},
        {289, {7, 0, 0}},
        {361, {7, 0, 0}},
        {441, {7, 0, 0}}
    };
    
    return config;
}

inline DifficultyConfig getChallengeChristopherCurseConfig() {
    DifficultyConfig config;
    config.modeName = "Christopher Curse";
    config.maxLevels = 5;
    config.hasLineGoal = true;
    config.lineGoal = 48;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic
    };
    config.allowedMediumPieces = {};
    config.allowedHardPieces = {
        PieceType::T_Hard
    };

    config.levelThresholds = {
        {0,   {7, 0, 3}},
        {9,   {7, 0, 3}},
        {25,  {7, 0, 3}},
        {49,  {7, 0, 3}},
        {81,  {7, 0, 3}},
        {121, {7, 0, 3}},
        {169, {7, 0, 3}},
        {225, {7, 0, 3}},
        {289, {7, 0, 3}},
        {361, {7, 0, 3}},
        {441, {7, 0, 3}}
    };
    
    return config;
}

inline DifficultyConfig getChallengeVanishingConfig() {
    DifficultyConfig config;
    config.modeName = "Vanishing";
    config.maxLevels = 10;
    config.hasLineGoal = true;
    config.lineGoal = 48;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic
    };
    config.allowedMediumPieces = {};
    config.allowedHardPieces = {};


    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 1, 0}},
        {25,  {7, 0, 1}},
        {49,  {7, 1, 1}},
        {81,  {7, 1, 1}},
        {121, {7, 1, 1}},
        {169, {7, 1, 1}},
        {225, {7, 1, 1}},
        {289, {7, 1, 1}},
        {361, {7, 1, 1}},
        {441, {7, 1, 1}}
    };
    
    return config;
}

inline DifficultyConfig getChallengeAutoDropConfig() {
    DifficultyConfig config;
    config.modeName = "Auto Drop";
    config.maxLevels = 10;
    config.hasLineGoal = true;
    config.lineGoal = 48;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic
    };
    config.allowedMediumPieces = {};
    config.allowedHardPieces = {};


    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 1, 0}},
        {25,  {7, 0, 1}},
        {49,  {7, 1, 1}},
        {81,  {7, 1, 1}},
        {121, {7, 1, 1}},
        {169, {7, 1, 1}},
        {225, {7, 1, 1}},
        {289, {7, 1, 1}},
        {361, {7, 1, 1}},
        {441, {7, 1, 1}}
    };
    
    return config;
}

inline DifficultyConfig getChallengeGravityFlipConfig() {
    DifficultyConfig config;
    config.modeName = "Gravity Flip";
    config.maxLevels = 10;
    config.hasLineGoal = true;
    config.lineGoal = 40;
    config.bombEnabled = true;
    config.bombInterval = 15;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic
    };
    config.allowedMediumPieces = {};
    config.allowedHardPieces = {};

    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 1, 0}},
        {25,  {7, 0, 1}},
        {49,  {7, 1, 1}},
        {81,  {7, 1, 1}},
        {121, {7, 1, 1}},
        {169, {7, 1, 1}},
        {225, {7, 1, 1}},
        {289, {7, 1, 1}},
        {361, {7, 1, 1}},
        {441, {7, 1, 1}}
    };
    
    return config;
}

inline DifficultyConfig getChallengePetrifyConfig() {
    DifficultyConfig config;
    config.modeName = "Petrify";
    config.maxLevels = 10;
    config.hasLineGoal = true;
    config.lineGoal = 48;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    config.gravityTable = GravityPresets::NORMAL;
    
    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::I_Basic, PieceType::T_Basic, PieceType::L_Basic, 
        PieceType::J_Basic, PieceType::O_Basic, PieceType::S_Basic, PieceType::Z_Basic
    };
    config.allowedMediumPieces = {};
    config.allowedHardPieces = {};

    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 1, 0}},
        {25,  {7, 0, 1}},
        {49,  {7, 1, 1}},
        {81,  {7, 1, 1}},
        {121, {7, 1, 1}},
        {169, {7, 1, 1}},
        {225, {7, 1, 1}},
        {289, {7, 1, 1}},
        {361, {7, 1, 1}},
        {441, {7, 1, 1}}
    };
    
    return config;
}


inline const DifficultyConfig* getDifficultyConfig(GameModeOption mode, ClassicDifficulty classic, SprintLines sprint, ChallengeMode challenge, PracticeDifficulty practiceDiff = PracticeDifficulty::Easy, PracticeLineGoal practiceGoal = PracticeLineGoal::Infinite, bool practiceInfBombs = false) {
    switch (mode) {
        case GameModeOption::Classic:
            switch (classic) {
                case ClassicDifficulty::Normal: {
                    static const DifficultyConfig config = getClassicNormalConfig();
                    return &config;
                }
                case ClassicDifficulty::Hard: {
                    static const DifficultyConfig config = getClassicHardConfig();
                    return &config;
                }
                default: {
                    static const DifficultyConfig config = getClassicNormalConfig();
                    return &config;
                }
            }
        
        case GameModeOption::Sprint:
            switch (sprint) {
                case SprintLines::Lines24: {
                    static const DifficultyConfig config = getSprint24Config();
                    return &config;
                }
                case SprintLines::Lines48: {
                    static const DifficultyConfig config = getSprint48Config();
                    return &config;
                }
                case SprintLines::Lines96: {
                    static const DifficultyConfig config = getSprint96Config();
                    return &config;
                }
                default: {
                    static const DifficultyConfig config = getSprint24Config();
                    return &config;
                }
            }
        
        case GameModeOption::Challenge:
            switch (challenge) {
                case ChallengeMode::Debug: {
                    static const DifficultyConfig config = getChallengeDebugConfig();
                    return &config;
                }
                case ChallengeMode::TheForest: {
                    static const DifficultyConfig config = getChallengeTheForestConfig();
                    return &config;
                }
                case ChallengeMode::Randomness: {
                    static const DifficultyConfig config = getChallengeRandomnessConfig();
                    return &config;
                }
                case ChallengeMode::NonStraight: {
                    static const DifficultyConfig config = getChallengeNonStraightConfig();
                    return &config;
                }
                case ChallengeMode::OneRot: {
                    static const DifficultyConfig config = getChallengeOneRotConfig();
                    return &config;
                }
                case ChallengeMode::ChristopherCurse: {
                    static const DifficultyConfig config = getChallengeChristopherCurseConfig();
                    return &config;
                }
                case ChallengeMode::Vanishing: {
                    static const DifficultyConfig config = getChallengeVanishingConfig();
                    return &config;
                }
                case ChallengeMode::AutoDrop: {
                    static const DifficultyConfig config = getChallengeAutoDropConfig();
                    return &config;
                }
                case ChallengeMode::GravityFlip: {
                    static const DifficultyConfig config = getChallengeGravityFlipConfig();
                    return &config;
                }
                case ChallengeMode::Petrify: {
                    static const DifficultyConfig config = getChallengePetrifyConfig();
                    return &config;
                }
                default: {
                    static const DifficultyConfig config = getChallengeDebugConfig();
                    return &config;
                }
            }
        
        case GameModeOption::Practice: {
            static DifficultyConfig config = getPracticeConfig(practiceDiff, practiceGoal, practiceInfBombs);
            config = getPracticeConfig(practiceDiff, practiceGoal, practiceInfBombs);
            return &config;
        }
        
        default: {
            static const DifficultyConfig config = getClassicNormalConfig();
            return &config;
        }
    }
}
