#pragma once

#include "types.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>


constexpr float SCREEN_WIDTH = 1920.0f;
constexpr float SCREEN_HEIGHT = 1080.0f;

void drawGameOver(sf::RenderWindow& window, int finalScore, int finalLines, int finalLevel, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, const SaveData& saveData, int hardDropScore, int lineScore, int comboScore, ClassicDifficulty difficulty, bool isSprintMode, float sprintTime, int sprintTarget, bool sprintCompleted, bool isChallengeMode, bool isPracticeMode = false);
void drawJigzterTitle(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded);
void drawPauseMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, PauseOption selectedOption);
void drawConfirmClearScores(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, ConfirmOption selectedOption);
void drawMainMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, MenuOption selectedOption, bool debugMode, const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void drawGameModeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, GameModeOption selectedOption);
void drawClassicDifficultyMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ClassicDifficulty selectedOption, const SaveData& saveData);
void drawSprintLinesMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, SprintLines selectedOption, const SaveData& saveData, bool debugMode);
void drawChallengeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ChallengeMode selectedOption, bool debugMode, const SaveData& saveData);
void drawPracticeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, PracticeDifficulty selectedDifficulty, PracticeLineGoal selectedLineGoal, bool infiniteBombs, PracticeStartLevel selectedStartLevel, int selectedOption);
void drawExtrasMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ExtrasOption selectedOption);
void drawAchievementsScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData, int hoveredAchievement);
void drawStatisticsScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData);
void drawBestScoresScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData);
void drawOptionsMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, bool debugMode, OptionsMenuOption selectedOption);
void drawRebindingScreen(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, const KeyBindings& bindings, int selectedBinding, bool waitingForKey);


void drawExplosionEffects(sf::RenderWindow& window, const std::vector<ExplosionEffect>& explosions);
void drawGlowEffects(sf::RenderWindow& window, const std::vector<GlowEffect>& glowEffects, const std::map<TextureType, sf::Texture>& textures);


void updateBackgroundPieces(std::vector<BackgroundPiece>& pieces, float deltaTime);
void drawBackgroundPieces(sf::RenderWindow& window, const std::vector<BackgroundPiece>& pieces, const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void drawBackgroundPiecesWithExplosions(sf::RenderWindow& window, const std::vector<BackgroundPiece>& pieces, const std::vector<ExplosionEffect>& explosions, const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void spawnBackgroundPiece(std::vector<BackgroundPiece>& pieces);
int checkBombClick(const std::vector<BackgroundPiece>& pieces, float mouseX, float mouseY);


std::string getKeyName(sf::Keyboard::Key key);
