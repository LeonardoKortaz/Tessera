#pragma once

#include "types.h"
#include <string>
#include <vector>
#include <utility>








namespace MenuConfig {




namespace GameModeMenu {
    constexpr const char* CLASSIC = "CLASSIC";
    constexpr const char* BLITZ = "BLITZ";
    constexpr const char* CHALLENGE = "CHALLENGE";
    constexpr const char* PRACTICE = "PRACTICE";
    

    constexpr float START_Y_OFFSET = -80.0f;
    constexpr float SPACING = 90.0f;
}




namespace ChallengeMenu {
    constexpr const char* TITLE = "CHALLENGE MODE";
    constexpr const char* DEBUG = "DEBUG (1 LINE)";
    constexpr const char* THE_FOREST = "THE FOREST";
    constexpr const char* RANDOMNESS = "RANDOMNESS";
    constexpr const char* NON_STRAIGHT = "NON STRAIGHT";
    constexpr const char* ONE_ROTATION = "ONE ROTATION";
    constexpr const char* THE_CURSE = "THE CURSE";
    constexpr const char* VANISHING = "VANISHING";
    constexpr const char* AUTO_DROP = "AUTO DROP";
    

    constexpr float BUTTON_WIDTH = 400.0f;
    constexpr float COLUMN_SPACING = 450.0f;
    constexpr float START_Y_OFFSET = -150.0f;
    constexpr float SPACING = 100.0f;
}




namespace PracticeMenu {
    constexpr const char* TITLE = "PRACTICE MODE";
    constexpr const char* LABEL_DIFFICULTY = "DIFFICULTY:";
    constexpr const char* LABEL_LINE_GOAL = "LINE GOAL:";
    constexpr const char* LABEL_INFINITE_BOMBS = "INFINITE BOMBS:";
    constexpr const char* LABEL_START_LEVEL = "START LEVEL:";
    constexpr const char* START_BUTTON = "START PRACTICE";
    

    constexpr const char* DIFF_VERY_EASY = "VERY EASY";
    constexpr const char* DIFF_EASY = "EASY";
    constexpr const char* DIFF_MEDIUM = "MEDIUM";
    constexpr const char* DIFF_HARD = "HARD";
    

    constexpr const char* GOAL_INFINITE = "INFINITE";
    constexpr const char* GOAL_24_LINES = "24 LINES";
    constexpr const char* GOAL_48_LINES = "48 LINES";
    constexpr const char* GOAL_96_LINES = "96 LINES";
    

    constexpr float BUTTON_WIDTH = 700.0f;
    constexpr float START_Y_OFFSET = -220.0f;
    constexpr float SPACING = 85.0f;
}




namespace ExtrasMenu {
    constexpr const char* ACHIEVEMENTS = "ACHIEVEMENTS";
    constexpr const char* STATISTICS = "STATISTICS";
    constexpr const char* BEST_SCORES = "BEST SCORES";
    

    constexpr float START_Y_OFFSET = -80.0f;
    constexpr float SPACING = 90.0f;
    

    constexpr int NUM_OPTIONS = 3;
}




namespace OptionsMenu {
    constexpr const char* AUDIO = "AUDIO";
    constexpr const char* REBIND_KEYS = "REBIND KEYS";
    constexpr const char* CLEAR_ALL_DATA = "CLEAR ALL DATA";
    

    constexpr float OPTION1_Y_OFFSET = -130.0f;
    constexpr float OPTION2_Y_OFFSET = -30.0f;
    constexpr float OPTION3_Y_OFFSET = 70.0f;
}

namespace CustomizationMenu {
    constexpr const char* TITLE = "THEME";
    constexpr const char* CLASSIC = "CLASSIC";
    constexpr const char* FOREST = "FOREST";
    constexpr const char* RACER = "RACER";
    
    constexpr float TITLE_Y_OFFSET = -150.0f;
    constexpr float OPTION1_Y_OFFSET = -50.0f;
    constexpr float OPTION2_Y_OFFSET = 50.0f;
    constexpr float OPTION3_Y_OFFSET = 150.0f;
}

namespace AudioMenu {
    constexpr const char* MAIN_VOLUME = "Main Volume";
    constexpr const char* MUSIC_VOLUME = "Music Volume";
    constexpr const char* SFX_VOLUME = "SFX Volume";
    
    constexpr float OPTION1_Y_OFFSET = 0.0f;
    constexpr float OPTION2_Y_OFFSET = 100.0f;
    constexpr float OPTION3_Y_OFFSET = 200.0f;
    
    constexpr float SLIDER_WIDTH = 400.0f;
    constexpr float SLIDER_HEIGHT = 20.0f;
    constexpr float SLIDER_HANDLE_WIDTH = 16.0f;
    constexpr float SLIDER_HANDLE_HEIGHT = 30.0f;
}


namespace RebindScreen {
    constexpr const char* SCHEME_CLASSIC = "CLASSIC";
    constexpr const char* SCHEME_ALTERNATIVE = "ALTERNATIVE";
    constexpr const char* SCHEME_CUSTOM = "CUSTOM";
    

    constexpr const char* MOVE_LEFT = "Move Left";
    constexpr const char* MOVE_RIGHT = "Move Right";
    constexpr const char* ROTATE_LEFT = "Rotate Left";
    constexpr const char* ROTATE_RIGHT = "Rotate Right";
    constexpr const char* FAST_FALL = "Fast Fall";
    constexpr const char* DROP = "Drop";
    constexpr const char* HOLD = "Hold";
    constexpr const char* BOMB = "Bomb";
    
    constexpr const char* PRESS_KEY = "Press key...";
}




namespace ScreenTitles {
    constexpr const char* ACHIEVEMENTS = "ACHIEVEMENTS";
    constexpr const char* STATISTICS = "STATISTICS";
    constexpr const char* BEST_SCORES = "BEST SCORES";
}






inline const char* getExtrasOptionText(int index) {
    switch (index) {
        case 0: return ExtrasMenu::ACHIEVEMENTS;
        case 1: return ExtrasMenu::STATISTICS;
        case 2: return ExtrasMenu::BEST_SCORES;
        default: return "";
    }
}


inline const char* getGameModeText(GameModeOption mode) {
    switch (mode) {
        case GameModeOption::Classic: return GameModeMenu::CLASSIC;
        case GameModeOption::Sprint: return GameModeMenu::BLITZ;
        case GameModeOption::Challenge: return GameModeMenu::CHALLENGE;
        case GameModeOption::Practice: return GameModeMenu::PRACTICE;
        default: return "";
    }
}


inline const char* getExtrasOptionText(ExtrasOption option) {
    switch (option) {
        case ExtrasOption::Achievements: return ExtrasMenu::ACHIEVEMENTS;
        case ExtrasOption::Statistics: return ExtrasMenu::STATISTICS;
        case ExtrasOption::BestScores: return ExtrasMenu::BEST_SCORES;
        default: return "";
    }
}

}
