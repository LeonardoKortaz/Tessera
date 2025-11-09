#include "achievements.h"
#include <iostream>


AchievementInfo getAchievementInfo(Achievement ach) {
    switch (ach) {
        case Achievement::Combo10:
            return {"Combo Master", "Chain Reaction", "Reach a combo of 12", false};
        case Achievement::Combo5OneClear:
            return {"Big Clear", "Quintuple Threat", "Clear 5 lines in one move", false};
        case Achievement::Score200kNoBomb:
            return {"Purist", "No Explosives Needed", "Score 200,000 in Classic without using bomb", false};
        case Achievement::Blitz48Under230:
            return {"Speed Demon", "Lightning Fast", "Complete Blitz 48 lines under 2:13.7", false};
        case Achievement::Score400kMedHard:
            return {"High Roller", "Difficulty Champion", "Score 400,000 in Classic Medium or Hard", false};
        case Achievement::HoldBomb:
            return {"Bomb Holder", "Strategic Reserve", "Hold a bomb piece", false};
        case Achievement::Combo6OneClear:
            return {"Is That Even Possible?", "Legendary Move", "Clear 6 lines in one move", false};
        case Achievement::Explosion:
            return {"Explosion", "Just play it, you feel it, you've get it... Explosion, explosion, explosion", "Detonate 3 bombs in a row", false};
        case Achievement::MenuBombClicker:
            return {"Curiosity", "What Does This Do?", "Click a bomb in the menu", true};
        case Achievement::PerfectClear:
            return {"She Hit the Floor", "Apple Bottom Jeans, Boots with the Fur", "Reach the floor with 100 cleared lines", false};
        default:
            return {"Unknown", "", "", false};
    }
}

AchievementInfo getAchievementInfo(Achievement ach, bool unlocked) {
    AchievementInfo info = getAchievementInfo(ach);
    

    if (info.isSecret && !unlocked) {
        info.title = "SECRET";
        info.subtitle = "Hidden Achievement";
        info.description = "???";
    }
    
    return info;
}


std::vector<AchievementInfo> getAllAchievementsInfo() {
    std::vector<AchievementInfo> allAchievements;
    

    for (int i = 0; i < TOTAL_ACHIEVEMENTS; ++i) {
        allAchievements.push_back(getAchievementInfo(static_cast<Achievement>(i)));
    }
    

    for (int i = TOTAL_ACHIEVEMENTS; i < MAX_ACHIEVEMENT_SLOTS; ++i) {
        allAchievements.push_back({"???", "Coming Soon", "This achievement has not been implemented yet", false});
    }
    
    return allAchievements;
}


std::string getAchievementName(Achievement ach) {
    return getAchievementInfo(ach).title;
}


std::string getAchievementDescription(Achievement ach) {
    return getAchievementInfo(ach).description;
}


bool tryUnlockAchievement(SaveData& saveData, Achievement ach) {
    int id = static_cast<int>(ach);
    if (!saveData.achievements[id]) {
        saveData.achievements[id] = true;
        
        std::cout << "đźŹ† ACHIEVEMENT UNLOCKED: " << getAchievementName(ach) 
                  << " - " << getAchievementDescription(ach) << std::endl;
        
        return true;
    }
    return false;
}
