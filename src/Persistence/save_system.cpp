#include "save_system.h"
#include <filesystem>
#include <fstream>
#include <iostream>

std::string getSaveFilePath() {
    
    std::filesystem::path saveDataPath;
    
#ifdef _WIN32
    
    char* appData = std::getenv("APPDATA");
    if (appData) {
        saveDataPath = std::filesystem::path(appData) / "Tessera";
    } else {
        saveDataPath = std::filesystem::current_path() / "Tessera_Saves";
    }
#else
    
    char* homePath = std::getenv("HOME");
    if (homePath) {
        saveDataPath = std::filesystem::path(homePath) / ".jigz";
    } else {
        saveDataPath = std::filesystem::current_path() / "Tessera_Saves";
    }
#endif
    
    
    std::filesystem::path gameFolder = saveDataPath;
    if (!std::filesystem::exists(gameFolder)) {
        std::filesystem::create_directories(gameFolder);
    }
    
    return (gameFolder / "save_data.txt").string();
}

void saveGameData(const SaveData& data) {
    std::string filePath = getSaveFilePath();
    std::ofstream file(filePath);
    
    if (file.is_open()) {
        file << "HIGH_SCORE=" << data.highScore << std::endl;
        file << "HIGH_SCORE_CLASSIC_NORMAL=" << data.highScoreClassicNormal << std::endl;
        file << "HIGH_SCORE_CLASSIC_HARD=" << data.highScoreClassicHard << std::endl;
        file << "BEST_TIME_SPRINT_1=" << data.bestTimeSprint1 << std::endl;
        file << "BEST_TIME_SPRINT_24=" << data.bestTimeSprint24 << std::endl;
        file << "BEST_TIME_SPRINT_48=" << data.bestTimeSprint48 << std::endl;
        file << "BEST_TIME_SPRINT_96=" << data.bestTimeSprint96 << std::endl;
        file << "BEST_TIME_CHALLENGE_DEBUG=" << data.bestTimeChallengeDebug << std::endl;
        file << "BEST_TIME_CHALLENGE_THE_FOREST=" << data.bestTimeChallengeTheForest << std::endl;
        file << "BEST_TIME_CHALLENGE_RANDOMNESS=" << data.bestTimeChallengeRandomness << std::endl;
        file << "BEST_TIME_CHALLENGE_NON_STRAIGHT=" << data.bestTimeChallengeNonStraight << std::endl;
        file << "BEST_TIME_CHALLENGE_ONE_ROT=" << data.bestTimeChallengeOneRot << std::endl;
        file << "BEST_TIME_CHALLENGE_CHRISTOPHER_CURSE=" << data.bestTimeChallengeChristopherCurse << std::endl;
        file << "BEST_TIME_CHALLENGE_VANISHING=" << data.bestTimeChallengeVanishing << std::endl;
        file << "BEST_TIME_CHALLENGE_AUTO_DROP=" << data.bestTimeChallengeAutoDrop << std::endl;
        file << "BEST_TIME_CHALLENGE_GRAVITY_FLIP=" << data.bestTimeChallengeGravityFlip << std::endl;
        file << "BEST_TIME_CHALLENGE_PETRIFY=" << data.bestTimeChallengePetrify << std::endl;
        file << "BEST_LINES=" << data.bestLines << std::endl;
        file << "BEST_LEVEL=" << data.bestLevel << std::endl;
        

        file << "STAT_TOTAL_LINES=" << data.totalLinesCleared << std::endl;
        file << "STAT_TOTAL_PIECES=" << data.totalPiecesPlaced << std::endl;
        file << "STAT_TOTAL_GAMES=" << data.totalGamesPlayed << std::endl;
        file << "STAT_TOTAL_SCORE=" << data.totalScore << std::endl;
        file << "STAT_MAX_COMBO=" << data.maxComboEver << std::endl;
        file << "STAT_TOTAL_BOMBS=" << data.totalBombsUsed << std::endl;
        file << "STAT_TOTAL_PLAYTIME=" << data.totalPlayTimeSeconds << std::endl;
        file << "STAT_TOTAL_ROTATIONS=" << data.totalRotations << std::endl;
        file << "STAT_TOTAL_HOLDS=" << data.totalHolds << std::endl;
        file << "STAT_TOTAL_PERFECT_CLEARS=" << data.totalPerfectClears << std::endl;
        
        file << "MASTER_VOLUME=" << data.masterVolume << std::endl;
        file << "MUSIC_VOLUME=" << data.musicVolume << std::endl;
        file << "SFX_VOLUME=" << data.sfxVolume << std::endl;
        file << "IS_MUTED=" << (data.isMuted ? 1 : 0) << std::endl;
        file << "SETUP_VERSION=" << data.setupVersion << std::endl;
        file << "SELECTED_THEME=" << data.selectedTheme << std::endl;
        

        for (int i = 0; i < 3; i++) {
            file << "TOP" << (i+1) << "_SCORE=" << data.topScores[i].score << std::endl;
            file << "TOP" << (i+1) << "_LINES=" << data.topScores[i].lines << std::endl;
            file << "TOP" << (i+1) << "_LEVEL=" << data.topScores[i].level << std::endl;
        }
        

        for (int i = 0; i < 3; i++) {
            file << "TOP_NORMAL_" << (i+1) << "_SCORE=" << data.topScoresNormal[i].score << std::endl;
            file << "TOP_NORMAL_" << (i+1) << "_LINES=" << data.topScoresNormal[i].lines << std::endl;
            file << "TOP_NORMAL_" << (i+1) << "_LEVEL=" << data.topScoresNormal[i].level << std::endl;
        }
        

        for (int i = 0; i < 3; i++) {
            file << "TOP_HARD_" << (i+1) << "_SCORE=" << data.topScoresHard[i].score << std::endl;
            file << "TOP_HARD_" << (i+1) << "_LINES=" << data.topScoresHard[i].lines << std::endl;
            file << "TOP_HARD_" << (i+1) << "_LEVEL=" << data.topScoresHard[i].level << std::endl;
        }
        

        for (int i = 0; i < 25; i++) {
            file << "ACHIEVEMENT_" << i << "=" << (data.achievements[i] ? 1 : 0) << std::endl;
        }
        

        file << "KEY_MOVE_LEFT=" << data.moveLeft << std::endl;
        file << "KEY_MOVE_RIGHT=" << data.moveRight << std::endl;
        file << "KEY_ROTATE_LEFT=" << data.rotateLeft << std::endl;
        file << "KEY_ROTATE_RIGHT=" << data.rotateRight << std::endl;
        file << "KEY_QUICK_FALL=" << data.quickFall << std::endl;
        file << "KEY_DROP=" << data.drop << std::endl;
        file << "KEY_HOLD=" << data.hold << std::endl;
        file << "KEY_BOMB=" << data.bomb << std::endl;
        file << "KEY_RESTART=" << data.restart << std::endl;
        file << "KEY_MUTE=" << data.mute << std::endl;
        file << "KEY_VOLUME_DOWN=" << data.volumeDown << std::endl;
        file << "KEY_VOLUME_UP=" << data.volumeUp << std::endl;
        file << "KEY_MENU=" << data.menu << std::endl;
        
        file.close();
        std::cout << "Game data saved to: " << filePath << std::endl;
    } else {
        std::cout << "Failed to save game data to: " << filePath << std::endl;
    }
}

SaveData loadGameData() {
    SaveData data;
    std::string filePath = getSaveFilePath();
    std::ifstream file(filePath);
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                if (key == "HIGH_SCORE") {
                    data.highScore = std::stoi(value);
                } else if (key == "HIGH_SCORE_CLASSIC_NORMAL" || key == "HIGH_SCORE_CLASSIC_EASY" || key == "HIGH_SCORE_CLASSIC_MEDIUM") {

                    int oldScore = std::stoi(value);
                    if (oldScore > data.highScoreClassicNormal) {
                        data.highScoreClassicNormal = oldScore;
                    }
                } else if (key == "HIGH_SCORE_CLASSIC_HARD") {
                    data.highScoreClassicHard = std::stoi(value);
                } else if (key == "BEST_TIME_SPRINT_1") {
                    data.bestTimeSprint1 = std::stof(value);
                } else if (key == "BEST_TIME_SPRINT_24") {
                    data.bestTimeSprint24 = std::stof(value);
                } else if (key == "BEST_TIME_SPRINT_48") {
                    data.bestTimeSprint48 = std::stof(value);
                } else if (key == "BEST_TIME_SPRINT_96") {
                    data.bestTimeSprint96 = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_DEBUG") {
                    data.bestTimeChallengeDebug = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_THE_FOREST") {
                    data.bestTimeChallengeTheForest = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_RANDOMNESS") {
                    data.bestTimeChallengeRandomness = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_NON_STRAIGHT") {
                    data.bestTimeChallengeNonStraight = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_ONE_ROT") {
                    data.bestTimeChallengeOneRot = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_CHRISTOPHER_CURSE") {
                    data.bestTimeChallengeChristopherCurse = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_VANISHING") {
                    data.bestTimeChallengeVanishing = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_AUTO_DROP") {
                    data.bestTimeChallengeAutoDrop = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_GRAVITY_FLIP") {
                    data.bestTimeChallengeGravityFlip = std::stof(value);
                } else if (key == "BEST_TIME_CHALLENGE_PETRIFY") {
                    data.bestTimeChallengePetrify = std::stof(value);
                } else if (key == "BEST_LINES") {
                    data.bestLines = std::stoi(value);
                } else if (key == "BEST_LEVEL") {
                    data.bestLevel = std::stoi(value);
                } else if (key == "STAT_TOTAL_LINES") {
                    data.totalLinesCleared = std::stoi(value);
                } else if (key == "STAT_TOTAL_PIECES") {
                    data.totalPiecesPlaced = std::stoi(value);
                } else if (key == "STAT_TOTAL_GAMES") {
                    data.totalGamesPlayed = std::stoi(value);
                } else if (key == "STAT_TOTAL_SCORE") {
                    data.totalScore = std::stoi(value);
                } else if (key == "STAT_MAX_COMBO") {
                    data.maxComboEver = std::stoi(value);
                } else if (key == "STAT_TOTAL_BOMBS") {
                    data.totalBombsUsed = std::stoi(value);
                } else if (key == "STAT_TOTAL_PLAYTIME") {
                    data.totalPlayTimeSeconds = std::stof(value);
                } else if (key == "STAT_TOTAL_ROTATIONS") {
                    data.totalRotations = std::stoi(value);
                } else if (key == "STAT_TOTAL_HOLDS") {
                    data.totalHolds = std::stoi(value);
                } else if (key == "STAT_TOTAL_PERFECT_CLEARS") {
                    data.totalPerfectClears = std::stoi(value);
                } else if (key == "MASTER_VOLUME") {
                    data.masterVolume = std::stof(value);
                } else if (key == "MUSIC_VOLUME") {
                    data.musicVolume = std::stof(value);
                } else if (key == "SFX_VOLUME") {
                    data.sfxVolume = std::stof(value);
                } else if (key == "IS_MUTED") {
                    data.isMuted = (std::stoi(value) == 1);
                } else if (key == "SETUP_VERSION") {
                    data.setupVersion = std::stoi(value);
                } else if (key == "SELECTED_THEME") {
                    data.selectedTheme = std::stoi(value);
                } else if (key.rfind("ACHIEVEMENT_", 0) == 0) {

                    int achievementId = std::stoi(key.substr(12));
                    if (achievementId >= 0 && achievementId < 25) {
                        data.achievements[achievementId] = (std::stoi(value) == 1);
                    }
                } else if (key == "KEY_MOVE_LEFT") {
                    data.moveLeft = std::stoi(value);
                } else if (key == "KEY_MOVE_RIGHT") {
                    data.moveRight = std::stoi(value);
                } else if (key == "KEY_ROTATE_LEFT") {
                    data.rotateLeft = std::stoi(value);
                } else if (key == "KEY_ROTATE_RIGHT") {
                    data.rotateRight = std::stoi(value);
                } else if (key == "KEY_QUICK_FALL") {
                    data.quickFall = std::stoi(value);
                } else if (key == "KEY_DROP") {
                    data.drop = std::stoi(value);
                } else if (key == "KEY_HOLD") {
                    data.hold = std::stoi(value);
                } else if (key == "KEY_BOMB") {
                    data.bomb = std::stoi(value);
                } else if (key == "KEY_RESTART") {
                    data.restart = std::stoi(value);
                } else if (key == "KEY_MUTE") {
                    data.mute = std::stoi(value);
                } else if (key == "KEY_VOLUME_DOWN") {
                    data.volumeDown = std::stoi(value);
                } else if (key == "KEY_VOLUME_UP") {
                    data.volumeUp = std::stoi(value);
                } else if (key == "KEY_MENU") {
                    data.menu = std::stoi(value);
                } else if (key.find("TOP") == 0) {

                    if (key.find("TOP_NORMAL_") == 0 || key.find("TOP_EASY_") == 0 || key.find("TOP_MEDIUM_") == 0) {

                        int prefixLen = (key.find("TOP_NORMAL_") == 0) ? 11 : ((key.find("TOP_EASY_") == 0) ? 9 : 11);
                        char topNum = key[prefixLen];
                        int index = topNum - '1';
                        if (index >= 0 && index < 3) {
                            if (key.find("_SCORE") != std::string::npos) {
                                int score = std::stoi(value);
                                if (score > data.topScoresNormal[index].score) {
                                    data.topScoresNormal[index].score = score;
                                }
                            } else if (key.find("_LINES") != std::string::npos) {
                                int lines = std::stoi(value);
                                if (lines > data.topScoresNormal[index].lines) {
                                    data.topScoresNormal[index].lines = lines;
                                }
                            } else if (key.find("_LEVEL") != std::string::npos) {
                                int level = std::stoi(value);
                                if (level > data.topScoresNormal[index].level) {
                                    data.topScoresNormal[index].level = level;
                                }
                            }
                        }
                    } else if (key.find("TOP_HARD_") == 0) {
                        char topNum = key[9];
                        int index = topNum - '1';
                        if (index >= 0 && index < 3) {
                            if (key.find("_SCORE") != std::string::npos) {
                                data.topScoresHard[index].score = std::stoi(value);
                            } else if (key.find("_LINES") != std::string::npos) {
                                data.topScoresHard[index].lines = std::stoi(value);
                            } else if (key.find("_LEVEL") != std::string::npos) {
                                data.topScoresHard[index].level = std::stoi(value);
                            }
                        }
                    } else {

                        char topNum = key[3];
                        int index = topNum - '1';
                        if (index >= 0 && index < 3) {
                            if (key.find("_SCORE") != std::string::npos) {
                                data.topScores[index].score = std::stoi(value);
                            } else if (key.find("_LINES") != std::string::npos) {
                                data.topScores[index].lines = std::stoi(value);
                            } else if (key.find("_LEVEL") != std::string::npos) {
                                data.topScores[index].level = std::stoi(value);
                            }
                        }
                    }
                }
            }
        }
        file.close();
        std::cout << "Game data loaded from: " << filePath << std::endl;
        std::cout << "High Score: " << data.highScore << " | Best Lines: " << data.bestLines << " | Best Level: " << data.bestLevel << std::endl;
    } else {
        std::cout << "No save file found, using default values" << std::endl;
    }
    
    return data;
}

bool insertNewScore(SaveData& saveData, int score, int lines, int level, ClassicDifficulty difficulty) {
    SaveData::ScoreEntry newEntry = {score, lines, level};
    

    SaveData::ScoreEntry* topScoresArray = nullptr;
    switch (difficulty) {
        case ClassicDifficulty::Normal:
            topScoresArray = saveData.topScoresNormal;
            break;
        case ClassicDifficulty::Hard:
            topScoresArray = saveData.topScoresHard;
            break;
    }
    

    int insertPos = -1;
    for (int i = 0; i < 3; i++) {
        if (score > topScoresArray[i].score) {
            insertPos = i;
            break;
        }
    }
    
    bool madeTopThree = (insertPos >= 0);
    
    if (insertPos >= 0) {

        for (int i = 2; i > insertPos; i--) {
            topScoresArray[i] = topScoresArray[i-1];
        }
        topScoresArray[insertPos] = newEntry;
        
        std::cout << "NEW TOP SCORE #" << (insertPos + 1) << " for difficulty " << static_cast<int>(difficulty) << ": " << score << " points!" << std::endl;
    }
    

    int legacyInsertPos = -1;
    for (int i = 0; i < 3; i++) {
        if (score > saveData.topScores[i].score) {
            legacyInsertPos = i;
            break;
        }
    }
    
    if (legacyInsertPos >= 0) {
        for (int i = 2; i > legacyInsertPos; i--) {
            saveData.topScores[i] = saveData.topScores[i-1];
        }
        saveData.topScores[legacyInsertPos] = newEntry;
        
        if (legacyInsertPos == 0) {
            saveData.highScore = score;
        }
    }
    

    switch (difficulty) {
        case ClassicDifficulty::Normal:
            if (score > saveData.highScoreClassicNormal) {
                saveData.highScoreClassicNormal = score;
                std::cout << "New Classic Normal high score: " << score << std::endl;
            }
            break;
        case ClassicDifficulty::Hard:
            if (score > saveData.highScoreClassicHard) {
                saveData.highScoreClassicHard = score;
                std::cout << "New Classic Hard high score: " << score << std::endl;
            }
            break;
    }
    
    if (score > saveData.highScore) {
        saveData.highScore = score;
    }
    
    return madeTopThree;
}
