#pragma once

#include "types.h"
#include <string>
#include <vector>


enum class Achievement {

    Combo10 = 0,
    Combo5OneClear = 1,
    Score200kNoBomb = 2,
    Blitz48Under230 = 3,
    Score400kMedHard = 4,
    HoldBomb = 5,
    Combo6OneClear = 6,
    Explosion = 7,
    MenuBombClicker = 8,
    PerfectClear = 9,
    Reserved10 = 10,
    Reserved11 = 11,
    Reserved12 = 12,
    Reserved13 = 13,
    Reserved14 = 14,
    

    ChallengeTheForest = 15,
    ChallengeRandomness = 16,
    ChallengeNonStraight = 17,
    ChallengeOneRot = 18,
    ChallengeChristopherCurse = 19,
    ChallengeVanishing = 20,
    ChallengeAutoDrop = 21,
    ChallengeFuture1 = 22,
    ChallengeFuture2 = 23,
    ChallengeFuture3 = 24
};

constexpr int TOTAL_ACHIEVEMENTS = 25;
constexpr int MAX_ACHIEVEMENT_SLOTS = 25;


struct AchievementInfo {
    std::string title;
    std::string subtitle;
    std::string description;
    bool isSecret;
};


std::string getAchievementName(Achievement ach);
std::string getAchievementDescription(Achievement ach);
AchievementInfo getAchievementInfo(Achievement ach);
AchievementInfo getAchievementInfo(Achievement ach, bool unlocked);
std::vector<AchievementInfo> getAllAchievementsInfo();


bool tryUnlockAchievement(SaveData& saveData, Achievement ach);
