#include "game_ui.h"
#include "piece_utils.h"
#include <iostream>

void drawGridBackground(sf::RenderWindow& window) {
}

void drawCell(sf::RenderWindow& window, float x, float y, float cellSize, const sf::Color& color, 
              TextureType texType, const std::map<TextureType, sf::Texture>& textures, 
              bool useTextures, const sf::Transform& transform) {
    if (useTextures) {

        TextureType textureToUse = texType;
        

        if (textures.find(textureToUse) == textures.end()) {
            textureToUse = TextureType::GenericBlock;
        }
        
        if (textures.find(textureToUse) != textures.end()) {
            sf::Sprite sprite(textures.at(textureToUse));
            sprite.setPosition(sf::Vector2f(x, y));
            

            if (textureToUse == TextureType::A_Bomb) {
                sprite.setColor(sf::Color::White);
            } else {
                sprite.setColor(color);
            }
            
            sf::Vector2u textureSize = textures.at(textureToUse).getSize();
            sprite.setScale(sf::Vector2f(cellSize / textureSize.x, cellSize / textureSize.y));
            window.draw(sprite, transform);
            return;
        }
    }
    

    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(cellSize, cellSize));
    rect.setPosition(sf::Vector2f(x, y));
    rect.setFillColor(color);
    window.draw(rect, transform);
}

void drawGridBorder(sf::RenderWindow& window) {
    sf::RectangleShape border;
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(100, 150, 255, 255));
    border.setOutlineThickness(BORDER_WIDTH);
    float borderX = GRID_OFFSET_X - BORDER_WIDTH;
    float borderY = GRID_OFFSET_Y - BORDER_WIDTH;
    float borderW = GRID_WIDTH * CELL_SIZE + (2 * BORDER_WIDTH);
    float borderH = GRID_HEIGHT * CELL_SIZE + (2 * BORDER_WIDTH);
    border.setPosition(sf::Vector2f(borderX, borderY));
    border.setSize(sf::Vector2f(borderW, borderH));
    window.draw(border);
}

void drawBombAbility(sf::RenderWindow& window, bool isAvailable, int linesSinceLastAbility, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, bool infiniteBombs) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y + 300;

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(30, 30, 40, 200));
    if (isAvailable || infiniteBombs) {
        panelBg.setOutlineColor(sf::Color(255, 100, 100, 255));
    } else {
        panelBg.setOutlineColor(sf::Color(100, 100, 100, 255));
    }
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(120, 180));
    window.draw(panelBg);
    
    if (fontLoaded) {
        sf::Text titleText(font, "BOMB");
        titleText.setCharacterSize(24);
        titleText.setFillColor((isAvailable || infiniteBombs) ? sf::Color(255, 100, 100) : sf::Color(100, 100, 100));
        titleText.setPosition(sf::Vector2f(panelX + 25, panelY));
        window.draw(titleText);
    } else {
        sf::RectangleShape titleLabel;
        titleLabel.setFillColor((isAvailable || infiniteBombs) ? sf::Color(255, 100, 100) : sf::Color(100, 100, 100));
        titleLabel.setSize(sf::Vector2f(60, 8));
        titleLabel.setPosition(sf::Vector2f(panelX + 20, panelY));
        window.draw(titleLabel);
    }
    
    if (isAvailable || infiniteBombs) {
        PieceShape bombShape = getPieceShape(PieceType::A_Bomb);
        TextureType texType = TextureType::A_Bomb;
        
        float centerX = panelX + 50;
        float centerY = panelY + 70;
        float miniCellSize = 64.0f;
        
        if (useTextures && textures.find(texType) != textures.end()) {
            sf::Sprite miniSprite(textures.at(texType));
            miniSprite.setPosition(sf::Vector2f(centerX - miniCellSize/2, centerY - miniCellSize/2));
            sf::Vector2u textureSize = textures.at(texType).getSize();
            miniSprite.setScale(sf::Vector2f(miniCellSize / textureSize.x, miniCellSize / textureSize.y));
            window.draw(miniSprite);
        } else {
            sf::RectangleShape miniCell;
            miniCell.setSize(sf::Vector2f(miniCellSize, miniCellSize));
            miniCell.setPosition(sf::Vector2f(centerX - miniCellSize/2, centerY - miniCellSize/2));
            miniCell.setFillColor(bombShape.color);
            window.draw(miniCell);
        }
        
        if (fontLoaded) {
            sf::Text readyText(font, infiniteBombs ? "INFINITE" : "READY!");
            readyText.setCharacterSize(infiniteBombs ? 22 : 25);
            readyText.setFillColor(infiniteBombs ? sf::Color(255, 215, 0) : sf::Color(100, 255, 100));
            readyText.setPosition(sf::Vector2f(panelX + (infiniteBombs ? 15 : 22), panelY + 100));
            window.draw(readyText);
            
            sf::Text keyText(font, "Press 'I'");
            keyText.setCharacterSize(16);
            keyText.setFillColor(sf::Color(200, 200, 200));
            keyText.setPosition(sf::Vector2f(panelX + 22, panelY + 135));
            window.draw(keyText);
        }
    } else {
        int linesNeeded = 10 - linesSinceLastAbility;
        
        if (fontLoaded) {
            sf::Text progressText(font, std::to_string(linesSinceLastAbility) + " / 10");
            progressText.setCharacterSize(25);
            progressText.setFillColor(sf::Color(200, 200, 200));
            progressText.setPosition(sf::Vector2f(panelX + 22, panelY + 100));
            window.draw(progressText);
            
            sf::Text needText(font, std::to_string(linesNeeded) + " more");
            needText.setCharacterSize(16);
            needText.setFillColor(sf::Color(150, 150, 150));
            needText.setPosition(sf::Vector2f(panelX + 22, panelY + 135));
            window.draw(needText);
        } else {
            sf::RectangleShape progressBar;
            float progress = linesSinceLastAbility / 10.0f;
            progressBar.setFillColor(sf::Color(100, 150, 200));
            progressBar.setSize(sf::Vector2f(80 * progress, 10));
            progressBar.setPosition(sf::Vector2f(panelX + 10, panelY + 40));
            window.draw(progressBar);
        }
    }
}

void drawHeldPiece(sf::RenderWindow& window, PieceType heldType, bool hasHeld, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y + 200;

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(30, 30, 40, 200));
    panelBg.setOutlineColor(sf::Color(255, 150, 100, 255));
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(120, 80));
    window.draw(panelBg);
    
    if (fontLoaded) {
        sf::Text holdLabel(font, "HOLD");
        holdLabel.setCharacterSize(14);
        holdLabel.setFillColor(sf::Color(255, 150, 100));
        holdLabel.setStyle(sf::Text::Bold);
        holdLabel.setPosition(sf::Vector2f(panelX + 35, panelY + 5));
        window.draw(holdLabel);
    } else {
        sf::RectangleShape holdLabel;
        holdLabel.setFillColor(sf::Color(255, 150, 100));
        holdLabel.setSize(sf::Vector2f(80, 8));
        holdLabel.setPosition(sf::Vector2f(panelX + 10, panelY + 5));
        window.draw(holdLabel);
    }
    
    if (hasHeld) {
        PieceShape shape = getPieceShape(heldType);
        TextureType texType = getTextureType(heldType);
        
        float centerX = panelX + 50;
        float centerY = panelY + 40;
        float miniCellSize = 16.0f;
        
        for (int row = 0; row < shape.height; ++row) {
            for (int col = 0; col < shape.width; ++col) {
                if (shape.blocks[row][col]) {
                    float miniX = centerX - (shape.width * miniCellSize) / 2 + col * miniCellSize;
                    float miniY = centerY - (shape.height * miniCellSize) / 2 + row * miniCellSize;
                    drawCell(window, miniX, miniY, miniCellSize, shape.color, texType, textures, useTextures);
                }
            }
        }
    } else {
        sf::RectangleShape emptyIndicator;
        emptyIndicator.setFillColor(sf::Color(100, 100, 100, 150));
        emptyIndicator.setSize(sf::Vector2f(60, 30));
        emptyIndicator.setPosition(sf::Vector2f(panelX + 20, panelY + 25));
        window.draw(emptyIndicator);
    }
}

void drawCombo(sf::RenderWindow& window, int currentCombo, int lastMoveScore, const sf::Font& font, bool fontLoaded) {
    float panelX = 250;
    float panelY = GRID_OFFSET_Y + 200;
    
    sf::RectangleShape comboBg;
    comboBg.setFillColor(sf::Color(40, 20, 60, 220));
    comboBg.setOutlineColor(currentCombo > 0 ? sf::Color(255, 100, 255, 255) : sf::Color(120, 60, 140, 200));
    comboBg.setOutlineThickness(3);
    comboBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    comboBg.setSize(sf::Vector2f(140, 90));
    window.draw(comboBg);
    
    if (fontLoaded) {
        sf::Text comboLabel(font, "COMBO");
        comboLabel.setCharacterSize(20);
        comboLabel.setFillColor(sf::Color(255, 200, 255));
        comboLabel.setStyle(sf::Text::Bold);
        comboLabel.setPosition(sf::Vector2f(panelX + 20, panelY));
        window.draw(comboLabel);
        
        sf::Text comboValue(font, "x" + std::to_string(currentCombo));
        comboValue.setCharacterSize(32);
        comboValue.setFillColor(currentCombo > 0 ? sf::Color(255, 100, 255) : sf::Color(150, 100, 150));
        comboValue.setStyle(sf::Text::Bold);
        comboValue.setPosition(sf::Vector2f(panelX + 25, panelY + 35));
        window.draw(comboValue);
    }
    
    float scorePanelY = panelY + 105;
    
    sf::RectangleShape scoreBg;
    scoreBg.setFillColor(sf::Color(20, 40, 60, 220));
    scoreBg.setOutlineColor(sf::Color(100, 200, 255, 255));
    scoreBg.setOutlineThickness(3);
    scoreBg.setPosition(sf::Vector2f(panelX - 10, scorePanelY - 10));
    scoreBg.setSize(sf::Vector2f(140, 75));
    window.draw(scoreBg);
    
    if (fontLoaded) {
        sf::Text lastScoreLabel(font, "LAST MOVE");
        lastScoreLabel.setCharacterSize(14);
        lastScoreLabel.setFillColor(sf::Color(150, 220, 255));
        lastScoreLabel.setStyle(sf::Text::Bold);
        lastScoreLabel.setPosition(sf::Vector2f(panelX + 10, scorePanelY));
        window.draw(lastScoreLabel);
        
        sf::Text lastScoreValue(font, "+" + std::to_string(lastMoveScore));
        lastScoreValue.setCharacterSize(26);
        lastScoreValue.setFillColor(lastMoveScore > 0 ? sf::Color(100, 255, 150) : sf::Color(150, 150, 150));
        lastScoreValue.setStyle(sf::Text::Bold);
        lastScoreValue.setPosition(sf::Vector2f(panelX + 20, scorePanelY + 30));
        window.draw(lastScoreValue);
    }
}

void drawLevelInfo(sf::RenderWindow& window, int totalLinesCleared, int currentLevel, int totalScore, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, bool sprintMode, float sprintTime, int sprintTarget, float gameTime) {
    float panelX = 50;
    float panelY = GRID_OFFSET_Y;
    
    if (sprintMode) {


        sf::RectangleShape timerBg;
        timerBg.setFillColor(sf::Color(30, 30, 40, 200));
        timerBg.setOutlineColor(sf::Color(255, 200, 100, 255));
        timerBg.setOutlineThickness(2);
        timerBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
        timerBg.setSize(sf::Vector2f(160, 90));
        window.draw(timerBg);
        
        if (fontLoaded) {
            sf::Text timerLabel(font, "TIME");
            timerLabel.setCharacterSize(14);
            timerLabel.setFillColor(sf::Color::Yellow);
            timerLabel.setStyle(sf::Text::Bold);
            timerLabel.setPosition(sf::Vector2f(panelX + 5, panelY));
            window.draw(timerLabel);
            

            int minutes = static_cast<int>(sprintTime) / 60;
            int seconds = static_cast<int>(sprintTime) % 60;
            int milliseconds = static_cast<int>((sprintTime - static_cast<int>(sprintTime)) * 100);
            std::string timeStr = std::to_string(minutes) + ":" + 
                                 (seconds < 10 ? "0" : "") + std::to_string(seconds) + "." +
                                 (milliseconds < 10 ? "0" : "") + std::to_string(milliseconds);
            
            sf::Text timeValue(font, timeStr);
            timeValue.setCharacterSize(24);
            timeValue.setFillColor(sf::Color::White);
            timeValue.setPosition(sf::Vector2f(panelX + 10, panelY + 25));
            window.draw(timeValue);
            

            sf::Text linesLabel(font, "LINES");
            linesLabel.setCharacterSize(14);
            linesLabel.setFillColor(sf::Color::Green);
            linesLabel.setStyle(sf::Text::Bold);
            linesLabel.setPosition(sf::Vector2f(panelX + 5, panelY + 60));
            window.draw(linesLabel);
            
            sf::Text linesValue(font, std::to_string(totalLinesCleared) + " / " + std::to_string(sprintTarget));
            linesValue.setCharacterSize(18);
            linesValue.setFillColor(sf::Color::White);
            linesValue.setPosition(sf::Vector2f(panelX + 60, panelY + 58));
            window.draw(linesValue);
        }
        

        float barX = GRID_OFFSET_X - GRID_WIDTH * CELL_SIZE + 200;
        float barY = panelY - 75;
        float barWidth = 75;
        float barHeight = 750;
        

        sf::RectangleShape progressBg;
        progressBg.setFillColor(sf::Color(20, 20, 30, 200));
        progressBg.setOutlineColor(sf::Color(100, 150, 255, 255));
        progressBg.setOutlineThickness(2);
        progressBg.setPosition(sf::Vector2f(barX, barY));
        progressBg.setSize(sf::Vector2f(barWidth, barHeight));
        window.draw(progressBg);
        

        float progressPercent = std::min(1.0f, static_cast<float>(totalLinesCleared) / static_cast<float>(sprintTarget));
        float fillHeight = barHeight * progressPercent;
        
        sf::RectangleShape progressFill;
        sf::Color fillColor;
        if (progressPercent < 0.33f) fillColor = sf::Color(255, 100, 100);
        else if (progressPercent < 0.66f) fillColor = sf::Color(255, 200, 100);
        else fillColor = sf::Color(100, 255, 100);
        
        progressFill.setFillColor(fillColor);
        progressFill.setPosition(sf::Vector2f(barX + 2, barY + barHeight - fillHeight - 2));
        progressFill.setSize(sf::Vector2f(barWidth - 4, fillHeight));
        window.draw(progressFill);
        

        if (fontLoaded) {
            sf::Text percentText(font, std::to_string(static_cast<int>(progressPercent * 100)) + "%");
            percentText.setCharacterSize(20);
            percentText.setFillColor(sf::Color::White);
            percentText.setStyle(sf::Text::Bold);
            sf::FloatRect percentBounds = percentText.getLocalBounds();
            percentText.setPosition(sf::Vector2f(barX + barWidth/2 - percentBounds.size.x/2, barY + barHeight/2 - 15));
            window.draw(percentText);
        }
    } else {

        sf::RectangleShape bg;
        bg.setFillColor(sf::Color(30, 30, 40, 200));
        bg.setOutlineColor(sf::Color(255, 200, 100, 255));
        bg.setOutlineThickness(2);
        bg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
        bg.setSize(sf::Vector2f(140, 170));
        window.draw(bg);
        
        if (fontLoaded) {

            sf::Text timeLabel(font, "TIME");
            timeLabel.setCharacterSize(14);
            timeLabel.setFillColor(sf::Color(200, 150, 255));
            timeLabel.setStyle(sf::Text::Bold);
            timeLabel.setPosition(sf::Vector2f(panelX + 5, panelY));
            window.draw(timeLabel);
            
            int minutes = static_cast<int>(gameTime) / 60;
            int seconds = static_cast<int>(gameTime) % 60;
            std::string timeStr = std::to_string(minutes) + ":" + 
                                 (seconds < 10 ? "0" : "") + std::to_string(seconds);
            
            sf::Text timeValue(font, timeStr);
            timeValue.setCharacterSize(18);
            timeValue.setFillColor(sf::Color::White);
            timeValue.setPosition(sf::Vector2f(panelX + 55, panelY));
            window.draw(timeValue);
            

            sf::Text scoreLabel(font, "SCORE");
            scoreLabel.setCharacterSize(14);
            scoreLabel.setFillColor(sf::Color::Yellow);
            scoreLabel.setStyle(sf::Text::Bold);
            scoreLabel.setPosition(sf::Vector2f(panelX + 5, panelY + 40));
            window.draw(scoreLabel);
            
            sf::Text scoreValue(font, std::to_string(totalScore));
            scoreValue.setCharacterSize(18);
            scoreValue.setFillColor(sf::Color::White);
            scoreValue.setPosition(sf::Vector2f(panelX + 45, panelY + 40));
            window.draw(scoreValue);
            

            sf::Text linesLabel(font, "LINES");
            linesLabel.setCharacterSize(14);
            linesLabel.setFillColor(sf::Color::Green);
            linesLabel.setStyle(sf::Text::Bold);
            linesLabel.setPosition(sf::Vector2f(panelX + 5, panelY + 80));
            window.draw(linesLabel);
            
            sf::Text linesValue(font, std::to_string(totalLinesCleared));
            linesValue.setCharacterSize(18);
            linesValue.setFillColor(sf::Color::White);
            linesValue.setPosition(sf::Vector2f(panelX + 45, panelY + 80));
            window.draw(linesValue);
            

            sf::Text levelLabel(font, "LEVEL");
            levelLabel.setCharacterSize(14);
            levelLabel.setFillColor(sf::Color::Cyan);
            levelLabel.setStyle(sf::Text::Bold);
            levelLabel.setPosition(sf::Vector2f(panelX + 5, panelY + 120));
            window.draw(levelLabel);
            
            sf::Text levelValue(font, std::to_string(currentLevel));
            levelValue.setCharacterSize(18);
            levelValue.setFillColor(sf::Color::White);
            levelValue.setPosition(sf::Vector2f(panelX + 45, panelY + 120));
            window.draw(levelValue);
        }
    }
}

void drawNextPieces(sf::RenderWindow& window, const std::vector<PieceType>& nextQueue, const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    float panelX = GRID_OFFSET_X + GRID_WIDTH * CELL_SIZE + 50;
    float panelY = GRID_OFFSET_Y + 50;
    

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(30, 30, 40, 200));
    panelBg.setOutlineColor(sf::Color(100, 150, 255, 255));
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(120, 200));
    window.draw(panelBg);
    

    sf::Font font;

    
    for (int i = 0; i < nextQueue.size(); ++i) {
        PieceShape shape = getPieceShape(nextQueue[i]);
        TextureType texType = getTextureType(nextQueue[i]);
        
        float startY = panelY + i * 60;
        float centerX = panelX + 50;
        float centerY = startY + 25;
        

        for (int row = 0; row < shape.height; ++row) {
            for (int col = 0; col < shape.width; ++col) {
                if (shape.blocks[row][col]) {
                    float miniCellSize = 16.0f;
                    float miniX = centerX - (shape.width * miniCellSize) / 2 + col * miniCellSize;
                    float miniY = centerY - (shape.height * miniCellSize) / 2 + row * miniCellSize;
                    drawCell(window, miniX, miniY, miniCellSize, shape.color, texType, textures, useTextures);
                }
            }
        }
    }
}

void drawGameModeLabel(sf::RenderWindow& window, const std::string& modeText, const sf::Font& font, bool fontLoaded) {
    if (!fontLoaded) return;
    
    sf::Text gameModeLabel(font);
    gameModeLabel.setString(modeText);
    gameModeLabel.setCharacterSize(32);
    gameModeLabel.setFillColor(sf::Color(200, 200, 220, 255));
    gameModeLabel.setStyle(sf::Text::Bold);
    
    sf::FloatRect textBounds = gameModeLabel.getLocalBounds();
    gameModeLabel.setPosition(sf::Vector2f(
        (1920.0f - textBounds.size.x) / 2.0f,
        20.0f
    ));
    window.draw(gameModeLabel);
}

void drawVolumeIndicator(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, float volume, bool isMuted) {
    if (!fontLoaded) return;
    
    std::string volumeStr = "Volume: " + std::to_string(static_cast<int>(volume)) + "%";
    
    sf::Text volumeText(font);
    volumeText.setString(volumeStr);
    volumeText.setCharacterSize(32);
    volumeText.setFillColor(sf::Color(255, 255, 255, 255));
    volumeText.setOutlineColor(sf::Color::Black);
    volumeText.setOutlineThickness(2);
    volumeText.setStyle(sf::Text::Bold);
    
    sf::FloatRect textBounds = volumeText.getLocalBounds();
    float textWidth = textBounds.size.x;
    

    float barWidth = 200.0f;
    float barHeight = 10.0f;
    

    float boxWidth = std::max(textWidth + 20.0f, barWidth + 20.0f);
    float boxHeight = 55.0f;
    
    float posX = 1920.0f - boxWidth - 20.0f;
    float posY = isMuted ? 80.0f : 20.0f;
    

    sf::RectangleShape volumeBg;
    volumeBg.setSize(sf::Vector2f(boxWidth, boxHeight));
    volumeBg.setPosition(sf::Vector2f(posX, posY));
    volumeBg.setFillColor(sf::Color(0, 0, 0, 180));
    volumeBg.setOutlineColor(sf::Color(100, 150, 255, 255));
    volumeBg.setOutlineThickness(2);
    window.draw(volumeBg);
    

    volumeText.setPosition(sf::Vector2f(posX + (boxWidth - textWidth) / 2.0f, posY + 5.0f));
    window.draw(volumeText);
    

    float barPosX = posX + (boxWidth - barWidth) / 2.0f;
    float barPosY = posY + 38.0f;
    
    sf::RectangleShape volumeBarBg;
    volumeBarBg.setSize(sf::Vector2f(barWidth, barHeight));
    volumeBarBg.setPosition(sf::Vector2f(barPosX, barPosY));
    volumeBarBg.setFillColor(sf::Color(50, 50, 50, 200));
    volumeBarBg.setOutlineColor(sf::Color(100, 100, 100, 255));
    volumeBarBg.setOutlineThickness(1);
    window.draw(volumeBarBg);
    
    float fillWidth = (volume / 100.0f) * barWidth;
    sf::RectangleShape volumeBarFill;
    volumeBarFill.setSize(sf::Vector2f(fillWidth, barHeight));
    volumeBarFill.setPosition(sf::Vector2f(barPosX, barPosY));
    
    sf::Color barColor;
    if (volume >= 70.0f) {
        barColor = sf::Color(100, 255, 100);
    } else if (volume >= 30.0f) {
        barColor = sf::Color(255, 255, 100);
    } else {
        barColor = sf::Color(255, 100, 100);
    }
    volumeBarFill.setFillColor(barColor);
    window.draw(volumeBarFill);
}

void drawMuteIcon(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, const sf::Font& font, bool fontLoaded) {
    if (textures.find(TextureType::MuteIcon) != textures.end()) {
        sf::Sprite muteSprite(textures.at(TextureType::MuteIcon));
        sf::Vector2u textureSize = textures.at(TextureType::MuteIcon).getSize();
        float iconSize = 48.0f;
        float scale = iconSize / textureSize.x;
        muteSprite.setScale(sf::Vector2f(scale, scale));
        muteSprite.setPosition(sf::Vector2f(1920.0f - iconSize - 20.0f, 20.0f));
        window.draw(muteSprite);
    } else if (fontLoaded) {
        sf::Text muteText(font);
        muteText.setString("MUTED");
        muteText.setCharacterSize(20);
        muteText.setFillColor(sf::Color::Red);
        muteText.setPosition(sf::Vector2f(1920.0f - 100.0f, 20.0f));
        window.draw(muteText);
    }
}

void drawDebugMode(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded) {
    if (!fontLoaded) return;
    
    sf::Text debugText(font);
    debugText.setString("DEBUG MODE");
    debugText.setCharacterSize(24);
    debugText.setFillColor(sf::Color::Yellow);
    debugText.setPosition(sf::Vector2f(1920.0f - 180.0f, 1080.0f - 40.0f));
    window.draw(debugText);
}

void drawAchievementPopups(sf::RenderWindow& window, const std::vector<AchievementPopup>& popups, const sf::Font& font, bool fontLoaded) {
    if (!fontLoaded) return;
    
    float popupY = 1080.0f - 140.0f;
    for (const auto& popup : popups) {
        float offsetX = popup.getOffsetX(1920.0f);
        

        const float popupWidth = 400.0f;
        const float popupHeight = 120.0f;
        sf::RectangleShape bg;
        bg.setSize(sf::Vector2f(popupWidth, popupHeight));
        bg.setPosition(sf::Vector2f(offsetX, popupY));
        bg.setFillColor(sf::Color(20, 20, 40, 230));
        bg.setOutlineColor(sf::Color(255, 215, 0, 255));
        bg.setOutlineThickness(3);
        window.draw(bg);
        

        sf::RectangleShape icon;
        icon.setSize(sf::Vector2f(80.0f, 80.0f));
        icon.setPosition(sf::Vector2f(offsetX + 15.0f, popupY + 20.0f));
        icon.setFillColor(sf::Color(60, 60, 80, 255));
        icon.setOutlineColor(sf::Color(255, 215, 0, 255));
        icon.setOutlineThickness(2);
        window.draw(icon);
        

        sf::Text questionMark(font);
        questionMark.setString("?");
        questionMark.setCharacterSize(60);
        questionMark.setFillColor(sf::Color(255, 215, 0, 255));
        questionMark.setStyle(sf::Text::Bold);
        sf::FloatRect qBounds = questionMark.getLocalBounds();
        questionMark.setPosition(sf::Vector2f(
            offsetX + 15.0f + (80.0f - qBounds.size.x) / 2.0f,
            popupY + 20.0f + (80.0f - qBounds.size.y) / 2.0f - 5.0f
        ));
        window.draw(questionMark);
        

        sf::Text headerText(font);
        headerText.setString("Achievement Unlocked!");
        headerText.setCharacterSize(18);
        headerText.setFillColor(sf::Color(200, 200, 200, 255));
        headerText.setStyle(sf::Text::Bold);
        headerText.setPosition(sf::Vector2f(offsetX + 110.0f, popupY + 15.0f));
        window.draw(headerText);
        

        sf::Text titleText(font);
        titleText.setString(popup.title);
        titleText.setCharacterSize(24);
        titleText.setFillColor(sf::Color(255, 215, 0, 255));
        titleText.setStyle(sf::Text::Bold);
        titleText.setPosition(sf::Vector2f(offsetX + 110.0f, popupY + 45.0f));
        window.draw(titleText);
    }
}

void drawCustomCursor(sf::RenderWindow& window, const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    sf::Vector2i cursorPixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f cursorPos = window.mapPixelToCoords(cursorPixelPos);
    float cursorSize = 24.0f;
    
    if (useTextures && textures.find(TextureType::GenericBlock) != textures.end()) {
        sf::Sprite cursorSprite(textures.at(TextureType::GenericBlock));
        sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
        float scale = cursorSize / textureSize.x;
        cursorSprite.setScale(sf::Vector2f(scale, scale));
        cursorSprite.setPosition(sf::Vector2f(cursorPos.x - cursorSize/2, cursorPos.y - cursorSize/2));
        cursorSprite.setColor(sf::Color(255, 255, 255, 255));
        window.draw(cursorSprite);
    } else {

        sf::CircleShape cursorShape(8.0f);
        cursorShape.setFillColor(sf::Color::White);
        cursorShape.setOutlineColor(sf::Color::Black);
        cursorShape.setOutlineThickness(2.0f);
        cursorShape.setPosition(sf::Vector2f(cursorPos.x - 8.0f, cursorPos.y - 8.0f));
        window.draw(cursorShape);
    }
}
