#pragma once

#include "types.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>


constexpr float SCREEN_WIDTH = 1920.0f;
constexpr float SCREEN_HEIGHT = 1080.0f;

void drawGameOver(sf::RenderWindow& window, int finalScore, int finalLines, int finalLevel, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, const SaveData& saveData, int hardDropScore, int lineScore, int comboScore, ClassicDifficulty difficulty, bool isSprintMode, float sprintTime, int sprintTarget, bool sprintCompleted, bool isChallengeMode, bool isPracticeMode = false, float uiAlpha = 1.0f, sf::Color frameColor = sf::Color::White, float statsRevealTime = 0.0f, bool isNewHighScore = false, int previousHighScore = 0, sf::Color backgroundColor = sf::Color::Black);
void drawTesseraTitle(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded);
void drawPauseMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, PauseOption selectedOption, const sf::Color& frameColor = sf::Color(100, 150, 255), const sf::Color& backgroundColor = sf::Color(10, 15, 31));
void drawConfirmClearScores(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, ConfirmOption selectedOption);
void drawFirstTimeSetup(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ControlScheme selectedScheme, const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void drawWelcomeScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded);
void drawMainMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, MenuOption selectedOption, bool debugMode, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime = 0.0f);
void drawGameModeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, GameModeOption selectedOption, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime, bool debugMode = false);
void drawClassicDifficultyMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ClassicDifficulty selectedOption, const SaveData& saveData, const std::map<TextureType, sf::Texture>& textures, bool useTextures, bool debugMode = false);
void drawSprintLinesMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, SprintLines selectedOption, const SaveData& saveData, bool debugMode, const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void drawChallengeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ChallengeMode selectedOption, bool debugMode, const SaveData& saveData, const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void drawPracticeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, PracticeDifficulty selectedDifficulty, PracticeLineGoal selectedLineGoal, bool infiniteBombs, PracticeStartLevel selectedStartLevel, int selectedOption, const std::map<TextureType, sf::Texture>& textures, bool useTextures, bool debugMode = false);
void drawExtrasMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ExtrasOption selectedOption, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime, bool debugMode = false);
void drawAchievementsScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData, int hoveredAchievement, bool debugMode = false);
void drawStatisticsScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData, bool debugMode = false);
void drawBestScoresScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData, bool debugMode = false);
void drawOptionsMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, bool debugMode, OptionsMenuOption selectedOption, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime);
void drawAudioMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, bool debugMode, AudioOption selectedOption, float mainVolume, float musicVolume, float sfxVolume, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime);
void drawCustomizationMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, GameThemeChoice hoveredTheme, GameThemeChoice selectedTheme, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime);
void drawRebindingScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const KeyBindings& bindings, ControlScheme selectedScheme, ControlScheme hoveredScheme, ControlScheme appliedScheme, int selectedBinding, bool waitingForKey, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime, bool debugMode = false);


float calculateButtonWidth(const sf::Font& font, const std::string& text, float textSize);

void drawExplosionEffects(sf::RenderWindow& window, const std::vector<ExplosionEffect>& explosions);
void drawGlowEffects(sf::RenderWindow& window, const std::vector<GlowEffect>& glowEffects, const std::map<TextureType, sf::Texture>& textures);


void updateBackgroundPieces(std::vector<BackgroundPiece>& pieces, float deltaTime);
void drawBackgroundPieces(sf::RenderWindow& window, const std::vector<BackgroundPiece>& pieces, const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void drawBackgroundPiecesWithExplosions(sf::RenderWindow& window, const std::vector<BackgroundPiece>& pieces, const std::vector<ExplosionEffect>& explosions, const std::map<TextureType, sf::Texture>& textures, bool useTextures);
void spawnBackgroundPiece(std::vector<BackgroundPiece>& pieces);
int checkBombClick(const std::vector<BackgroundPiece>& pieces, float mouseX, float mouseY);


void updateGameBackgroundPieces(std::vector<BackgroundPiece>& pieces, float deltaTime);
void drawGameBackgroundPieces(sf::RenderWindow& window, const std::vector<BackgroundPiece>& pieces, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Color& pieceColor = sf::Color(100, 180, 255, 40));
void spawnGameBackgroundPiece(std::vector<BackgroundPiece>& pieces);


std::string getKeyName(sf::Keyboard::Key key);
