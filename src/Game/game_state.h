#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "types.h"
#include "difficulty_config.h"
#include <array>
#include <algorithm>






struct GameSession {

    std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> grid;
    

    int score = 0;
    int linesCleared = 0;
    int currentLevel = 0;
    

    float elapsedTime = 0.0f;
    float gameOverMusicTimer = 0.0f;
    

    bool gameOver = false;
    bool gameWin = false;
    bool gameOverSoundPlayed = false;
    bool gamePaused = false;
    

    bool bombAvailable = false;
    bool bombUsedThisGame = false;
    int piecesPlacedSinceBomb = 0;
    

    PieceType heldPiece = PieceType::I_Basic;
    bool hasHeldPiece = false;
    bool canUseHold = true;
    

    bool sprintModeActive = false;
    bool challengeModeActive = false;
    int sprintLinesGoal = 0;
    int challengeLinesGoal = 0;
    

    int currentPieceRotations = 0;
    

    bool gravityFlipped = false;
    int gravityFlipPieceCount = 0;
    

    float leftHoldTime = 0.0f;
    float rightHoldTime = 0.0f;
    float dasTimer = 0.0f;
    bool leftPressed = false;
    bool rightPressed = false;
    

    void reset() {

        for (auto& row : grid) {
            for (auto& cell : row) {
                cell = Cell();
            }
        }
        
        score = 0;
        linesCleared = 0;
        currentLevel = 0;
        elapsedTime = 0.0f;
        gameOverMusicTimer = 0.0f;
        
        gameOver = false;
        gameWin = false;
        gameOverSoundPlayed = false;
        gamePaused = false;
        
        bombAvailable = false;
        bombUsedThisGame = false;
        piecesPlacedSinceBomb = 0;
        
        hasHeldPiece = false;
        canUseHold = true;
        
        sprintModeActive = false;
        challengeModeActive = false;
        sprintLinesGoal = 0;
        challengeLinesGoal = 0;
        
        currentPieceRotations = 0;
        gravityFlipped = false;
        gravityFlipPieceCount = 0;
        
        leftHoldTime = 0.0f;
        rightHoldTime = 0.0f;
        dasTimer = 0.0f;
        leftPressed = false;
        rightPressed = false;
    }
    

    void initForMode(GameModeOption mode, const DifficultyConfig* config) {
        reset();
        
        if (config) {
            if (mode == GameModeOption::Sprint) {
                sprintModeActive = true;
                sprintLinesGoal = config->lineGoal;
            } else if (mode == GameModeOption::Challenge) {
                challengeModeActive = true;
                challengeLinesGoal = config->lineGoal;
            }
        }
    }
};






struct EffectsState {

    float shakeIntensity = 0.0f;
    float shakeDuration = 0.0f;
    float shakeTimer = 0.0f;
    

    std::vector<ExplosionEffect> explosionEffects;
    std::vector<GlowEffect> glowEffects;
    std::vector<ThermometerParticle> thermometerParticles;
    std::vector<AchievementPopup> achievementPopups;
    

    float volumeChangeTimer = 0.0f;
    
    void clear() {
        shakeIntensity = 0.0f;
        shakeDuration = 0.0f;
        shakeTimer = 0.0f;
        explosionEffects.clear();
        glowEffects.clear();
        thermometerParticles.clear();
        achievementPopups.clear();
        volumeChangeTimer = 0.0f;
    }
    
    void update(float deltaTime) {

        if (shakeDuration > 0.0f) {
            shakeTimer += deltaTime;
            if (shakeTimer >= shakeDuration) {
                shakeIntensity = 0.0f;
                shakeDuration = 0.0f;
                shakeTimer = 0.0f;
            }
        }
        

        if (volumeChangeTimer > 0.0f) {
            volumeChangeTimer -= deltaTime;
        }
        

        for (auto& glow : glowEffects) {
            glow.update(deltaTime);
        }
        

        glowEffects.erase(
            std::remove_if(glowEffects.begin(), glowEffects.end(),
                [](const GlowEffect& g) { return g.timer <= 0; }),
            glowEffects.end()
        );
        

        for (auto& particle : thermometerParticles) {
            particle.update(deltaTime);
        }
        

        thermometerParticles.erase(
            std::remove_if(thermometerParticles.begin(), thermometerParticles.end(),
                [](const ThermometerParticle& p) { return p.isFinished(); }),
            thermometerParticles.end()
        );
        

        for (auto& popup : achievementPopups) {
            popup.update(deltaTime);
        }
        

        achievementPopups.erase(
            std::remove_if(achievementPopups.begin(), achievementPopups.end(),
                [](const AchievementPopup& p) { return p.isFinished(); }),
            achievementPopups.end()
        );
    }
};

#endif
