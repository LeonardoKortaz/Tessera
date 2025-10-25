#include "achievements.h"
#include <iostream>


std::string getAchievementName(Achievement ach) {
    switch (ach) {
        case Achievement::Combo10: return "Combo Master";
        case Achievement::Combo5OneClear: return "Big Clear";
        case Achievement::Score200kNoBomb: return "Purist";
        case Achievement::Blitz48Under230: return "Speed Demon";
        case Achievement::Score400kMedHard: return "High Roller";
        case Achievement::HoldBomb: return "Bomb Holder";
        case Achievement::Combo6OneClear: return "Is That Even Possible?";
        default: return "Unknown";
    }
}


std::string getAchievementDescription(Achievement ach) {
    switch (ach) {
        case Achievement::Combo10: return "Reach a combo of 10";
        case Achievement::Combo5OneClear: return "Clear 5 lines in one move";
        case Achievement::Score200kNoBomb: return "Score 200,000 in Classic without using bomb";
        case Achievement::Blitz48Under230: return "Complete Blitz 48 lines under 2:00";
        case Achievement::Score400kMedHard: return "Score 400,000 in Classic Medium or Hard";
        case Achievement::HoldBomb: return "Hold a bomb piece";
        case Achievement::Combo6OneClear: return "Clear 6 lines in one move";
        default: return "";
    }
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
