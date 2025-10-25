#pragma once

#include "types.h"
#include <string>


enum class Achievement {
    Combo10 = 0,
    Combo5OneClear = 1,
    Score200kNoBomb = 2,
    Blitz48Under230 = 3,
    Score400kMedHard = 4,
    HoldBomb = 5,
    Combo6OneClear = 6
};


struct AchievementData {
    std::string name;
    std::string description;
    bool unlocked;
};


std::string getAchievementName(Achievement ach);
std::string getAchievementDescription(Achievement ach);


bool tryUnlockAchievement(SaveData& saveData, Achievement ach);
