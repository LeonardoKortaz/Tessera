#include "game_logic.h"
#include "piece_utils.h"
#include "save_system.h"
#include "achievements.h"
#include <iostream>
#include <cstdlib>





int calculateLevel(int linesCleared) {
    for (int level = MAX_LEVEL; level >= 1; level--) {
        if (linesCleared >= LEVEL_THRESHOLDS[level]) {
            return level;
        }
    }
    return 0;
}

int calculateScore(int linesCleared) {
    if (linesCleared == 0) return 0;
    int baseScore = 1000;
    int bonus = (linesCleared - 1) * 250;
    int scorePerLine = baseScore + bonus;
    
    return linesCleared * scorePerLine;
}

int getHighScoreForMode(const SaveData& saveData, ClassicDifficulty difficulty) {
    switch (difficulty) {
        case ClassicDifficulty::Normal: return saveData.highScoreClassicNormal;
        case ClassicDifficulty::Hard: return saveData.highScoreClassicHard;
        default: return 0;
    }
}

float getBestTimeForMode(const SaveData& saveData, GameModeOption mode, SprintLines sprintLines, ChallengeMode challengeMode) {
    if (mode == GameModeOption::Sprint) {
        switch (sprintLines) {
            case SprintLines::Lines1: return saveData.bestTimeSprint1;
            case SprintLines::Lines24: return saveData.bestTimeSprint24;
            case SprintLines::Lines48: return saveData.bestTimeSprint48;
            case SprintLines::Lines96: return saveData.bestTimeSprint96;
            default: return 0.0f;
        }
    } else if (mode == GameModeOption::Challenge) {
        switch (challengeMode) {
            case ChallengeMode::Debug: return saveData.bestTimeChallengeDebug;
            case ChallengeMode::TheForest: return saveData.bestTimeChallengeTheForest;
            case ChallengeMode::Randomness: return saveData.bestTimeChallengeRandomness;
            case ChallengeMode::NonStraight: return saveData.bestTimeChallengeNonStraight;
            case ChallengeMode::OneRot: return saveData.bestTimeChallengeOneRot;
            case ChallengeMode::ChristopherCurse: return saveData.bestTimeChallengeChristopherCurse;
            case ChallengeMode::Vanishing: return saveData.bestTimeChallengeVanishing;
            case ChallengeMode::AutoDrop: return saveData.bestTimeChallengeAutoDrop;
            case ChallengeMode::GravityFlip: return saveData.bestTimeChallengeGravityFlip;
            case ChallengeMode::Petrify: return saveData.bestTimeChallengePetrify;
            default: return 0.0f;
        }
    }
    return 0.0f;
}





int findFirstFilledRow(const PieceShape& shape) {
    for (int y = 0; y < shape.height; y++) {
        for (int x = 0; x < shape.width; x++) {
            if (shape.blocks[y][x]) {
                return y;
            }
        }
    }
    return 0;
}

int getColorIndexForPieceType(PieceType type) {
    switch (type) {
        case PieceType::I_Basic:
        case PieceType::I_Medium:
        case PieceType::I_Hard:
            return 0;
        case PieceType::T_Basic:
        case PieceType::T_Medium:
        case PieceType::T_Hard:
            return 1;
        case PieceType::L_Basic:
        case PieceType::L_Medium:
        case PieceType::L_Hard:
            return 2;
        case PieceType::J_Basic:
        case PieceType::J_Medium:
        case PieceType::J_Hard:
            return 3;
        case PieceType::O_Basic:
        case PieceType::O_Medium:
        case PieceType::O_Hard:
            return 4;
        case PieceType::S_Basic:
        case PieceType::S_Medium:
        case PieceType::S_Hard:
            return 5;
        case PieceType::Z_Basic:
        case PieceType::Z_Medium:
        case PieceType::Z_Hard:
            return 6;
        default:
            return 0;
    }
}

std::string getChallengeModeString(ChallengeMode mode) {
    switch (mode) {
        case ChallengeMode::Debug: return "DEBUG";
        case ChallengeMode::TheForest: return "THE FOREST";
        case ChallengeMode::Randomness: return "RANDOMNESS";
        case ChallengeMode::NonStraight: return "NON-STRAIGHT";
        case ChallengeMode::OneRot: return "ONE ROTATION";
        case ChallengeMode::ChristopherCurse: return "THE CURSE";
        case ChallengeMode::Vanishing: return "VANISHING";
        case ChallengeMode::AutoDrop: return "AUTO DROP";
        case ChallengeMode::GravityFlip: return "GRAVITY FLIP";
        case ChallengeMode::Petrify: return "PETRIFY";
        default: return "UNKNOWN";
    }
}





void updatePetrifyCounters(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid) {
    const sf::Color STONE_COLOR = sf::Color(80, 80, 90);
    
    for (int row = 0; row < GRID_HEIGHT; row++) {
        for (int col = 0; col < GRID_WIDTH; col++) {
            if (grid[row][col].occupied && !grid[row][col].isPetrified) {
                grid[row][col].petrifyCounter++;
                
                if (grid[row][col].petrifyCounter >= 12) {
                    grid[row][col].isPetrified = true;
                    grid[row][col].color = STONE_COLOR;
                    grid[row][col].textureType = TextureType::GenericBlock;
                }
            }
        }
    }
}

int clearFullLines(
    std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid,
    AudioManager& audioManager,
    std::vector<GlowEffect>* glowEffects,
    float* shakeIntensity,
    float* shakeDuration,
    float* shakeTimer,
    bool isPetrifyMode,
    std::vector<ThermometerParticle>* thermometerParticles,
    bool isRaceMode
) {
    int linesCleared = 0;
    

    float thermometerX = GRID_OFFSET_X - 95 + 25;
    float thermometerBottomY = GRID_OFFSET_Y + GRID_HEIGHT * CELL_SIZE - 30;

    std::vector<std::pair<int, int>> fullLineRows;
    for (int row = GRID_HEIGHT - 1; row >= 0; row--) {
        bool isFullLine = true;
        bool hasPetrified = false;
        for (int col = 0; col < GRID_WIDTH; col++) {
            if (!grid[row][col].occupied) {
                isFullLine = false;
                break;
            }
            if (isPetrifyMode && grid[row][col].isPetrified) {
                hasPetrified = true;
            }
        }
        if (isPetrifyMode && hasPetrified) {
            isFullLine = false;
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
                    float glowRotation = static_cast<float>(rand() % 360);
                    
                    glowEffects->push_back(GlowEffect(baseWorldX + offsetX, baseWorldY + offsetY, blockColor, glowRotation));
                }
            }
            
            if (thermometerParticles && isRaceMode) {
                for (int col = 0; col < GRID_WIDTH; col++) {
                    float startX = GRID_OFFSET_X + col * CELL_SIZE + CELL_SIZE / 2;
                    float startY = GRID_OFFSET_Y + row * CELL_SIZE + CELL_SIZE / 2;
                    float targetY = thermometerBottomY - (rand() % 30);
                    thermometerParticles->push_back(ThermometerParticle(startX, startY, thermometerX, targetY, sf::Color::White));
                }
            }
        }
    }
    
    for (int row = GRID_HEIGHT - 1; row >= 0; row--) {
        bool isFullLine = true;
        bool hasPetrified = false;
        for (int col = 0; col < GRID_WIDTH; col++) {
            if (!grid[row][col].occupied) {
                isFullLine = false;
                break;
            }
            if (isPetrifyMode && grid[row][col].isPetrified) {
                hasPetrified = true;
            }
        }
        if (isPetrifyMode && hasPetrified) {
            isFullLine = false;
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
    
    if (linesCleared > 0) { 
        audioManager.playLineClearSound(); 
    }

    if (linesCleared > 0 && shakeIntensity && shakeDuration && shakeTimer) {
        float intensity = 5.0f + (linesCleared * 2.5f);
        *shakeIntensity = intensity;
        *shakeDuration = 0.3f;
        *shakeTimer = 0.0f;
    }
    
    if (linesCleared >= 4) {
        int randomWow = rand() % 3;
        audioManager.playWowSound(randomWow);
    }
    
    return linesCleared;
}





void applyGameTheme(
    GameModeTheme& currentTheme,
    AudioManager& audioManager,
    GameModeOption mode,
    ClassicDifficulty classicDiff,
    ChallengeMode challengeMode,
    GameThemeChoice themeChoice
) {
    currentTheme = GameThemes::getThemeForGameMode(mode, classicDiff, challengeMode, themeChoice);
    audioManager.playMusicFromPath(currentTheme.musicPath, currentTheme.musicVolume);
    audioManager.setLineClearSound(currentTheme.lineClearSoundPath);
    audioManager.setDropSound(currentTheme.dropSoundPath);
}





void unlockAchievement(
    SaveData& saveData,
    Achievement ach,
    std::vector<AchievementPopup>* popups,
    AudioManager* audioManager
) {
    if (tryUnlockAchievement(saveData, ach)) {
        saveGameData(saveData);
        
        if (popups) {
            AchievementInfo info = getAchievementInfo(ach);
            popups->emplace_back(ach, info.title);
        }
        
        if (audioManager) {
            audioManager->playAchievementSound();
        }
    }
}
