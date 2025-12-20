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
                    const sf::Color& frameColor = sf::Color(100, 150, 255));
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
