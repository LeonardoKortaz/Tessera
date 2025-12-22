#include "game_ui.h"
#include "piece_utils.h"
#include <iostream>

void drawGridBackground(sf::RenderWindow& window, const sf::Color& backgroundColor) {

    sf::Vector2u windowSize = window.getSize();
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    background.setPosition(sf::Vector2f(0, 0));
    background.setFillColor(backgroundColor);
    window.draw(background);
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

void drawGridBorder(sf::RenderWindow& window, const sf::Color& borderColor) {
    sf::RectangleShape border;
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(borderColor);
    border.setOutlineThickness(BORDER_WIDTH);
    float borderX = GRID_OFFSET_X - BORDER_WIDTH;
    float borderY = GRID_OFFSET_Y - BORDER_WIDTH;
    float borderW = GRID_WIDTH * CELL_SIZE + (2 * BORDER_WIDTH);
    float borderH = GRID_HEIGHT * CELL_SIZE + (2 * BORDER_WIDTH);
    border.setPosition(sf::Vector2f(borderX, borderY));
    border.setSize(sf::Vector2f(borderW, borderH));
    window.draw(border);
}

void drawBombAbility(sf::RenderWindow& window, bool isAvailable, int linesSinceLastAbility, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, bool infiniteBombs, const sf::Color& frameColor, AbilityChoice selectedAbility, int linesRequired) {

    static sf::Shader grayscaleShader;
    static bool shaderLoaded = false;
    static bool shaderLoadAttempted = false;
    
    if (!shaderLoadAttempted) {
        shaderLoadAttempted = true;
        if (sf::Shader::isAvailable()) {
            shaderLoaded = grayscaleShader.loadFromFile("Assets/Shaders/grayscale.frag", sf::Shader::Type::Fragment);
        }
    }
    

    float panelX = GRID_OFFSET_X - 250;
    float panelY = GRID_OFFSET_Y + 180;


    if (fontLoaded) {
        std::string abilityName;
        sf::Color abilityColor;
        
        switch(selectedAbility) {
            case AbilityChoice::Bomb:
                abilityName = "BOMB";
                abilityColor = sf::Color(255, 100, 100);
                break;
            case AbilityChoice::Delivery:
                abilityName = "DELIVERY";
                abilityColor = sf::Color(255, 200, 100);
                break;
            case AbilityChoice::Stomp:
                abilityName = "STOMP";
                abilityColor = sf::Color(160, 120, 90);
                break;
            default:
                abilityName = "ABILITY";
                abilityColor = sf::Color::White;
                break;
        }
        
        sf::Text bombTitle(font, abilityName);
        bombTitle.setCharacterSize(32);
        bombTitle.setFillColor((isAvailable || infiniteBombs) ? abilityColor : sf::Color::White);
        bombTitle.setStyle(sf::Text::Bold);
        bombTitle.setPosition(sf::Vector2f(panelX, panelY - 40));
        window.draw(bombTitle);
    }

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(30, 35, 50, 220));
    if (isAvailable || infiniteBombs) {
        panelBg.setOutlineColor(sf::Color(255, 100, 100, 255));
    } else {
        panelBg.setOutlineColor(frameColor);
    }
    panelBg.setOutlineThickness(3);
    panelBg.setPosition(sf::Vector2f(panelX, panelY));
    panelBg.setSize(sf::Vector2f(100, 110));
    window.draw(panelBg);
    

    PieceShape bombShape = getPieceShape(PieceType::A_Bomb);
    TextureType texType = TextureType::A_Bomb;
    
    float centerX = panelX + 50;
    float centerY = panelY + 35;
    float miniCellSize = 48.0f;
    
    if (useTextures && textures.find(texType) != textures.end()) {
        sf::Sprite miniSprite(textures.at(texType));
        miniSprite.setPosition(sf::Vector2f(centerX - miniCellSize/2, centerY - miniCellSize/2));
        sf::Vector2u textureSize = textures.at(texType).getSize();
        miniSprite.setScale(sf::Vector2f(miniCellSize / textureSize.x, miniCellSize / textureSize.y));
        
        if (isAvailable || infiniteBombs) {

            window.draw(miniSprite);
        } else {

            if (shaderLoaded) {
                grayscaleShader.setUniform("texture", sf::Shader::CurrentTexture);
                grayscaleShader.setUniform("saturation", 0.0f);
                window.draw(miniSprite, &grayscaleShader);
            } else {

                miniSprite.setColor(sf::Color(180, 180, 180, 255));
                window.draw(miniSprite);
            }
        }
    } else {
        sf::RectangleShape miniCell;
        miniCell.setSize(sf::Vector2f(miniCellSize, miniCellSize));
        miniCell.setPosition(sf::Vector2f(centerX - miniCellSize/2, centerY - miniCellSize/2));
        if (isAvailable || infiniteBombs) {
            miniCell.setFillColor(bombShape.color);
        } else {

            miniCell.setFillColor(sf::Color(128, 128, 128, 255));
        }
        window.draw(miniCell);
    }
    

    float barStartX = panelX + 5;
    float barY = panelY + 75;
    

    const float TOTAL_BAR_WIDTH = 90.0f;
    float barSpacing = 1.0f;
    float totalSpacingWidth = (linesRequired - 1) * barSpacing;
    float barWidth = (TOTAL_BAR_WIDTH - totalSpacingWidth) / linesRequired;
    float barHeight = 20.0f;
    
    for (int i = 0; i < linesRequired; ++i) {
        sf::RectangleShape segment;
        segment.setSize(sf::Vector2f(barWidth, barHeight));
        segment.setPosition(sf::Vector2f(barStartX + i * (barWidth + barSpacing), barY));
        
        if (infiniteBombs) {

            segment.setFillColor(sf::Color(255, 215, 0));
        } else if (i < linesSinceLastAbility) {

            float t = static_cast<float>(i) / (linesRequired - 1.0f);
            sf::Color fillColor(
                static_cast<std::uint8_t>(100 + 155 * t),
                static_cast<std::uint8_t>(255 - 155 * t),
                static_cast<std::uint8_t>(100)
            );
            segment.setFillColor(fillColor);
        } else {

            segment.setFillColor(sf::Color(50, 50, 60, 180));
        }
        
        segment.setOutlineColor(sf::Color(80, 80, 100, 150));
        segment.setOutlineThickness(1);
        window.draw(segment);
    }
}

void drawHeldPiece(sf::RenderWindow& window, PieceType heldType, bool hasHeld, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, const sf::Color& frameColor) {

    float panelX = GRID_OFFSET_X - 250;
    float panelY = GRID_OFFSET_Y + 40;


    if (fontLoaded) {
        sf::Text holdTitle(font, "HOLD");
        holdTitle.setCharacterSize(32);
        holdTitle.setFillColor(sf::Color::White);
        holdTitle.setStyle(sf::Text::Bold);
        holdTitle.setPosition(sf::Vector2f(panelX, GRID_OFFSET_Y));
        window.draw(holdTitle);
    }

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(20, 25, 40, 220));
    panelBg.setOutlineColor(frameColor);
    panelBg.setOutlineThickness(3);
    panelBg.setPosition(sf::Vector2f(panelX, panelY));
    panelBg.setSize(sf::Vector2f(100, 90));
    window.draw(panelBg);
    
    
    if (hasHeld) {
        PieceShape shape = getPieceShape(heldType);
        TextureType texType = getTextureType(heldType);
        
        float centerX = panelX + 50;
        float centerY = panelY + 50;
        float miniCellSize = 21.0f;
        
        for (int row = 0; row < shape.height; ++row) {
            for (int col = 0; col < shape.width; ++col) {
                if (shape.blocks[row][col]) {
                    float miniX = centerX - (shape.width * miniCellSize) / 2 + col * miniCellSize;
                    float miniY = centerY - (shape.height * miniCellSize) / 2 + row * miniCellSize;
                    drawCell(window, miniX, miniY, miniCellSize, shape.color, texType, textures, useTextures);
                }
            }
        }
    }
}

void drawCombo(sf::RenderWindow& window, float displayCombo, int targetCombo, int lastMoveScore, const sf::Font& font, bool fontLoaded, bool showScore, float fadeScale) {

    if ((displayCombo <= 0.0f && targetCombo <= 0) || !fontLoaded || fadeScale <= 0.0f) return;
    
    float comboToShow = displayCombo > 0.0f ? displayCombo : static_cast<float>(targetCombo);
    int comboToDisplay = static_cast<int>(std::round(comboToShow));
    

    float posX = GRID_OFFSET_X + GRID_WIDTH * CELL_SIZE + 50;
    float posY = GRID_OFFSET_Y + GRID_HEIGHT * CELL_SIZE - 140;
    

    std::uint8_t alpha = static_cast<std::uint8_t>(255 * fadeScale);
    sf::Color textColor(255, 255, 255, alpha);
    

    sf::Text comboLabel(font, "COMBO");
    comboLabel.setCharacterSize(36);
    comboLabel.setFillColor(textColor);
    comboLabel.setStyle(sf::Text::Bold);
    comboLabel.setScale(sf::Vector2f(fadeScale, fadeScale));
    comboLabel.setPosition(sf::Vector2f(posX, posY));
    window.draw(comboLabel);
    

    float cappedCombo = std::min(comboToShow, 30.0f);
    float baseFontSize = 48.0f + cappedCombo * 2.0f;
    float scaleFactor = (baseFontSize / 48.0f) * fadeScale;
    

    std::string comboStr = "x" + std::to_string(comboToDisplay);
    if (comboToDisplay >= 12) {
        comboStr += "!!!";
    } else if (comboToDisplay >= 8) {
        comboStr += "!";
    }
    
    sf::Text comboValue(font, comboStr);
    comboValue.setCharacterSize(48);
    comboValue.setFillColor(textColor);
    comboValue.setStyle(sf::Text::Bold);
    comboValue.setScale(sf::Vector2f(scaleFactor, scaleFactor));
    comboValue.setPosition(sf::Vector2f(posX, posY + 40));
    window.draw(comboValue);
    

    if (showScore && lastMoveScore > 0) {
        sf::Text scoreText(font, "+" + std::to_string(lastMoveScore));
        scoreText.setCharacterSize(40);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setStyle(sf::Text::Bold);
        scoreText.setPosition(sf::Vector2f(posX, posY + 110));
        window.draw(scoreText);
    }
}

void drawLevelInfo(sf::RenderWindow& window, int totalLinesCleared, int currentLevel, int totalScore, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, bool sprintMode, float sprintTime, int sprintTarget, float gameTime, float displayedProgress, const sf::Color& frameColor) {

    float panelX = GRID_OFFSET_X - 250;
    float panelY = GRID_OFFSET_Y + 270;
    
    if (sprintMode) {

        

        float barX = GRID_OFFSET_X - 95;
        float barY = GRID_OFFSET_Y;
        float barWidth = 50;
        float barHeight = GRID_HEIGHT * CELL_SIZE;
        

        sf::RectangleShape progressBg;
        progressBg.setFillColor(sf::Color(20, 20, 30, 200));
        progressBg.setOutlineColor(frameColor);
        progressBg.setOutlineThickness(2);
        progressBg.setPosition(sf::Vector2f(barX, barY));
        progressBg.setSize(sf::Vector2f(barWidth, barHeight));
        window.draw(progressBg);
        

        float progressPercent;
        if (displayedProgress >= 0.0f) {
            progressPercent = std::min(1.0f, displayedProgress);
        } else {
            progressPercent = std::min(1.0f, static_cast<float>(totalLinesCleared) / static_cast<float>(sprintTarget));
        }
        float fillHeight = barHeight * progressPercent;
        

        sf::RectangleShape progressFill;
        progressFill.setFillColor(sf::Color(255, 255, 255));
        progressFill.setPosition(sf::Vector2f(barX + 2, barY + barHeight - fillHeight - 2));
        progressFill.setSize(sf::Vector2f(barWidth - 4, fillHeight));
        window.draw(progressFill);
        

        if (fontLoaded) {

            sf::Text percentText(font, std::to_string(static_cast<int>(progressPercent * 100)) + "%");
            percentText.setCharacterSize(18);
            percentText.setFillColor(sf::Color::White);
            percentText.setStyle(sf::Text::Bold);
            sf::FloatRect percentBounds = percentText.getLocalBounds();
            percentText.setPosition(sf::Vector2f(barX + barWidth/2 - percentBounds.size.x/2, barY + barHeight/2 - 22));
            window.draw(percentText);
            

            std::string linesStr = std::to_string(totalLinesCleared) + "/" + std::to_string(sprintTarget);
            sf::Text linesText(font, linesStr);
            linesText.setCharacterSize(14);
            linesText.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect linesBounds = linesText.getLocalBounds();
            linesText.setPosition(sf::Vector2f(barX + barWidth/2 - linesBounds.size.x/2, barY + barHeight/2 + 2));
            window.draw(linesText);
        }
    } else {


        float posX = GRID_OFFSET_X - 300;
        float posY = GRID_OFFSET_Y + GRID_HEIGHT * CELL_SIZE - 100;
        
        if (fontLoaded) {

            sf::Text levelText(font, "LEVEL " + std::to_string(currentLevel));
            levelText.setCharacterSize(48);
            levelText.setFillColor(sf::Color::White);
            levelText.setStyle(sf::Text::Bold);
            levelText.setPosition(sf::Vector2f(posX, posY));
            window.draw(levelText);
            

            sf::Text linesText(font, std::to_string(totalLinesCleared) + " LINES");
            linesText.setCharacterSize(28);
            linesText.setFillColor(sf::Color::White);
            linesText.setStyle(sf::Text::Bold);
            linesText.setPosition(sf::Vector2f(posX, posY + 55));
            window.draw(linesText);
        }
    }
}

void drawNextPieces(sf::RenderWindow& window, const std::vector<PieceType>& nextQueue, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Color& frameColor, const sf::Font& font, bool fontLoaded) {

    float panelX = GRID_OFFSET_X + GRID_WIDTH * CELL_SIZE + 50;
    float panelY = GRID_OFFSET_Y + 40;
    

    if (fontLoaded) {
        sf::Text nextTitle(font, "NEXT");
        nextTitle.setCharacterSize(32);
        nextTitle.setFillColor(sf::Color::White);
        nextTitle.setStyle(sf::Text::Bold);
        nextTitle.setPosition(sf::Vector2f(panelX, GRID_OFFSET_Y));
        window.draw(nextTitle);
    }
    
    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(60, 60, 60, 180));
    panelBg.setOutlineColor(frameColor);
    panelBg.setOutlineThickness(3);
    panelBg.setPosition(sf::Vector2f(panelX, panelY));
    panelBg.setSize(sf::Vector2f(110, 260));
    window.draw(panelBg);
    

    int piecesToDraw = std::min(3, static_cast<int>(nextQueue.size()));
    for (int i = 0; i < piecesToDraw; ++i) {
        PieceShape shape = getPieceShape(nextQueue[i]);
        TextureType texType = getTextureType(nextQueue[i]);
        
        float startY = panelY + 10 + i * 85;
        float centerX = panelX + 55;
        float centerY = startY + 35;
        
        for (int row = 0; row < shape.height; ++row) {
            for (int col = 0; col < shape.width; ++col) {
                if (shape.blocks[row][col]) {
                    float miniCellSize = 21.0f;
                    float miniX = centerX - (shape.width * miniCellSize) / 2 + col * miniCellSize;
                    float miniY = centerY - (shape.height * miniCellSize) / 2 + row * miniCellSize;
                    drawCell(window, miniX, miniY, miniCellSize, shape.color, texType, textures, useTextures);
                }
            }
        }
    }
}

void drawGameModeLabel(sf::RenderWindow& window, const std::string& modeText, const sf::Font& font, bool fontLoaded,
                       bool isRaceMode, float timeValue, int scoreValue, float bestTime, int highScore) {
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
    

    if (isRaceMode) {

        int minutes = static_cast<int>(timeValue) / 60;
        int seconds = static_cast<int>(timeValue) % 60;
        std::string timeStr = std::to_string(minutes) + ":" + 
                             (seconds < 10 ? "0" : "") + std::to_string(seconds);
        
        sf::Text timeLabel(font);
        timeLabel.setString(timeStr);
        timeLabel.setCharacterSize(28);
        timeLabel.setFillColor(sf::Color(255, 220, 100, 255));
        timeLabel.setStyle(sf::Text::Bold);
        
        sf::FloatRect timeBounds = timeLabel.getLocalBounds();
        timeLabel.setPosition(sf::Vector2f(
            (1920.0f - timeBounds.size.x) / 2.0f,
            55.0f
        ));
        window.draw(timeLabel);
        

        if (bestTime > 0.0f) {
            int bestMin = static_cast<int>(bestTime) / 60;
            int bestSec = static_cast<int>(bestTime) % 60;
            int bestMs = static_cast<int>((bestTime - static_cast<int>(bestTime)) * 100);
            std::string bestStr = "BEST: " + std::to_string(bestMin) + ":" + 
                                 (bestSec < 10 ? "0" : "") + std::to_string(bestSec) + "." +
                                 (bestMs < 10 ? "0" : "") + std::to_string(bestMs);
            
            sf::Text bestLabel(font);
            bestLabel.setString(bestStr);
            bestLabel.setCharacterSize(18);
            bestLabel.setFillColor(sf::Color(150, 150, 180, 255));
            
            sf::FloatRect bestBounds = bestLabel.getLocalBounds();
            bestLabel.setPosition(sf::Vector2f(
                (1920.0f - bestBounds.size.x) / 2.0f,
                85.0f
            ));
            window.draw(bestLabel);
        }
    } else {

        sf::Text scoreLabel(font);
        scoreLabel.setString(std::to_string(scoreValue));
        scoreLabel.setCharacterSize(28);
        scoreLabel.setFillColor(sf::Color(255, 255, 100, 255));
        scoreLabel.setStyle(sf::Text::Bold);
        
        sf::FloatRect scoreBounds = scoreLabel.getLocalBounds();
        scoreLabel.setPosition(sf::Vector2f(
            (1920.0f - scoreBounds.size.x) / 2.0f,
            55.0f
        ));
        window.draw(scoreLabel);
        

        if (highScore > 0) {
            std::string highStr = "HIGH: " + std::to_string(highScore);
            
            sf::Text highLabel(font);
            highLabel.setString(highStr);
            highLabel.setCharacterSize(18);
            highLabel.setFillColor(sf::Color(150, 150, 180, 255));
            
            sf::FloatRect highBounds = highLabel.getLocalBounds();
            highLabel.setPosition(sf::Vector2f(
                (1920.0f - highBounds.size.x) / 2.0f,
                85.0f
            ));
            window.draw(highLabel);
        }
    }
}

void drawVolumeIndicator(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, float volume, bool isMuted) {
    if (!fontLoaded) return;
    
    std::string volumeStr;
    if (isMuted) {
        volumeStr = "Volume: MUTED";
    } else {
        volumeStr = "Volume: " + std::to_string(static_cast<int>(volume)) + "%";
    }
    
    sf::Text volumeText(font);
    volumeText.setString(volumeStr);
    volumeText.setCharacterSize(32);
    volumeText.setFillColor(isMuted ? sf::Color(255, 100, 100, 255) : sf::Color(255, 255, 255, 255));
    volumeText.setOutlineColor(sf::Color::Black);
    volumeText.setOutlineThickness(2);
    volumeText.setStyle(sf::Text::Bold);
    
    sf::FloatRect textBounds = volumeText.getLocalBounds();
    float textWidth = textBounds.size.x;
    

    float barWidth = 200.0f;
    float barHeight = 10.0f;
    

    float boxWidth = std::max(textWidth + 20.0f, barWidth + 20.0f);
    float boxHeight = isMuted ? 45.0f : 55.0f;
    
    float posX = 1920.0f - boxWidth - 20.0f;
    float posY = 20.0f;
    

    sf::RectangleShape volumeBg;
    volumeBg.setSize(sf::Vector2f(boxWidth, boxHeight));
    volumeBg.setPosition(sf::Vector2f(posX, posY));
    volumeBg.setFillColor(sf::Color(0, 0, 0, 180));
    volumeBg.setOutlineColor(isMuted ? sf::Color(255, 100, 100, 255) : sf::Color(100, 150, 255, 255));
    volumeBg.setOutlineThickness(2);
    window.draw(volumeBg);
    

    volumeText.setPosition(sf::Vector2f(posX + (boxWidth - textWidth) / 2.0f, posY + 5.0f));
    window.draw(volumeText);
    

    if (!isMuted) {
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

void drawThermometerParticles(sf::RenderWindow& window, const std::vector<ThermometerParticle>& particles) {
    for (const auto& particle : particles) {
        float size = particle.size;
        
        sf::RectangleShape particleShape(sf::Vector2f(size, size));
        

        float alpha = 255.0f * (1.0f - particle.progress * 0.7f);
        sf::Color particleColor(255, 255, 255, static_cast<uint8_t>(alpha));
        particleShape.setFillColor(particleColor);
        

        particleShape.setRotation(sf::degrees(particle.progress * 360.0f));
        particleShape.setOrigin(sf::Vector2f(size/2, size/2));
        particleShape.setPosition(sf::Vector2f(particle.x, particle.y));
        
        window.draw(particleShape);
        

        float coreSize = size * 0.6f;
        sf::RectangleShape coreShape(sf::Vector2f(coreSize, coreSize));
        coreShape.setOrigin(sf::Vector2f(coreSize/2, coreSize/2));
        coreShape.setPosition(sf::Vector2f(particle.x, particle.y));
        coreShape.setRotation(sf::degrees(particle.progress * 360.0f));
        coreShape.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
        window.draw(coreShape);
    }
}

void drawFallingCells(sf::RenderWindow& window, const std::vector<FallingCell>& cells,
                     const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    for (const auto& cell : cells) {
        float cellSize = CELL_SIZE;
        

        float alpha = std::min(1.0f, cell.timer) * 255.0f;
        sf::Color drawColor = cell.color;
        drawColor.a = static_cast<uint8_t>(alpha);
        
        if (useTextures && textures.count(cell.texType)) {
            sf::Sprite cellSprite(textures.at(cell.texType));
            cellSprite.setColor(drawColor);
            
            sf::Vector2u texSize = textures.at(cell.texType).getSize();
            float scaleX = cellSize / static_cast<float>(texSize.x);
            float scaleY = cellSize / static_cast<float>(texSize.y);
            cellSprite.setScale(sf::Vector2f(scaleX, scaleY));
            

            cellSprite.setOrigin(sf::Vector2f(texSize.x / 2.0f, texSize.y / 2.0f));
            cellSprite.setPosition(sf::Vector2f(cell.x + cellSize/2, cell.y + cellSize/2));
            cellSprite.setRotation(sf::degrees(cell.rotation * 57.2958f));
            
            window.draw(cellSprite);
        } else {
            sf::RectangleShape cellShape(sf::Vector2f(cellSize - 2, cellSize - 2));
            cellShape.setFillColor(drawColor);
            cellShape.setOutlineColor(sf::Color(drawColor.r/2, drawColor.g/2, drawColor.b/2, drawColor.a));
            cellShape.setOutlineThickness(1);
            
            cellShape.setOrigin(sf::Vector2f((cellSize-2)/2, (cellSize-2)/2));
            cellShape.setPosition(sf::Vector2f(cell.x + cellSize/2, cell.y + cellSize/2));
            cellShape.setRotation(sf::degrees(cell.rotation * 57.2958f));
            
            window.draw(cellShape);
        }
    }
}

void drawThemeSelector(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, 
                      GameThemeChoice currentTheme, GameThemeChoice hoveredTheme, 
                      bool isHovered) {
    float panelX = 1400;
    float panelY = 450;
    float panelWidth = 220;
    float themeHeight = 70;
    

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(20, 25, 35, 230));
    panelBg.setOutlineColor(sf::Color(100, 150, 255, 255));
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(panelWidth + 20, themeHeight * 3 + 40));
    window.draw(panelBg);
    

    sf::Text titleText(font, "THEMES");
    titleText.setCharacterSize(20);
    titleText.setFillColor(sf::Color(200, 220, 255));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(sf::Vector2f(panelX + 60, panelY));
    window.draw(titleText);
    

    std::vector<std::pair<GameThemeChoice, std::string>> themes = {
        {GameThemeChoice::Classic, "CLASSIC"},
        {GameThemeChoice::Forest, "FOREST"},
        {GameThemeChoice::Racer, "RACER"}
    };
    
    for (size_t i = 0; i < themes.size(); i++) {
        float yPos = panelY + 35 + i * themeHeight;
        GameThemeChoice theme = themes[i].first;
        bool isSelected = (theme == currentTheme);
        bool isHover = isHovered && (theme == hoveredTheme);
        

        sf::RectangleShape themeButton;
        themeButton.setSize(sf::Vector2f(panelWidth, themeHeight - 10));
        themeButton.setPosition(sf::Vector2f(panelX, yPos));
        
        if (isSelected) {
            themeButton.setFillColor(sf::Color(50, 100, 200, 200));
            themeButton.setOutlineColor(sf::Color(100, 200, 255, 255));
            themeButton.setOutlineThickness(3);
        } else if (isHover) {
            themeButton.setFillColor(sf::Color(40, 80, 160, 180));
            themeButton.setOutlineColor(sf::Color(80, 160, 220, 200));
            themeButton.setOutlineThickness(2);
        } else {
            themeButton.setFillColor(sf::Color(30, 40, 60, 150));
            themeButton.setOutlineColor(sf::Color(60, 80, 120, 150));
            themeButton.setOutlineThickness(1);
        }
        
        window.draw(themeButton);
        

        sf::Text themeName(font, themes[i].second);
        themeName.setCharacterSize(isSelected ? 24 : 20);
        themeName.setFillColor(isSelected ? sf::Color(255, 255, 255) : 
                              isHover ? sf::Color(220, 220, 255) : sf::Color(180, 180, 200));
        themeName.setStyle(isSelected ? sf::Text::Bold : sf::Text::Regular);
        

        sf::FloatRect textBounds = themeName.getLocalBounds();
        themeName.setPosition(sf::Vector2f(
            panelX + (panelWidth - textBounds.size.x) / 2,
            yPos + (themeHeight - 10 - textBounds.size.y) / 2 - 5
        ));
        
        window.draw(themeName);
        

        if (isSelected) {
            sf::CircleShape indicator(6);
            indicator.setFillColor(sf::Color(100, 255, 150));
            indicator.setPosition(sf::Vector2f(panelX + 15, yPos + themeHeight/2 - 11));
            window.draw(indicator);
        }
    }
}

void drawAbilitySelector(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded, 
                        AbilityChoice currentAbility, AbilityChoice hoveredAbility, 
                        bool isHovered) {
    float panelX = 300;
    float panelY = 450;
    float panelWidth = 220;
    float abilityHeight = 70;
    

    sf::RectangleShape panelBg;
    panelBg.setFillColor(sf::Color(20, 25, 35, 230));
    panelBg.setOutlineColor(sf::Color(255, 150, 100, 255));
    panelBg.setOutlineThickness(2);
    panelBg.setPosition(sf::Vector2f(panelX - 10, panelY - 10));
    panelBg.setSize(sf::Vector2f(panelWidth + 20, abilityHeight * 3 + 40));
    window.draw(panelBg);
    

    sf::Text titleText(font, "ABILITY");
    titleText.setCharacterSize(20);
    titleText.setFillColor(sf::Color(255, 220, 200));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(sf::Vector2f(panelX + 60, panelY));
    window.draw(titleText);
    

    std::vector<std::pair<AbilityChoice, std::string>> abilities = {
        {AbilityChoice::Bomb, "BOMB"},
        {AbilityChoice::Delivery, "DELIVERY"},
        {AbilityChoice::Stomp, "STOMP"}
    };
    
    for (size_t i = 0; i < abilities.size(); i++) {
        float yPos = panelY + 35 + i * abilityHeight;
        AbilityChoice ability = abilities[i].first;
        bool isSelected = (ability == currentAbility);
        bool isHover = isHovered && (ability == hoveredAbility);
        bool isLocked = false;
        

        sf::RectangleShape abilityButton;
        abilityButton.setSize(sf::Vector2f(panelWidth, abilityHeight - 10));
        abilityButton.setPosition(sf::Vector2f(panelX, yPos));
        
        if (isLocked) {
            abilityButton.setFillColor(sf::Color(40, 40, 40, 150));
            abilityButton.setOutlineColor(sf::Color(80, 80, 80, 150));
            abilityButton.setOutlineThickness(1);
        } else if (isSelected) {
            abilityButton.setFillColor(sf::Color(200, 100, 50, 200));
            abilityButton.setOutlineColor(sf::Color(255, 150, 100, 255));
            abilityButton.setOutlineThickness(3);
        } else if (isHover) {
            abilityButton.setFillColor(sf::Color(160, 80, 40, 180));
            abilityButton.setOutlineColor(sf::Color(220, 130, 80, 200));
            abilityButton.setOutlineThickness(2);
        } else {
            abilityButton.setFillColor(sf::Color(60, 40, 30, 150));
            abilityButton.setOutlineColor(sf::Color(120, 80, 60, 150));
            abilityButton.setOutlineThickness(1);
        }
        
        window.draw(abilityButton);
        

        sf::Text abilityName(font, abilities[i].second);
        abilityName.setCharacterSize(isLocked ? 18 : (isSelected ? 24 : 20));
        
        if (isLocked) {
            abilityName.setFillColor(sf::Color(100, 100, 100));
        } else {
            abilityName.setFillColor(isSelected ? sf::Color(255, 255, 255) : 
                                  isHover ? sf::Color(255, 220, 200) : sf::Color(200, 180, 160));
        }
        
        abilityName.setStyle(isSelected ? sf::Text::Bold : sf::Text::Regular);
        

        sf::FloatRect textBounds = abilityName.getLocalBounds();
        abilityName.setPosition(sf::Vector2f(
            panelX + (panelWidth - textBounds.size.x) / 2,
            yPos + (abilityHeight - 10 - textBounds.size.y) / 2 - 5
        ));
        
        window.draw(abilityName);
        

        if (isSelected && !isLocked) {
            sf::CircleShape indicator(6);
            indicator.setFillColor(sf::Color(255, 150, 100));
            indicator.setPosition(sf::Vector2f(panelX + 15, yPos + abilityHeight/2 - 11));
            window.draw(indicator);
        }
    }
}
