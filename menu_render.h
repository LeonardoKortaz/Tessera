#pragma once

#include "types.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>


void drawGameOver(sf::RenderWindow& window, int finalScore, int finalLines, int finalLevel, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, const SaveData& saveData, int hardDropScore, int lineScore, int comboScore, ClassicDifficulty difficulty, bool isSprintMode, float sprintTime, int sprintTarget, bool sprintCompleted);
void drawJigtrizTitle(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded);
void drawPauseMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, PauseOption selectedOption);
void drawConfirmClearScores(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, ConfirmOption selectedOption);
void drawMainMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, MenuOption selectedOption, bool debugMode);
void drawGameModeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, GameModeOption selectedOption);
void drawClassicDifficultyMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ClassicDifficulty selectedOption, const SaveData& saveData);
void drawSprintLinesMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, SprintLines selectedOption, const SaveData& saveData, bool debugMode);
void drawChallengeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ChallengeMode selectedOption);
void drawJigtrizopediaMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, JigtrizopediaOption selectedOption);
void drawAchievementsScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, int hoveredAchievement);
void drawOptionsMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, bool debugMode, OptionsMenuOption selectedOption);
void drawRebindingScreen(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, const KeyBindings& bindings, int selectedBinding, bool waitingForKey);


void drawExplosionEffects(sf::RenderWindow& window, const std::vector<ExplosionEffect>& explosions);
void drawGlowEffects(sf::RenderWindow& window, const std::vector<GlowEffect>& glowEffects, const std::map<TextureType, sf::Texture>& textures);


std::string getKeyName(sf::Keyboard::Key key);
