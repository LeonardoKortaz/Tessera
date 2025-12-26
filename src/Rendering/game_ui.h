#pragma once

#include "types.h"
#include "game_mode_theme.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>


void drawGridBackground(sf::RenderWindow& window, const sf::Color& backgroundColor = sf::Color(10, 15, 31));
void drawCell(sf::RenderWindow& window, float x, float y, float cellSize, const sf::Color& color, 
              TextureType texType, const std::map<TextureType, sf::Texture>& textures, 
              bool useTextures, const sf::Transform& transform = sf::Transform::Identity);
void drawGridBorder(sf::RenderWindow& window, const sf::Color& borderColor = sf::Color(100, 150, 255));
void drawBombAbility(sf::RenderWindow& window, bool isAvailable, int linesSinceLastAbility, 
                    const std::map<TextureType, sf::Texture>& textures, bool useTextures, 
                    const sf::Font& font, bool fontLoaded, bool infiniteBombs = false,
                    const sf::Color& frameColor = sf::Color(100, 150, 255),
                    AbilityChoice selectedAbility = AbilityChoice::Bomb,
                    int linesRequired = 10);
void drawHeldPiece(sf::RenderWindow& window, PieceType heldType, bool hasHeld, 
                  const std::map<TextureType, sf::Texture>& textures, bool useTextures, 
                  const sf::Font& font, bool fontLoaded,
                  const sf::Color& frameColor = sf::Color(100, 150, 255));
void drawCombo(sf::RenderWindow& window, float displayCombo, int targetCombo, int lastMoveScore, 
              const sf::Font& font, bool fontLoaded, bool showScore = true, float fadeScale = 1.0f);
void drawLevelInfo(sf::RenderWindow& window, int totalLinesCleared, int currentLevel, int totalScore, 
                  const std::map<TextureType, sf::Texture>& textures, bool useTextures, 
                  const sf::Font& font, bool fontLoaded, bool sprintMode = false, 
                  float sprintTime = 0.0f, int sprintTarget = 0, float gameTime = 0.0f,
                  float displayedProgress = -1.0f,
                  const sf::Color& frameColor = sf::Color(100, 150, 255));
void drawNextPieces(sf::RenderWindow& window, const std::vector<PieceType>& nextQueue, 
                   const std::map<TextureType, sf::Texture>& textures, bool useTextures,
                   const sf::Color& frameColor = sf::Color(100, 150, 255),
                   const sf::Font& font = sf::Font(), bool fontLoaded = false);
void drawGameModeLabel(sf::RenderWindow& window, const std::string& modeText, 
                      const sf::Font& font, bool fontLoaded,
                      bool isRaceMode = false, float timeValue = 0.0f, int scoreValue = 0,
                      float bestTime = 0.0f, int highScore = 0);
void drawVolumeIndicator(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, 
                        float volume, bool isMuted);
void drawMuteIcon(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, 
                 const sf::Font& font, bool fontLoaded);
void drawDebugMode(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded);
void drawAchievementPopups(sf::RenderWindow& window, const std::vector<AchievementPopup>& popups, 
                          const sf::Font& font, bool fontLoaded);
void drawCustomCursor(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, 
                     bool useTextures);
void drawThermometerParticles(sf::RenderWindow& window, const std::vector<ThermometerParticle>& particles);
void drawFallingCells(sf::RenderWindow& window, const std::vector<FallingCell>& cells,
                     const std::map<TextureType, sf::Texture>& textures, bool useTextures);

struct PickerDimensions {
    float pickerWidth = 600.0f;
    float pickerHeight = 70.0f;
    float arrowWidth = 70.0f;
    float boxWidth = 650.0f;
    float boxHeight = 150.0f;
    float labelHeight = 55.0f;
    float labelYOffset = 15.0f;
};


const PickerDimensions& getPickerDimensions();


void drawPickerSelector(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded,
                       const std::string& displayText, const sf::Color& textColor,
                       const sf::Color& highlightColor, float pickerY,
                       bool isLeftArrowHovered, bool isRightArrowHovered, bool isKeyboardSelected = false);


void drawThemeSelector(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, 
                      GameThemeChoice currentTheme, GameThemeChoice hoveredTheme, 
                      bool isLeftArrowHovered, bool isRightArrowHovered, bool isKeyboardSelected = false);
void drawAbilitySelector(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, 
                        AbilityChoice currentAbility, AbilityChoice hoveredAbility, 
                        bool isLeftArrowHovered, bool isRightArrowHovered, bool isKeyboardSelected = false);
void drawDifficultySelector(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded,
                           ClassicDifficulty currentDifficulty, bool isLeftArrowHovered,
                           bool isRightArrowHovered, bool isKeyboardSelected = false);


struct PickerHitboxes {
    float pickerY;
    float leftArrowX1, leftArrowX2;
    float rightArrowX1, rightArrowX2;
    float pickerHeight;
};


PickerHitboxes createPickerHitbox(float pickerY);


float getDifficultyPickerY();
float getAbilityPickerY();
float getThemePickerY();
float getPlayButtonY();


struct BoxPosition {
    float x, y, width, height;
};
BoxPosition getDifficultyBoxPosition();
BoxPosition getAbilityBoxPosition();
BoxPosition getThemeBoxPosition();


inline PickerHitboxes getDifficultyPickerHitboxes() { return createPickerHitbox(getDifficultyPickerY()); }
inline PickerHitboxes getAbilityPickerHitboxes() { return createPickerHitbox(getAbilityPickerY()); }
inline PickerHitboxes getThemePickerHitboxes() { return createPickerHitbox(getThemePickerY()); }
