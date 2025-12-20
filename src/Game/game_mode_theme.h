#ifndef GAME_MODE_THEME_H
#define GAME_MODE_THEME_H

#include <string>
#include <SFML/Graphics.hpp>


enum class GameModeOption;
enum class ClassicDifficulty;
enum class ChallengeMode;
enum class GameThemeChoice;



struct GameModeTheme {
    std::string musicPath;
    sf::Color backgroundColor;
    sf::Color fallingPiecesColor;
    sf::Color frameColor;
    std::string backgroundTexturePath;
    std::string lineClearSoundPath;
    std::string dropSoundPath;
    float musicVolume;
    
    GameModeTheme() 
        : musicPath("Assets/Sound/Music/Tessera_Main.ogg")
        , backgroundColor(sf::Color(15, 15, 25))
        , fallingPiecesColor(sf::Color(255, 255, 255, 40))
        , frameColor(sf::Color(60, 60, 80))
        , backgroundTexturePath("")
        , lineClearSoundPath("")
        , dropSoundPath("")
        , musicVolume(1.0f)
    {}
    

    GameModeTheme(const std::string& music, sf::Color bgColor, sf::Color piecesColor, sf::Color frame, const std::string& bgTexture = "", float volume = 1.0f, const std::string& lineClearSound = "", const std::string& dropSound = "")
        : musicPath(music)
        , backgroundColor(bgColor)
        , fallingPiecesColor(piecesColor)
        , frameColor(frame)
        , backgroundTexturePath(bgTexture)
        , lineClearSoundPath(lineClearSound)
        , dropSoundPath(dropSound)
        , musicVolume(volume)
    {}
    

    GameModeTheme(const std::string& music, sf::Color bgColor, const std::string& bgTexture = "", float volume = 1.0f)
        : musicPath(music)
        , backgroundColor(bgColor)
        , fallingPiecesColor(sf::Color(255, 255, 255, 40))
        , frameColor(sf::Color(60, 60, 80))
        , backgroundTexturePath(bgTexture)
        , lineClearSoundPath("")
        , dropSoundPath("")
        , musicVolume(volume)
    {}
};





namespace GameThemes {


    inline GameModeTheme getDefaultTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(10, 10, 25)
        );
    }


    
    inline GameModeTheme getClassicNormalTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(10, 10, 25)
        );
    }
    
    inline GameModeTheme getClassicHardTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(10, 10, 25)
        );
    }
    

    
    inline GameModeTheme getClassicForestTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/flowerhead - Somewhat Good- Forest - 03 Wood Temple.ogg",
            sf::Color(8, 28, 12),
            sf::Color(100, 180, 100, 50),
            sf::Color(40, 80, 40),
            "",
            1.0f,
            "Assets/Sound/SFX/sweep.ogg",
            "Assets/Sound/SFX/ground_forest.ogg"
        );
    }
    

    
    inline GameModeTheme getClassicRacerTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/flowerhead - Somewhat Good- Karts - 01 KARTS!.ogg",
            sf::Color(35, 0, 0),
            sf::Color(255, 255, 255, 50),
            sf::Color(255, 255, 255),
            "",
            1.0f,
            "",
            ""
        );
    }


    
    inline GameModeTheme getSprintTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(10, 20, 15)
        );
    }
    

    
    inline GameModeTheme getSprintForestTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/flowerhead - Somewhat Good- Forest - 03 Wood Temple.ogg",
            sf::Color(8, 28, 12),
            sf::Color(100, 180, 100, 50),
            sf::Color(40, 80, 40),
            "",
            1.0f,
            "Assets/Sound/SFX/sweep.ogg",
            "Assets/Sound/SFX/ground_forest.ogg"
        );
    }
    

    
    inline GameModeTheme getSprintRacerTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/flowerhead - Somewhat Good- Karts - 01 KARTS!.ogg",
            sf::Color(50, 10, 10),
            sf::Color(255, 240, 150, 50),
            sf::Color(255, 255, 255),
            "",
            1.0f,
            "",
            ""
        );
    }


    
    inline GameModeTheme getPracticeTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(15, 15, 30)
        );
    }



    
    inline GameModeTheme getChallengeTheForestTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/flowerhead - Somewhat Good- Forest - 03 Wood Temple.ogg",
            sf::Color(8, 28, 12),
            sf::Color(100, 180, 100, 50),
            sf::Color(40, 80, 40),
            "",
            1.0f,
            "Assets/Sound/SFX/sweep.ogg",
            "Assets/Sound/SFX/ground_forest.ogg"
        );
    }
    
    inline GameModeTheme getChallengeRandomnessTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(20, 10, 25)
        );
    }
    
    inline GameModeTheme getChallengeNonStraightTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(20, 15, 10)
        );
    }
    
    inline GameModeTheme getChallengeOneRotTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(15, 15, 25)
        );
    }
    
    inline GameModeTheme getChallengeChristopherCurseTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(30, 5, 10)
        );
    }
    
    inline GameModeTheme getChallengeVanishingTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(10, 10, 20)
        );
    }
    
    inline GameModeTheme getChallengeAutoDropTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(25, 20, 5)
        );
    }
    
    inline GameModeTheme getChallengeGravityFlipTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(15, 5, 25)
        );
    }
    
    inline GameModeTheme getChallengePetrifyTheme() {
        return GameModeTheme(
            "Assets/Sound/Music/Tessera_Main.ogg",
            sf::Color(20, 20, 25)
        );
    }




    

    inline GameModeTheme getThemeForChallenge(ChallengeMode mode) {
        switch (mode) {
            case ChallengeMode::TheForest:       return getChallengeTheForestTheme();
            case ChallengeMode::Randomness:      return getChallengeRandomnessTheme();
            case ChallengeMode::NonStraight:     return getChallengeNonStraightTheme();
            case ChallengeMode::OneRot:          return getChallengeOneRotTheme();
            case ChallengeMode::ChristopherCurse: return getChallengeChristopherCurseTheme();
            case ChallengeMode::Vanishing:       return getChallengeVanishingTheme();
            case ChallengeMode::AutoDrop:        return getChallengeAutoDropTheme();
            case ChallengeMode::GravityFlip:     return getChallengeGravityFlipTheme();
            case ChallengeMode::Petrify:         return getChallengePetrifyTheme();
            default:                             return getDefaultTheme();
        }
    }
    

    inline GameModeTheme getThemeForClassic(ClassicDifficulty difficulty, GameThemeChoice themeChoice) {
        if (themeChoice == GameThemeChoice::Forest) {
            return getClassicForestTheme();
        }
        if (themeChoice == GameThemeChoice::Racer) {
            return getClassicRacerTheme();
        }

        switch (difficulty) {
            case ClassicDifficulty::Normal: return getClassicNormalTheme();
            case ClassicDifficulty::Hard:   return getClassicHardTheme();
            default:                        return getDefaultTheme();
        }
    }
    

    inline GameModeTheme getThemeForClassic(ClassicDifficulty difficulty) {
        return getThemeForClassic(difficulty, GameThemeChoice::Classic);
    }
    

    inline GameModeTheme getThemeForSprint(GameThemeChoice themeChoice) {
        if (themeChoice == GameThemeChoice::Forest) {
            return getSprintForestTheme();
        }
        if (themeChoice == GameThemeChoice::Racer) {
            return getSprintRacerTheme();
        }
        return getSprintTheme();
    }
    

    inline GameModeTheme getThemeForGameMode(GameModeOption mode, ClassicDifficulty classicDiff, ChallengeMode challengeMode, GameThemeChoice themeChoice) {
        switch (mode) {
            case GameModeOption::Classic:   return getThemeForClassic(classicDiff, themeChoice);
            case GameModeOption::Sprint:    return getThemeForSprint(themeChoice);
            case GameModeOption::Practice:  return getPracticeTheme();
            case GameModeOption::Challenge: return getThemeForChallenge(challengeMode);
            default:                        return getDefaultTheme();
        }
    }
    

    inline GameModeTheme getThemeForGameMode(GameModeOption mode, ClassicDifficulty classicDiff = ClassicDifficulty::Normal, ChallengeMode challengeMode = ChallengeMode::TheForest) {
        return getThemeForGameMode(mode, classicDiff, challengeMode, GameThemeChoice::Classic);
    }

}

#endif
