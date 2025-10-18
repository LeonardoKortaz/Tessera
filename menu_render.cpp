#include "menu_render.h"
#include <algorithm>
#include <iostream>

void drawGameOver(sf::RenderWindow& window, int finalScore, int finalLines, int finalLevel, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, const SaveData& saveData, int dropScore, int lineScore, int comboScore, ClassicDifficulty difficulty, bool isSprintMode, float sprintTime, int sprintTarget, bool sprintCompleted) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (isSprintMode) {

        sf::RectangleShape gameOverBg;
        gameOverBg.setFillColor(sf::Color(40, 40, 50, 220));
        
        if (sprintCompleted) {

            gameOverBg.setOutlineColor(sf::Color(100, 255, 100, 255));
        } else {

            gameOverBg.setOutlineColor(sf::Color(255, 100, 100, 255));
        }
        
        gameOverBg.setOutlineThickness(4);
        gameOverBg.setSize(sf::Vector2f(700, 450));
        gameOverBg.setPosition(sf::Vector2f(centerX - 350, centerY - 225));
        window.draw(gameOverBg);
        
        if (fontLoaded) {
            sf::Text gameOverText(font, sprintCompleted ? "BLITZ COMPLETE!" : "GAME OVER");
            gameOverText.setCharacterSize(40);
            gameOverText.setFillColor(sprintCompleted ? sf::Color(100, 255, 100) : sf::Color(255, 100, 100));
            gameOverText.setStyle(sf::Text::Bold);
            sf::FloatRect textBounds = gameOverText.getLocalBounds();
            gameOverText.setPosition(sf::Vector2f(centerX - textBounds.size.x/2, centerY - 180));
            window.draw(gameOverText);
            
            sf::Text targetText(font, std::to_string(sprintTarget) + " Lines");
            targetText.setCharacterSize(24);
            targetText.setFillColor(sf::Color(150, 200, 255));
            sf::FloatRect targetBounds = targetText.getLocalBounds();
            targetText.setPosition(sf::Vector2f(centerX - targetBounds.size.x/2, centerY - 135));
            window.draw(targetText);
            

            int minutes = static_cast<int>(sprintTime) / 60;
            int seconds = static_cast<int>(sprintTime) % 60;
            int milliseconds = static_cast<int>((sprintTime - static_cast<int>(sprintTime)) * 100);
            std::string timeStr = std::to_string(minutes) + ":" + 
                                 (seconds < 10 ? "0" : "") + std::to_string(seconds) + "." +
                                 (milliseconds < 10 ? "0" : "") + std::to_string(milliseconds);
            

            float bestTime = 0.0f;
            if (sprintTarget == 1) bestTime = saveData.bestTimeSprint1;
            else if (sprintTarget == 24) bestTime = saveData.bestTimeSprint24;
            else if (sprintTarget == 48) bestTime = saveData.bestTimeSprint48;
            else if (sprintTarget == 96) bestTime = saveData.bestTimeSprint96;
            
            bool newBestTime = sprintCompleted && (bestTime == 0.0f || sprintTime < bestTime);
            

            if (sprintCompleted) {
                if (newBestTime) {
                    sf::Text newRecordText(font, "NEW BEST TIME!");
                    newRecordText.setCharacterSize(18);
                    newRecordText.setFillColor(sf::Color(255, 215, 0));
                    newRecordText.setStyle(sf::Text::Bold);
                    sf::FloatRect recordBounds = newRecordText.getLocalBounds();
                    newRecordText.setPosition(sf::Vector2f(centerX - recordBounds.size.x/2, centerY - 105));
                    window.draw(newRecordText);
                } else if (bestTime > 0.0f) {
                    int bMin = static_cast<int>(bestTime) / 60;
                    int bSec = static_cast<int>(bestTime) % 60;
                    int bMs = static_cast<int>((bestTime - static_cast<int>(bestTime)) * 100);
                    std::string bestTimeStr = std::to_string(bMin) + ":" + 
                                             (bSec < 10 ? "0" : "") + std::to_string(bSec) + "." +
                                             (bMs < 10 ? "0" : "") + std::to_string(bMs);
                    
                    sf::Text prevBestText(font, "Best: " + bestTimeStr);
                    prevBestText.setCharacterSize(18);
                    prevBestText.setFillColor(sf::Color(180, 180, 180));
                    sf::FloatRect prevBounds = prevBestText.getLocalBounds();
                    prevBestText.setPosition(sf::Vector2f(centerX - prevBounds.size.x/2, centerY - 105));
                    window.draw(prevBestText);
                }
            }
            

            sf::Text timeLabel(font, "TIME");
            timeLabel.setCharacterSize(32);
            timeLabel.setFillColor(newBestTime ? sf::Color(255, 215, 0) : sf::Color::Yellow);
            timeLabel.setStyle(sf::Text::Bold);
            sf::FloatRect timeLabelBounds = timeLabel.getLocalBounds();
            timeLabel.setPosition(sf::Vector2f(centerX - timeLabelBounds.size.x/2, centerY - 65));
            window.draw(timeLabel);
            
            sf::Text timeValue(font, timeStr);
            timeValue.setCharacterSize(36);
            timeValue.setFillColor(sf::Color::White);
            sf::FloatRect timeValueBounds = timeValue.getLocalBounds();
            timeValue.setPosition(sf::Vector2f(centerX - timeValueBounds.size.x/2, centerY - 25));
            window.draw(timeValue);
            

            sf::Text linesLabel(font, "LINES");
            linesLabel.setCharacterSize(28);
            linesLabel.setFillColor(sf::Color::Green);
            linesLabel.setStyle(sf::Text::Bold);
            sf::FloatRect linesLabelBounds = linesLabel.getLocalBounds();
            linesLabel.setPosition(sf::Vector2f(centerX - linesLabelBounds.size.x/2, centerY + 40));
            window.draw(linesLabel);
            
            sf::Text linesValue(font, std::to_string(finalLines) + " / " + std::to_string(sprintTarget));
            linesValue.setCharacterSize(28);
            linesValue.setFillColor(sf::Color::White);
            sf::FloatRect linesValueBounds = linesValue.getLocalBounds();
            linesValue.setPosition(sf::Vector2f(centerX - linesValueBounds.size.x/2, centerY + 70));
            window.draw(linesValue);
            
            sf::Text restartText(font, "Press R to restart | ESC to return to menu");
            restartText.setCharacterSize(14);
            restartText.setFillColor(sf::Color(150, 150, 255));
            sf::FloatRect restartBounds = restartText.getLocalBounds();
            restartText.setPosition(sf::Vector2f(centerX - restartBounds.size.x/2, centerY + 140));
            window.draw(restartText);
        }
    } else {

        sf::RectangleShape gameOverBg;
    gameOverBg.setFillColor(sf::Color(40, 40, 50, 220));
    gameOverBg.setOutlineColor(sf::Color(255, 100, 100, 255));
    gameOverBg.setOutlineThickness(4);
    gameOverBg.setSize(sf::Vector2f(700, 500));
    gameOverBg.setPosition(sf::Vector2f(centerX - 350, centerY - 250));
    window.draw(gameOverBg);
    

    int currentHighScore = 0;
    std::string difficultyName = "";
    switch (difficulty) {
        case ClassicDifficulty::Easy:
            currentHighScore = saveData.highScoreClassicEasy;
            difficultyName = "Easy";
            break;
        case ClassicDifficulty::Medium:
            currentHighScore = saveData.highScoreClassicMedium;
            difficultyName = "Medium";
            break;
        case ClassicDifficulty::Hard:
            currentHighScore = saveData.highScoreClassicHard;
            difficultyName = "Hard";
            break;
    }
    
    bool newHighScore = finalScore > currentHighScore;
    bool newBestLines = finalLines > saveData.bestLines;
    bool newBestLevel = finalLevel > saveData.bestLevel;
    
    if (fontLoaded) {
        sf::Text gameOverText(font, "GAME OVER");
        gameOverText.setCharacterSize(40);
        gameOverText.setFillColor(sf::Color(255, 100, 100));
        gameOverText.setStyle(sf::Text::Bold);
        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setPosition(sf::Vector2f(centerX - textBounds.size.x/2, centerY - 200));
        window.draw(gameOverText);
        

        sf::Text difficultyText(font, "Classic " + difficultyName);
        difficultyText.setCharacterSize(24);
        difficultyText.setFillColor(sf::Color(150, 150, 255));
        sf::FloatRect difficultyBounds = difficultyText.getLocalBounds();
        difficultyText.setPosition(sf::Vector2f(centerX - difficultyBounds.size.x/2, centerY - 160));
        window.draw(difficultyText);
        
        if (newHighScore) {
            sf::Text newRecordText(font, "NEW HIGH SCORE!");
            newRecordText.setCharacterSize(18);
            newRecordText.setFillColor(sf::Color(255, 215, 0));
            newRecordText.setStyle(sf::Text::Bold);
            sf::FloatRect recordBounds = newRecordText.getLocalBounds();
            newRecordText.setPosition(sf::Vector2f(centerX - recordBounds.size.x/2, centerY - 130));
            window.draw(newRecordText);
        } else if (currentHighScore > 0) {

            sf::Text prevHighScoreText(font, "Best: " + std::to_string(currentHighScore));
            prevHighScoreText.setCharacterSize(18);
            prevHighScoreText.setFillColor(sf::Color(180, 180, 180));
            sf::FloatRect prevBounds = prevHighScoreText.getLocalBounds();
            prevHighScoreText.setPosition(sf::Vector2f(centerX - prevBounds.size.x/2, centerY - 130));
            window.draw(prevHighScoreText);
        }
        

        sf::Text scoreLabel(font, "SCORE");
        scoreLabel.setCharacterSize(28);
        scoreLabel.setFillColor(newHighScore ? sf::Color(255, 215, 0) : sf::Color::Yellow);
        scoreLabel.setStyle(sf::Text::Bold);
        sf::FloatRect scoreLabelBounds = scoreLabel.getLocalBounds();
        scoreLabel.setPosition(sf::Vector2f(centerX - scoreLabelBounds.size.x/2, centerY - 90));
        window.draw(scoreLabel);
        
        sf::Text scoreValue(font, std::to_string(finalScore));
        scoreValue.setCharacterSize(28);
        scoreValue.setFillColor(sf::Color::White);
        sf::FloatRect scoreValueBounds = scoreValue.getLocalBounds();
        scoreValue.setPosition(sf::Vector2f(centerX - scoreValueBounds.size.x/2, centerY - 60));
        window.draw(scoreValue);
        
        sf::Text linesLabel(font, "LINES");
        linesLabel.setCharacterSize(28);
        linesLabel.setFillColor(newBestLines ? sf::Color(255, 215, 0) : sf::Color::Green);
        linesLabel.setStyle(sf::Text::Bold);
        sf::FloatRect linesLabelBounds = linesLabel.getLocalBounds();
        linesLabel.setPosition(sf::Vector2f(centerX - linesLabelBounds.size.x/2, centerY + 10));
        window.draw(linesLabel);
        
        sf::Text linesValue(font, std::to_string(finalLines));
        linesValue.setCharacterSize(28);
        linesValue.setFillColor(sf::Color::White);
        sf::FloatRect linesValueBounds = linesValue.getLocalBounds();
        linesValue.setPosition(sf::Vector2f(centerX - linesValueBounds.size.x/2, centerY + 40));
        window.draw(linesValue);
        
        sf::Text levelLabel(font, "LEVEL");
        levelLabel.setCharacterSize(28);
        levelLabel.setFillColor(newBestLevel ? sf::Color(255, 215, 0) : sf::Color::Cyan);
        levelLabel.setStyle(sf::Text::Bold);
        sf::FloatRect levelLabelBounds = levelLabel.getLocalBounds();
        levelLabel.setPosition(sf::Vector2f(centerX - levelLabelBounds.size.x/2, centerY + 90));
        window.draw(levelLabel);
        
        sf::Text levelValue(font, std::to_string(finalLevel));
        levelValue.setCharacterSize(28);
        levelValue.setFillColor(sf::Color::White);
        sf::FloatRect levelValueBounds = levelValue.getLocalBounds();
        levelValue.setPosition(sf::Vector2f(centerX - levelValueBounds.size.x/2, centerY + 120));
        window.draw(levelValue);
        
        sf::RectangleShape statsBg;
        statsBg.setFillColor(sf::Color(30, 30, 40, 180));
        statsBg.setOutlineColor(sf::Color(100, 150, 255, 150));
        statsBg.setOutlineThickness(2);
        statsBg.setSize(sf::Vector2f(225, 200));
        statsBg.setPosition(sf::Vector2f(centerX - 320, centerY - 70));
        window.draw(statsBg);
        
        sf::Text statsTitle(font, "SCORE BREAKDOWN");
        statsTitle.setCharacterSize(24);
        statsTitle.setFillColor(sf::Color(150, 200, 255));
        statsTitle.setStyle(sf::Text::Bold);
        statsTitle.setPosition(sf::Vector2f(centerX - 310, centerY - 50));
        window.draw(statsTitle);
        
        sf::Text lineText(font, "Lines: " + std::to_string(lineScore));
        lineText.setCharacterSize(24);
        lineText.setFillColor(sf::Color(100, 255, 150));
        lineText.setPosition(sf::Vector2f(centerX - 310, centerY - 10));
        window.draw(lineText);
        
        sf::Text comboText(font, "Combo Bonus: " + std::to_string(comboScore));
        comboText.setCharacterSize(24);
        comboText.setFillColor(sf::Color(255, 100, 255));
        comboText.setPosition(sf::Vector2f(centerX - 310, centerY + 30));
        window.draw(comboText);

        sf::Text dropText(font, "Hard Drops: " + std::to_string(dropScore));
        dropText.setCharacterSize(24);
        dropText.setFillColor(sf::Color(255, 200, 100));
        dropText.setPosition(sf::Vector2f(centerX - 310, centerY + 70));
        window.draw(dropText);
        
        sf::Text restartText(font, "Press R to restart | ESC to return to menu");
        restartText.setCharacterSize(14);
        restartText.setFillColor(sf::Color(150, 150, 255));
        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setPosition(sf::Vector2f(centerX - restartBounds.size.x/2, centerY + 165));
        window.draw(restartText);
        

        const SaveData::ScoreEntry* topScoresArray = nullptr;
        switch (difficulty) {
            case ClassicDifficulty::Easy:
                topScoresArray = saveData.topScoresEasy;
                break;
            case ClassicDifficulty::Medium:
                topScoresArray = saveData.topScoresMedium;
                break;
            case ClassicDifficulty::Hard:
                topScoresArray = saveData.topScoresHard;
                break;
        }
        
        sf::Text topScoresTitle(font, "TOP 3 SCORES");
        topScoresTitle.setCharacterSize(36);
        topScoresTitle.setFillColor(sf::Color(100, 255, 150));
        topScoresTitle.setStyle(sf::Text::Bold);
        topScoresTitle.setPosition(sf::Vector2f(centerX + 120, centerY - 120));
        window.draw(topScoresTitle);
        
        for (int i = 0; i < 3; i++) {
            if (topScoresArray[i].score > 0) {
                sf::Color scoreColor = sf::Color::White;
                if (i == 0) scoreColor = sf::Color(255, 215, 0);
                else if (i == 1) scoreColor = sf::Color(192, 192, 192);
                else if (i == 2) scoreColor = sf::Color(205, 127, 50);
                
                sf::Text rankText(font, "#" + std::to_string(i + 1) + ": " + std::to_string(topScoresArray[i].score));
                rankText.setCharacterSize(24);
                rankText.setFillColor(scoreColor);
                rankText.setPosition(sf::Vector2f(centerX + 120, centerY - 70 + i * 60));
                window.draw(rankText);
                
                sf::Text detailText(font, "L" + std::to_string(topScoresArray[i].lines) + " Lv" + std::to_string(topScoresArray[i].level));
                detailText.setCharacterSize(18);
                detailText.setFillColor(sf::Color(180, 180, 180));
                detailText.setPosition(sf::Vector2f(centerX + 130, centerY - 50 + i * 60));
                window.draw(detailText);
            } else {
                sf::Text emptyText(font, "#" + std::to_string(i + 1) + ": ---");
                emptyText.setCharacterSize(24);
                emptyText.setFillColor(sf::Color(100, 100, 100));
                emptyText.setPosition(sf::Vector2f(centerX + 120, centerY - 70 + i * 60));
                window.draw(emptyText);
            }
        }
        } else {
            sf::RectangleShape titleBar;
            titleBar.setFillColor(sf::Color(255, 100, 100));
            titleBar.setSize(sf::Vector2f(300, 40));
            titleBar.setPosition(sf::Vector2f(centerX - 150, centerY - 170));
            window.draw(titleBar);
            
            sf::RectangleShape restartHint;
            restartHint.setFillColor(sf::Color(150, 150, 255));
            restartHint.setSize(sf::Vector2f(200, 20));
            restartHint.setPosition(sf::Vector2f(centerX - 100, centerY + 160));
            window.draw(restartHint);
        }
    }
}

void drawJigtrizTitle(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded) {
    if (!fontLoaded) return;
    
    sf::Text titleText(font, "Jigtriz 0.2.5");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color(100, 255, 150));
    titleText.setStyle(sf::Text::Bold);
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(2);
    
    titleText.setPosition(sf::Vector2f(50, 1080 - 80));
    
    window.draw(titleText);
}

void drawPauseMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, PauseOption selectedOption) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    sf::RectangleShape menuBg;
    menuBg.setFillColor(sf::Color(40, 40, 50, 240));
    menuBg.setOutlineColor(sf::Color(100, 150, 255, 255));
    menuBg.setOutlineThickness(4);
    menuBg.setSize(sf::Vector2f(400, 350));
    menuBg.setPosition(sf::Vector2f(centerX - 200, centerY - 175));
    window.draw(menuBg);
    
    if (fontLoaded) {
        sf::Text pausedText(menuFont, "Jigtriz");
        pausedText.setCharacterSize(48);
        pausedText.setFillColor(sf::Color(100, 150, 255));
        pausedText.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = pausedText.getLocalBounds();
        pausedText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 140));
        window.draw(pausedText);
        
        sf::Text resumeText(menuFont, "RESUME");
        resumeText.setCharacterSize(32);
        if (selectedOption == PauseOption::Resume) {
            resumeText.setFillColor(sf::Color::Yellow);
            resumeText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 50));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(2);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY - 50));
            window.draw(selector);
        } else {
            resumeText.setFillColor(sf::Color::White);
        }
        sf::FloatRect resumeBounds = resumeText.getLocalBounds();
        resumeText.setPosition(sf::Vector2f(centerX - resumeBounds.size.x/2, centerY - 40));
        window.draw(resumeText);
        
        sf::Text restartText(menuFont, "RESTART");
        restartText.setCharacterSize(32);
        if (selectedOption == PauseOption::Restart) {
            restartText.setFillColor(sf::Color::Yellow);
            restartText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 50));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(2);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY + 20));
            window.draw(selector);
        } else {
            restartText.setFillColor(sf::Color::White);
        }
        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setPosition(sf::Vector2f(centerX - restartBounds.size.x/2, centerY + 30));
        window.draw(restartText);
        
        sf::Text quitText(menuFont, "QUIT TO MENU");
        quitText.setCharacterSize(32);
        if (selectedOption == PauseOption::QuitToMenu) {
            quitText.setFillColor(sf::Color::Yellow);
            quitText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 50));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(2);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY + 90));
            window.draw(selector);
        } else {
            quitText.setFillColor(sf::Color::White);
        }
        sf::FloatRect quitBounds = quitText.getLocalBounds();
        quitText.setPosition(sf::Vector2f(centerX - quitBounds.size.x/2, centerY + 100));
        window.draw(quitText);
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(14);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 155));
        window.draw(controlsText);
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(100, 150, 255));
        titleBar.setSize(sf::Vector2f(300, 60));
        titleBar.setPosition(sf::Vector2f(centerX - 150, centerY - 140));
        window.draw(titleBar);
        
        float optionY = centerY - 40;
        for (int i = 0; i < 3; i++) {
            sf::RectangleShape option;
            option.setFillColor(i == static_cast<int>(selectedOption) ? sf::Color::Yellow : sf::Color::White);
            option.setSize(sf::Vector2f(280, 40));
            option.setPosition(sf::Vector2f(centerX - 140, optionY + i * 70));
            window.draw(option);
        }
    }
}

void drawConfirmClearScores(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, ConfirmOption selectedOption) {

    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    

    sf::RectangleShape dialogBg;
    dialogBg.setFillColor(sf::Color(40, 40, 50, 240));
    dialogBg.setOutlineColor(sf::Color(100, 150, 255, 255));
    dialogBg.setOutlineThickness(4);
    dialogBg.setSize(sf::Vector2f(600, 280));
    dialogBg.setPosition(sf::Vector2f(centerX - 300, centerY - 140));
    window.draw(dialogBg);
    
    if (fontLoaded) {

        sf::Text titleText(menuFont, "Confirm");
        titleText.setCharacterSize(42);
        titleText.setFillColor(sf::Color(255, 100, 100));
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 110));
        window.draw(titleText);
        

        sf::Text questionText(menuFont, "Are you sure you want to clear");
        questionText.setCharacterSize(26);
        questionText.setFillColor(sf::Color::White);
        sf::FloatRect q1Bounds = questionText.getLocalBounds();
        questionText.setPosition(sf::Vector2f(centerX - q1Bounds.size.x/2, centerY - 50));
        window.draw(questionText);
        
        sf::Text questionText2(menuFont, "all your scores?");
        questionText2.setCharacterSize(26);
        questionText2.setFillColor(sf::Color::White);
        sf::FloatRect q2Bounds = questionText2.getLocalBounds();
        questionText2.setPosition(sf::Vector2f(centerX - q2Bounds.size.x/2, centerY - 20));
        window.draw(questionText2);
        

        sf::Text yesText(menuFont, "YES");
        yesText.setCharacterSize(36);
        if (selectedOption == ConfirmOption::Yes) {
            yesText.setFillColor(sf::Color::Yellow);
            yesText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape yesSelector;
            yesSelector.setSize(sf::Vector2f(200, 60));
            yesSelector.setFillColor(sf::Color(255, 255, 0, 50));
            yesSelector.setOutlineColor(sf::Color::Yellow);
            yesSelector.setOutlineThickness(3);
            yesSelector.setPosition(sf::Vector2f(centerX - 250, centerY + 40));
            window.draw(yesSelector);
        } else {
            yesText.setFillColor(sf::Color::White);
        }
        sf::FloatRect yesBounds = yesText.getLocalBounds();
        yesText.setPosition(sf::Vector2f(centerX - 150 - yesBounds.size.x/2, centerY + 55));
        window.draw(yesText);
        

        sf::Text noText(menuFont, "NO");
        noText.setCharacterSize(36);
        if (selectedOption == ConfirmOption::No) {
            noText.setFillColor(sf::Color::Yellow);
            noText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape noSelector;
            noSelector.setSize(sf::Vector2f(200, 60));
            noSelector.setFillColor(sf::Color(255, 255, 0, 50));
            noSelector.setOutlineColor(sf::Color::Yellow);
            noSelector.setOutlineThickness(3);
            noSelector.setPosition(sf::Vector2f(centerX + 50, centerY + 40));
            window.draw(noSelector);
        } else {
            noText.setFillColor(sf::Color::White);
        }
        sf::FloatRect noBounds = noText.getLocalBounds();
        noText.setPosition(sf::Vector2f(centerX + 150 - noBounds.size.x/2, centerY + 55));
        window.draw(noText);
        

        sf::Text instructText(menuFont, "Arrow keys to select, SPACE/ENTER to confirm");
        instructText.setCharacterSize(18);
        instructText.setFillColor(sf::Color(180, 180, 180));
        sf::FloatRect instructBounds = instructText.getLocalBounds();
        instructText.setPosition(sf::Vector2f(centerX - instructBounds.size.x/2, centerY + 120));
        window.draw(instructText);
    }
}

void drawMainMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, MenuOption selectedOption, bool debugMode) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "JIGTRIZ");
        titleText.setCharacterSize(128);
        titleText.setFillColor(sf::Color(100, 255, 150));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 350));
        window.draw(titleText);
        
        sf::Text versionText(menuFont, "v0.2.5");
        versionText.setCharacterSize(24);
        versionText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect versionBounds = versionText.getLocalBounds();
        versionText.setPosition(sf::Vector2f(centerX - versionBounds.size.x/2, centerY - 200));
        window.draw(versionText);
        
        sf::Text startText(menuFont, "START");
        startText.setCharacterSize(48);
        if (selectedOption == MenuOption::Start) {
            startText.setFillColor(sf::Color::Yellow);
            startText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY - 90));
            window.draw(selector);
        } else {
            startText.setFillColor(sf::Color::White);
        }
        sf::FloatRect startBounds = startText.getLocalBounds();
        startText.setPosition(sf::Vector2f(centerX - startBounds.size.x/2, centerY - 90));
        window.draw(startText);
        
        sf::Text jigtrizopediaText(menuFont, "JIGTRIZOPEDIA");
        jigtrizopediaText.setCharacterSize(48);
        if (selectedOption == MenuOption::Jigtrizopedia) {
            jigtrizopediaText.setFillColor(sf::Color::Yellow);
            jigtrizopediaText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(400, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 200, centerY + 10));
            window.draw(selector);
        } else {
            jigtrizopediaText.setFillColor(sf::Color::White);
        }
        sf::FloatRect jigtrizopediaBounds = jigtrizopediaText.getLocalBounds();
        jigtrizopediaText.setPosition(sf::Vector2f(centerX - jigtrizopediaBounds.size.x/2, centerY+10));
        window.draw(jigtrizopediaText);
        
        sf::Text optionsText(menuFont, "OPTIONS");
        optionsText.setCharacterSize(48);
        if (selectedOption == MenuOption::Options) {
            optionsText.setFillColor(sf::Color::Yellow);
            optionsText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY + 110));
            window.draw(selector);
        } else {
            optionsText.setFillColor(sf::Color::White);
        }
        sf::FloatRect optionsBounds = optionsText.getLocalBounds();
        optionsText.setPosition(sf::Vector2f(centerX - optionsBounds.size.x/2, centerY + 110));
        window.draw(optionsText);
        
        sf::Text exitText(menuFont, "EXIT");
        exitText.setCharacterSize(48);
        if (selectedOption == MenuOption::Exit) {
            exitText.setFillColor(sf::Color::Yellow);
            exitText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(300, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 150, centerY + 210));
            window.draw(selector);
        } else {
            exitText.setFillColor(sf::Color::White);
        }
        sf::FloatRect exitBounds = exitText.getLocalBounds();
        exitText.setPosition(sf::Vector2f(centerX - exitBounds.size.x/2, centerY + 210));
        window.draw(exitText);
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 300));
        window.draw(controlsText);
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(100, 255, 150));
        titleBar.setSize(sf::Vector2f(400, 80));
        titleBar.setPosition(sf::Vector2f(centerX - 200, centerY - 250));
        window.draw(titleBar);
        
        float optionY = centerY - 90;
        for (int i = 0; i < 3; i++) {
            sf::RectangleShape option;
            option.setSize(sf::Vector2f(300, 60));
            if ((i == 0 && selectedOption == MenuOption::Start) || 
                (i == 1 && selectedOption == MenuOption::Options) || 
                (i == 2 && selectedOption == MenuOption::Exit)) {
                option.setFillColor(sf::Color::Yellow);
            } else {
                option.setFillColor(sf::Color::White);
            }
            option.setPosition(sf::Vector2f(centerX - 150, optionY + i * 100));
            window.draw(option);
        }
    }
    
    if (debugMode && fontLoaded) {
        sf::Text debugText(menuFont);
        debugText.setString("DEBUG MODE");
        debugText.setCharacterSize(24);
        debugText.setFillColor(sf::Color::Yellow);
        debugText.setPosition(sf::Vector2f(1920.0f - 180.0f, 1080.0f - 40.0f));
        window.draw(debugText);
    }
}

void drawGameModeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, GameModeOption selectedOption) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "SELECT GAME MODE");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color(100, 255, 150));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 300));
        window.draw(titleText);
        
        std::vector<std::pair<std::string, GameModeOption>> options = {
            {"CLASSIC", GameModeOption::Classic},
            {"BLITZ", GameModeOption::Sprint},
            {"CHALLENGE", GameModeOption::Challenge}
        };
        
        float startY = centerY - 80;
        float spacing = 90;
        
        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text optionText(menuFont, options[i].first);
            optionText.setCharacterSize(48);
            

            bool isDisabled = (options[i].second == GameModeOption::Challenge);
            
            if (selectedOption == options[i].second) {
                if (isDisabled) {
                    optionText.setFillColor(sf::Color(100, 100, 100));
                } else {
                    optionText.setFillColor(sf::Color::Yellow);
                    optionText.setStyle(sf::Text::Bold);
                    
                    sf::RectangleShape selector;
                    selector.setSize(sf::Vector2f(400, 60));
                    selector.setFillColor(sf::Color(255, 255, 0, 50));
                    selector.setOutlineColor(sf::Color::Yellow);
                    selector.setOutlineThickness(3);
                    selector.setPosition(sf::Vector2f(centerX - 200, startY + i * spacing - 5));
                    window.draw(selector);
                }
            } else {
                if (isDisabled) {
                    optionText.setFillColor(sf::Color(80, 80, 80));
                } else {
                    optionText.setFillColor(sf::Color::White);
                }
            }
            
            sf::FloatRect optionBounds = optionText.getLocalBounds();
            optionText.setPosition(sf::Vector2f(centerX - optionBounds.size.x/2, startY + i * spacing));
            window.draw(optionText);
            

            if (isDisabled) {
                sf::Text disabledText(menuFont, "(Coming Soon)");
                disabledText.setCharacterSize(24);
                disabledText.setFillColor(sf::Color(120, 120, 120));
                sf::FloatRect disabledBounds = disabledText.getLocalBounds();
                disabledText.setPosition(sf::Vector2f(centerX - disabledBounds.size.x/2, startY + i * spacing + 50));
                window.draw(disabledText);
            }
        }
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 350));
        window.draw(controlsText);
    }
}

void drawClassicDifficultyMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ClassicDifficulty selectedOption, const SaveData& saveData) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "CLASSIC MODE");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color(100, 200, 255));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 300));
        window.draw(titleText);
        
        std::vector<std::tuple<std::string, ClassicDifficulty, int>> options = {
            {"EASY", ClassicDifficulty::Easy, saveData.highScoreClassicEasy},
            {"MEDIUM", ClassicDifficulty::Medium, saveData.highScoreClassicMedium},
            {"HARD", ClassicDifficulty::Hard, saveData.highScoreClassicHard}
        };
        
        float startY = centerY - 80;
        float spacing = 90;
        
        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text optionText(menuFont, std::get<0>(options[i]));
            optionText.setCharacterSize(48);
            
            if (selectedOption == std::get<1>(options[i])) {
                optionText.setFillColor(sf::Color::Yellow);
                optionText.setStyle(sf::Text::Bold);
                
                sf::RectangleShape selector;
                selector.setSize(sf::Vector2f(550, 60));
                selector.setFillColor(sf::Color(255, 255, 0, 50));
                selector.setOutlineColor(sf::Color::Yellow);
                selector.setOutlineThickness(3);
                selector.setPosition(sf::Vector2f(centerX - 275, startY + i * spacing - 5));
                window.draw(selector);
            } else {
                optionText.setFillColor(sf::Color::White);
            }
            
            sf::FloatRect optionBounds = optionText.getLocalBounds();
            optionText.setPosition(sf::Vector2f(centerX - 200, startY + i * spacing));
            window.draw(optionText);
            

            int highScore = std::get<2>(options[i]);
            sf::Text scoreText(menuFont, "Best: " + std::to_string(highScore));
            scoreText.setCharacterSize(32);
            scoreText.setFillColor(sf::Color(180, 180, 180));
            scoreText.setPosition(sf::Vector2f(centerX + 80, startY + i * spacing + 8));
            window.draw(scoreText);
        }
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 350));
        window.draw(controlsText);
    }
}

void drawSprintLinesMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, SprintLines selectedOption, const SaveData& saveData, bool debugMode) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "BLITZ MODE");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color(255, 200, 100));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 300));
        window.draw(titleText);
        
        std::vector<std::tuple<std::string, SprintLines, float>> options;
        
        options.push_back({"24 LINES", SprintLines::Lines24, saveData.bestTimeSprint24});
        options.push_back({"48 LINES", SprintLines::Lines48, saveData.bestTimeSprint48});
        options.push_back({"96 LINES", SprintLines::Lines96, saveData.bestTimeSprint96});
        if (debugMode) {
            options.push_back({"1 LINE (DEBUG)", SprintLines::Lines1, saveData.bestTimeSprint1});
        }
        
        float startY = centerY - 80;
        float spacing = 90;
        
        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text optionText(menuFont, std::get<0>(options[i]));
            optionText.setCharacterSize(48);
            
            if (selectedOption == std::get<1>(options[i])) {
                optionText.setFillColor(sf::Color::Yellow);
                optionText.setStyle(sf::Text::Bold);
                
                sf::RectangleShape selector;
                selector.setSize(sf::Vector2f(550, 60));
                selector.setFillColor(sf::Color(255, 255, 0, 50));
                selector.setOutlineColor(sf::Color::Yellow);
                selector.setOutlineThickness(3);
                selector.setPosition(sf::Vector2f(centerX - 275, startY + i * spacing - 5));
                window.draw(selector);
            } else {
                optionText.setFillColor(sf::Color::White);
            }
            
            sf::FloatRect optionBounds = optionText.getLocalBounds();
            optionText.setPosition(sf::Vector2f(centerX - 180, startY + i * spacing));
            window.draw(optionText);
            

            float bestTime = std::get<2>(options[i]);
            std::string timeStr;
            if (bestTime > 0.0f) {
                int minutes = static_cast<int>(bestTime) / 60;
                int seconds = static_cast<int>(bestTime) % 60;
                int milliseconds = static_cast<int>((bestTime - static_cast<int>(bestTime)) * 100);
                timeStr = std::to_string(minutes) + ":" + 
                         (seconds < 10 ? "0" : "") + std::to_string(seconds) + "." +
                         (milliseconds < 10 ? "0" : "") + std::to_string(milliseconds);
            } else {
                timeStr = "--:--";
            }
            
            sf::Text timeText(menuFont, "Best: " + timeStr);
            timeText.setCharacterSize(32);
            timeText.setFillColor(sf::Color(180, 180, 180));
            timeText.setPosition(sf::Vector2f(centerX + 80, startY + i * spacing + 8));
            window.draw(timeText);
        }
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 350));
        window.draw(controlsText);
    }
}

void drawChallengeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ChallengeMode selectedOption) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "CHALLENGE MODE");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color(255, 100, 255));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 300));
        window.draw(titleText);
        
        std::vector<std::pair<std::string, ChallengeMode>> options = {
            {"THE FOREST", ChallengeMode::TheForest},
            {"RANDOMNESS", ChallengeMode::Randomness},
            {"NON STRAIGHT", ChallengeMode::NonStraight}
        };
        
        float startY = centerY - 80;
        float spacing = 90;
        
        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text optionText(menuFont, options[i].first);
            optionText.setCharacterSize(48);
            
            if (selectedOption == options[i].second) {
                optionText.setFillColor(sf::Color::Yellow);
                optionText.setStyle(sf::Text::Bold);
                
                sf::RectangleShape selector;
                selector.setSize(sf::Vector2f(500, 60));
                selector.setFillColor(sf::Color(255, 255, 0, 50));
                selector.setOutlineColor(sf::Color::Yellow);
                selector.setOutlineThickness(3);
                selector.setPosition(sf::Vector2f(centerX - 250, startY + i * spacing - 5));
                window.draw(selector);
            } else {
                optionText.setFillColor(sf::Color::White);
            }
            
            sf::FloatRect optionBounds = optionText.getLocalBounds();
            optionText.setPosition(sf::Vector2f(centerX - optionBounds.size.x/2, startY + i * spacing));
            window.draw(optionText);
        }
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 350));
        window.draw(controlsText);
    }
}

void drawOptionsMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, bool debugMode, OptionsMenuOption selectedOption) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(menuFont, "OPTIONS");
        titleText.setCharacterSize(128);
        titleText.setFillColor(sf::Color(100, 255, 150));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 320));
        window.draw(titleText);
        
        sf::Text versionText(menuFont, "Settings");
        versionText.setCharacterSize(24);
        versionText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect versionBounds = versionText.getLocalBounds();
        versionText.setPosition(sf::Vector2f(centerX - versionBounds.size.x/2, centerY - 170));
        window.draw(versionText);
        
        sf::Text clearText(menuFont, "CLEAR SCORES");
        clearText.setCharacterSize(48);
        if (selectedOption == OptionsMenuOption::ClearScores) {
            clearText.setFillColor(sf::Color::Yellow);
            clearText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(400, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 200, centerY - 85));
            window.draw(selector);
        } else {
            clearText.setFillColor(sf::Color::White);
        }
        sf::FloatRect clearBounds = clearText.getLocalBounds();
        clearText.setPosition(sf::Vector2f(centerX - clearBounds.size.x/2, centerY - 90));
        window.draw(clearText);
        
        sf::Text rebindText(menuFont, "REBIND KEYS");
        rebindText.setCharacterSize(48);
        if (selectedOption == OptionsMenuOption::RebindKeys) {
            rebindText.setFillColor(sf::Color::Yellow);
            rebindText.setStyle(sf::Text::Bold);
            
            sf::RectangleShape selector;
            selector.setSize(sf::Vector2f(400, 60));
            selector.setFillColor(sf::Color(255, 255, 0, 50));
            selector.setOutlineColor(sf::Color::Yellow);
            selector.setOutlineThickness(3);
            selector.setPosition(sf::Vector2f(centerX - 200, centerY + 15));
            window.draw(selector);
        } else {
            rebindText.setFillColor(sf::Color::White);
        }
        sf::FloatRect rebindBounds = rebindText.getLocalBounds();
        rebindText.setPosition(sf::Vector2f(centerX - rebindBounds.size.x/2, centerY + 10));
        window.draw(rebindText);
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 230));
        window.draw(controlsText);
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(100, 255, 150));
        titleBar.setSize(sf::Vector2f(400, 80));
        titleBar.setPosition(sf::Vector2f(centerX - 200, centerY - 250));
        window.draw(titleBar);
        
        float optionY = centerY - 90;
        for (int i = 0; i < 2; i++) {
            sf::RectangleShape option;
            option.setSize(sf::Vector2f(400, 60));
            if ((i == 0 && selectedOption == OptionsMenuOption::ClearScores) || 
                (i == 1 && selectedOption == OptionsMenuOption::RebindKeys)) {
                option.setFillColor(sf::Color::Yellow);
            } else {
                option.setFillColor(sf::Color::White);
            }
            option.setPosition(sf::Vector2f(centerX - 200, optionY + i * 100));
            window.draw(option);
        }
    }
    
    if (debugMode && fontLoaded) {
        sf::Text debugText(menuFont);
        debugText.setString("DEBUG MODE");
        debugText.setCharacterSize(24);
        debugText.setFillColor(sf::Color::Yellow);
        debugText.setPosition(sf::Vector2f(1920.0f - 180.0f, 1080.0f - 40.0f));
        window.draw(debugText);
    }
}

std::string getKeyName(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::A: return "A";
        case sf::Keyboard::Key::B: return "B";
        case sf::Keyboard::Key::C: return "C";
        case sf::Keyboard::Key::D: return "D";
        case sf::Keyboard::Key::E: return "E";
        case sf::Keyboard::Key::F: return "F";
        case sf::Keyboard::Key::G: return "G";
        case sf::Keyboard::Key::H: return "H";
        case sf::Keyboard::Key::I: return "I";
        case sf::Keyboard::Key::J: return "J";
        case sf::Keyboard::Key::K: return "K";
        case sf::Keyboard::Key::L: return "L";
        case sf::Keyboard::Key::M: return "M";
        case sf::Keyboard::Key::N: return "N";
        case sf::Keyboard::Key::O: return "O";
        case sf::Keyboard::Key::P: return "P";
        case sf::Keyboard::Key::Q: return "Q";
        case sf::Keyboard::Key::R: return "R";
        case sf::Keyboard::Key::S: return "S";
        case sf::Keyboard::Key::T: return "T";
        case sf::Keyboard::Key::U: return "U";
        case sf::Keyboard::Key::V: return "V";
        case sf::Keyboard::Key::W: return "W";
        case sf::Keyboard::Key::X: return "X";
        case sf::Keyboard::Key::Y: return "Y";
        case sf::Keyboard::Key::Z: return "Z";
        case sf::Keyboard::Key::Space: return "SPACE";
        case sf::Keyboard::Key::Left: return "LEFT";
        case sf::Keyboard::Key::Right: return "RIGHT";
        case sf::Keyboard::Key::Up: return "UP";
        case sf::Keyboard::Key::Down: return "DOWN";
        case sf::Keyboard::Key::Tab: return "TAB";
        case sf::Keyboard::Key::LShift: return "LSHIFT";
        case sf::Keyboard::Key::RShift: return "RSHIFT";
        case sf::Keyboard::Key::LControl: return "LCTRL";
        case sf::Keyboard::Key::RControl: return "RCTRL";
        case sf::Keyboard::Key::Escape: return "ESC";
        case sf::Keyboard::Key::Comma: return "COMMA";
        case sf::Keyboard::Key::Period: return "PERIOD";
        case sf::Keyboard::Key::Slash: return "SLASH";
        case sf::Keyboard::Key::Semicolon: return "SEMICOLON";
        case sf::Keyboard::Key::Enter: return "ENTER";
        case sf::Keyboard::Key::Backspace: return "BACKSPACE";
        default: return "UNKNOWN";
    }
}

void drawRebindingScreen(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, const KeyBindings& bindings, int selectedBinding, bool waitingForKey) {
    const float centerX = 1920.0f / 2.0f;
    const float startY = 150.0f;
    const float lineHeight = 50.0f;
    
    if (fontLoaded) {
        sf::Text titleText(menuFont);
        titleText.setString("REBIND KEYS");
        titleText.setCharacterSize(56);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
        titleText.setPosition(sf::Vector2f(centerX, 80.0f));
        window.draw(titleText);
        
        std::vector<std::pair<std::string, std::string>> keyList = {
            {"Move Left", getKeyName(bindings.moveLeft)},
            {"Move Right", getKeyName(bindings.moveRight)},
            {"Rotate Left", getKeyName(bindings.rotateLeft)},
            {"Rotate Right", getKeyName(bindings.rotateRight)},
            {"Fall", getKeyName(bindings.quickFall)},
            {"Drop", getKeyName(bindings.drop)},
            {"Hold", getKeyName(bindings.hold)},
            {"Bomb", getKeyName(bindings.bomb)},
            {"Mute", getKeyName(bindings.mute)},
            {"Volume Down", getKeyName(bindings.volumeDown)},
            {"Volume Up", getKeyName(bindings.volumeUp)},
            {"Menu", getKeyName(bindings.menu)},
            {"RESTORE DEFAULT", ""},
        };
        
        for (size_t i = 0; i < keyList.size(); i++) {

            float extraSpace = (i == 12) ? 30.0f : 0.0f;
            float yPos = startY + i * lineHeight + extraSpace;
            

            if (static_cast<int>(i) == selectedBinding) {
                sf::RectangleShape selector;
                selector.setSize(sf::Vector2f(800, 45));

                if (i == 12) {
                    selector.setFillColor(sf::Color(255, 165, 0, 150));
                    selector.setOutlineColor(sf::Color(255, 215, 0));
                    selector.setOutlineThickness(4);
                } else {
                    selector.setFillColor(sf::Color(100, 200, 255, 128));
                }
                selector.setPosition(sf::Vector2f(centerX - 400, yPos - 5));
                window.draw(selector);
            }
            
            sf::Text labelText(menuFont);
            labelText.setString(keyList[i].first);
            labelText.setCharacterSize(28);
            
            if (i == 12) {

                if (static_cast<int>(i) == selectedBinding) {
                    labelText.setFillColor(sf::Color::Yellow);
                    labelText.setStyle(sf::Text::Bold);
                } else {
                    labelText.setFillColor(sf::Color(255, 200, 100));
                    labelText.setStyle(sf::Text::Bold);
                }
            } else {
                labelText.setFillColor(sf::Color::White);
            }
            
            labelText.setPosition(sf::Vector2f(centerX - 380, yPos));
            window.draw(labelText);
            
            if (!keyList[i].second.empty()) {
                sf::Text keyText(menuFont);
                if (static_cast<int>(i) == selectedBinding && waitingForKey) {
                    keyText.setString("Press any key...");
                    keyText.setFillColor(sf::Color::Yellow);
                } else {
                    keyText.setString(keyList[i].second);
                    keyText.setFillColor(sf::Color(100, 255, 100));
                }
                keyText.setCharacterSize(28);
                keyText.setStyle(sf::Text::Bold);
                keyText.setPosition(sf::Vector2f(centerX + 150, yPos));
                window.draw(keyText);
            }
        }
        
        sf::Text instructionsText(menuFont);
        if (waitingForKey) {
            instructionsText.setString("Press a key to bind | ESC to cancel");
        } else {
            instructionsText.setString("W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        }
        instructionsText.setCharacterSize(22);
        instructionsText.setFillColor(sf::Color(200, 200, 200));
        sf::FloatRect instrBounds = instructionsText.getLocalBounds();
        instructionsText.setOrigin(sf::Vector2f(instrBounds.size.x / 2.0f, instrBounds.size.y / 2.0f));
        instructionsText.setPosition(sf::Vector2f(centerX, 1020.0f));
        window.draw(instructionsText);
    }
}

void drawExplosionEffects(sf::RenderWindow& window, const std::vector<ExplosionEffect>& explosions) {
    for (const auto& explosion : explosions) {
        float worldX = GRID_OFFSET_X + explosion.x * CELL_SIZE;
        float worldY = GRID_OFFSET_Y + explosion.y * CELL_SIZE;
        
        sf::RectangleShape explosionBox;
        explosionBox.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        explosionBox.setPosition(sf::Vector2f(worldX, worldY));
        explosionBox.setFillColor(sf::Color::White);
        
        window.draw(explosionBox);
    }
}

void drawGlowEffects(sf::RenderWindow& window, const std::vector<GlowEffect>& glowEffects, const std::map<TextureType, sf::Texture>& textures) {
    for (const auto& glow : glowEffects) {
        if (textures.find(TextureType::GenericBlock) != textures.end()) {
            sf::Sprite glowSprite(textures.at(TextureType::GenericBlock));
            glowSprite.setPosition(sf::Vector2f(glow.worldX, glow.worldY));
            
            sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
            float scale = (CELL_SIZE * 1.0f) / textureSize.x;
            glowSprite.setScale(sf::Vector2f(scale, scale));
            
            sf::Color glowColor = glow.color;
            glowColor.r = std::min(255, static_cast<int>(glowColor.r + (255 - glowColor.r) * 0.66f));
            glowColor.g = std::min(255, static_cast<int>(glowColor.g + (255 - glowColor.g) * 0.66f));
            glowColor.b = std::min(255, static_cast<int>(glowColor.b + (255 - glowColor.b) * 0.66f));
            glowColor.a = static_cast<std::uint8_t>(glow.alpha);
            
            glowSprite.setColor(glowColor);
            window.draw(glowSprite);
        } else {
            sf::RectangleShape glowBox;
            glowBox.setSize(sf::Vector2f(CELL_SIZE * 1.0f, CELL_SIZE * 1.0f));
            glowBox.setPosition(sf::Vector2f(glow.worldX, glow.worldY));
            
            sf::Color glowColor = glow.color;
            glowColor.a = static_cast<std::uint8_t>(glow.alpha);
            glowBox.setFillColor(glowColor);
            
            window.draw(glowBox);
        }
    }
}

void drawJigtrizopediaMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, JigtrizopediaOption selectedOption) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "JIGTRIZOPEDIA");
        titleText.setCharacterSize(96);
        titleText.setFillColor(sf::Color(150, 200, 255));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 380));
        window.draw(titleText);
        
        std::vector<std::pair<std::string, JigtrizopediaOption>> options = {
            {"JIGTRIZ PIECES", JigtrizopediaOption::JigtrizPieces},
            {"ACHIEVEMENTS", JigtrizopediaOption::Achievements},
            {"STATISTICS", JigtrizopediaOption::Statistics}
        };
        
        float startY = centerY - 200;
        float spacing = 70;
        
        for (size_t i = 0; i < options.size(); ++i) {
            sf::Text optionText(menuFont, options[i].first);
            optionText.setCharacterSize(40);
            

            optionText.setFillColor(sf::Color(100, 100, 100));
            
            sf::FloatRect optionBounds = optionText.getLocalBounds();
            optionText.setPosition(sf::Vector2f(centerX - optionBounds.size.x/2, startY + i * spacing));
            window.draw(optionText);
        }
        
        sf::Text controlsText(menuFont, "W/S or UP/DOWN to select | SPACE/ENTER to confirm | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, centerY + 380));
        window.draw(controlsText);
    }
}

void drawAchievementsScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, int hoveredAchievement) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 230));
    overlay.setSize(sf::Vector2f(1920, 1080));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = 1920 / 2.0f;
    float centerY = 1080 / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "ACHIEVEMENTS");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color(255, 215, 0));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, 80));
        window.draw(titleText);
        
        const int COLS = 5;
        const int ROWS = 4;
        const float CELL_SIZE = 120.0f;
        const float SPACING = 20.0f;
        const float GRID_WIDTH = COLS * CELL_SIZE + (COLS - 1) * SPACING;
        const float GRID_HEIGHT = ROWS * CELL_SIZE + (ROWS - 1) * SPACING;
        const float START_X = centerX - GRID_WIDTH / 2.0f;
        const float START_Y = 250;
        
        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                int achievementId = row * COLS + col;
                float x = START_X + col * (CELL_SIZE + SPACING);
                float y = START_Y + row * (CELL_SIZE + SPACING);
                
                sf::RectangleShape achievementBox;
                achievementBox.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                achievementBox.setPosition(sf::Vector2f(x, y));
                
                if (achievementId == hoveredAchievement) {
                    achievementBox.setFillColor(sf::Color(80, 80, 120, 220));
                    achievementBox.setOutlineColor(sf::Color(255, 215, 0));
                    achievementBox.setOutlineThickness(4);
                } else {
                    achievementBox.setFillColor(sf::Color(40, 40, 60, 200));
                    achievementBox.setOutlineColor(sf::Color(100, 100, 140));
                    achievementBox.setOutlineThickness(2);
                }
                
                window.draw(achievementBox);
                
                sf::Text lockText(menuFont, "?");
                lockText.setCharacterSize(60);
                lockText.setFillColor(sf::Color(120, 120, 120));
                lockText.setStyle(sf::Text::Bold);
                sf::FloatRect lockBounds = lockText.getLocalBounds();
                lockText.setPosition(sf::Vector2f(x + CELL_SIZE/2 - lockBounds.size.x/2, y + CELL_SIZE/2 - lockBounds.size.y/2 - 10));
                window.draw(lockText);
            }
        }
        
        if (hoveredAchievement >= 0 && hoveredAchievement < 20) {
            sf::RectangleShape infoBg;
            infoBg.setSize(sf::Vector2f(700, 150));
            infoBg.setFillColor(sf::Color(20, 20, 40, 240));
            infoBg.setOutlineColor(sf::Color(255, 215, 0));
            infoBg.setOutlineThickness(3);
            infoBg.setPosition(sf::Vector2f(centerX - 350, 850));
            window.draw(infoBg);
            
            std::string achievementName = "Achievement_";
            if (hoveredAchievement < 9) achievementName += "0";
            achievementName += std::to_string(hoveredAchievement + 1);
            
            std::string achievementDesc = "Description_";
            if (hoveredAchievement < 9) achievementDesc += "0";
            achievementDesc += std::to_string(hoveredAchievement + 1);
            
            sf::Text nameText(menuFont, achievementName);
            nameText.setCharacterSize(32);
            nameText.setFillColor(sf::Color(255, 215, 0));
            nameText.setStyle(sf::Text::Bold);
            sf::FloatRect nameBounds = nameText.getLocalBounds();
            nameText.setPosition(sf::Vector2f(centerX - nameBounds.size.x/2, 870));
            window.draw(nameText);
            
            sf::Text descText(menuFont, achievementDesc);
            descText.setCharacterSize(24);
            descText.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect descBounds = descText.getLocalBounds();
            descText.setPosition(sf::Vector2f(centerX - descBounds.size.x/2, 920));
            window.draw(descText);
        }
        
        sf::Text controlsText(menuFont, "Hover over achievements to see details | ESC to go back");
        controlsText.setCharacterSize(20);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect controlsBounds = controlsText.getLocalBounds();
        controlsText.setPosition(sf::Vector2f(centerX - controlsBounds.size.x/2, 1020));
        window.draw(controlsText);
    }
}
