#pragma once

#include "types.h"
#include <string>


std::string getSaveFilePath();


void saveGameData(const SaveData& data);


SaveData loadGameData();


void deleteSaveFile();


bool insertNewScore(SaveData& saveData, int score, int lines, int level, ClassicDifficulty difficulty);
