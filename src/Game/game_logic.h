#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "types.h"
#include "game_mode_theme.h"
#include "audio_manager.h"
#include "difficulty_config.h"
#include <array>
#include <vector>






int calculateLevel(int linesCleared);


int calculateScore(int linesCleared);


int getHighScoreForMode(const SaveData& saveData, ClassicDifficulty difficulty);


float getBestTimeForMode(const SaveData& saveData, GameModeOption mode, SprintLines sprintLines, ChallengeMode challengeMode);






int findFirstFilledRow(const PieceShape& shape);



int getColorIndexForPieceType(PieceType type);


std::string getChallengeModeString(ChallengeMode mode);







int clearFullLines(
    std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid,
    AudioManager& audioManager,
    std::vector<GlowEffect>* glowEffects = nullptr,
    float* shakeIntensity = nullptr,
    float* shakeDuration = nullptr,
    float* shakeTimer = nullptr,
    bool isPetrifyMode = false,
    std::vector<ThermometerParticle>* thermometerParticles = nullptr,
    bool isRaceMode = false
);


void updatePetrifyCounters(std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT>& grid);






void applyGameTheme(
    GameModeTheme& currentTheme,
    AudioManager& audioManager,
    GameModeOption mode,
    ClassicDifficulty classicDiff,
    ChallengeMode challengeMode,
    GameThemeChoice themeChoice = GameThemeChoice::Classic
);






void unlockAchievement(
    SaveData& saveData,
    Achievement ach,
    std::vector<AchievementPopup>* popups = nullptr,
    AudioManager* audioManager = nullptr
);

#endif
