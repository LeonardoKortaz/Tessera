#pragma once

#include "types.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>


void drawGridBackground(sf::RenderWindow& window);
void drawCell(sf::RenderWindow& window, float x, float y, float cellSize, const sf::Color& color, 
              TextureType texType, const std::map<TextureType, sf::Texture>& textures, 
              bool useTextures, const sf::Transform& transform = sf::Transform::Identity);
void drawGridBorder(sf::RenderWindow& window);
void drawBombAbility(sf::RenderWindow& window, bool isAvailable, int linesSinceLastAbility, 
                    const std::map<TextureType, sf::Texture>& textures, bool useTextures, 
                    const sf::Font& font, bool fontLoaded, bool infiniteBombs = false);
void drawHeldPiece(sf::RenderWindow& window, PieceType heldType, bool hasHeld, 
                  const std::map<TextureType, sf::Texture>& textures, bool useTextures, 
                  const sf::Font& font, bool fontLoaded);
void drawCombo(sf::RenderWindow& window, int currentCombo, int lastMoveScore, 
              const sf::Font& font, bool fontLoaded);
void drawLevelInfo(sf::RenderWindow& window, int totalLinesCleared, int currentLevel, int totalScore, 
                  const std::map<TextureType, sf::Texture>& textures, bool useTextures, 
                  const sf::Font& font, bool fontLoaded, bool sprintMode = false, 
                  float sprintTime = 0.0f, int sprintTarget = 0, float gameTime = 0.0f);
void drawNextPieces(sf::RenderWindow& window, const std::vector<PieceType>& nextQueue, 
                   const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void drawGameModeLabel(sf::RenderWindow& window, const std::string& modeText, 
                      const sf::Font& font, bool fontLoaded);
void drawVolumeIndicator(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, 
                        float volume, bool isMuted);
void drawMuteIcon(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, 
                 const sf::Font& font, bool fontLoaded);
void drawDebugMode(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded);
void drawAchievementPopups(sf::RenderWindow& window, const std::vector<AchievementPopup>& popups, 
                          const sf::Font& font, bool fontLoaded);
void drawCustomCursor(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, 
                     bool useTextures);
