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
    

    DifficultyConfig() 
        : modeName("Unknown"), maxLevels(0), hasLineGoal(false), 
          lineGoal(0), bombEnabled(false), bombInterval(10), holdEnabled(true),
          useCustomPieceFilter(false) {}
};



inline DifficultyConfig getClassicEasyConfig() {
    DifficultyConfig config;
    config.modeName = "Classic Easy";
    config.maxLevels = 5;
    config.hasLineGoal = false;
    config.lineGoal = 0;
    config.bombEnabled = true;
    config.bombInterval = 5;
    config.holdEnabled = true;
    
    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 0, 0}},
        {25,  {7, 1, 0}},
        {49,  {7, 1, 0}},
        {81,  {7, 0, 1}},
        {121, {7, 1, 1}}
    };
    
    return config;
}

inline DifficultyConfig getClassicMediumConfig() {
    DifficultyConfig config;
    config.modeName = "Classic Medium";
    config.maxLevels = 5;
    config.hasLineGoal = false;
    config.lineGoal = 0;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    
    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 1, 0}},
        {25,  {7, 0, 1}},
        {49,  {7, 1, 1}},
        {81,  {7, 2, 1}},
        {121, {7, 2, 2}}
    };
    
    return config;
}

inline DifficultyConfig getClassicHardConfig() {
    DifficultyConfig config;
    config.modeName = "Classic Hard";
    config.maxLevels = 5;
    config.hasLineGoal = false;
    config.lineGoal = 0;
    config.bombEnabled = true;
    config.bombInterval = 10;
    config.holdEnabled = true;
    
    config.levelThresholds = {
        {0,   {7, 0, 0}},
        {9,   {7, 1, 0}},
        {25,  {7, 1, 1}},
        {49,  {7, 2, 1}},
        {81,  {7, 2, 2}},
        {121, {7, 3, 3}}
    };
    
    return config;
}



inline DifficultyConfig getSprint1Config() {
    DifficultyConfig config;
    config.modeName = "Sprint 1 Line (DEBUG)";
    config.maxLevels = 0;
    config.hasLineGoal = true;
    config.lineGoal = 1;
    config.bombEnabled = false;
    config.bombInterval = 0;
    config.holdEnabled = true;
    
    config.levelThresholds = {
        {0, {7, 0, 0}}
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
    

    config.useCustomPieceFilter = true;
    config.allowedBasicPieces = {
        PieceType::L_Basic, 
        PieceType::J_Basic, 
        PieceType::T_Basic, 
        PieceType::I_Basic
    };
    config.allowedMediumPieces = {
        PieceType::L_Medium, 
        PieceType::J_Medium, 
        PieceType::T_Medium, 
        PieceType::I_Medium
    };
    config.allowedHardPieces = {
        PieceType::L_Hard, 
        PieceType::J_Hard, 
        PieceType::T_Hard, 
        PieceType::I_Hard
    };

    config.levelThresholds = {
        {0,  {4, 0, 0}},
        {10, {4, 0, 0}},
        {25, {4, 1, 0}},
        {50, {4, 1, 1}},
        {75, {4, 2, 1}},
        {100, {4, 2, 2}}
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
        {0,  {5, 2, 2}},
        {10, {5, 2, 2}},
        {25, {5, 3, 2}},
        {50, {5, 3, 3}},
        {75, {5, 4, 3}},
        {100, {5, 4, 4}}
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
        {0,  {4, 0, 0}},
        {10, {4, 0, 0}},
        {25, {4, 1, 0}},
        {50, {4, 1, 1}},
        {75, {4, 2, 1}},
        {100, {4, 2, 2}}
    };
    
    return config;
}



inline const DifficultyConfig* getDifficultyConfig(GameModeOption mode, ClassicDifficulty classic, SprintLines sprint, ChallengeMode challenge) {
    switch (mode) {
        case GameModeOption::Classic:
            switch (classic) {
                case ClassicDifficulty::Easy: {
                    static const DifficultyConfig config = getClassicEasyConfig();
                    return &config;
                }
                case ClassicDifficulty::Medium: {
                    static const DifficultyConfig config = getClassicMediumConfig();
                    return &config;
                }
                case ClassicDifficulty::Hard: {
                    static const DifficultyConfig config = getClassicHardConfig();
                    return &config;
                }
                default: {
                    static const DifficultyConfig config = getClassicMediumConfig();
                    return &config;
                }
            }
        
        case GameModeOption::Sprint:
            switch (sprint) {
                case SprintLines::Lines1: {
                    static const DifficultyConfig config = getSprint1Config();
                    return &config;
                }
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
                default: {
                    static const DifficultyConfig config = getChallengeDebugConfig();
                    return &config;
                }
            }
        
        default: {
            static const DifficultyConfig config = getClassicMediumConfig();
            return &config;
        }
    }
}
