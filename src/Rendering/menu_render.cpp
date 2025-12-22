#include "menu_render.h"
#include "menu_config.h"
#include "piece_utils.h"
#include "achievements.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cmath>

const std::string GAME_VERSION = "v0.3.0-beta.10";

float drawButtonSprite(sf::RenderWindow& window, 
                       const std::map<TextureType, sf::Texture>& textures,
                       bool useTextures,
                       float centerX, float y,
                       float scale,
                       bool isActive) {
    if (useTextures && textures.count(TextureType::ButtonActive) && textures.count(TextureType::Button)) {
        TextureType btnType = isActive ? TextureType::ButtonActive : TextureType::Button;
        sf::Sprite btn(textures.at(btnType));
        sf::Vector2u ts = textures.at(btnType).getSize();
        float btnW = static_cast<float>(ts.x) * scale;
        float btnH = static_cast<float>(ts.y) * scale;
        btn.setScale(sf::Vector2f(scale, scale));
        btn.setPosition(sf::Vector2f(centerX - btnW/2, y));
        window.draw(btn);
        return btnH;
    }
    return 0.0f;
}


struct MenuOptionStyle {
    float buttonScale = 3.0f;
    float fallbackSelectorWidth = 500.0f;
    float fallbackSelectorHeight = 60.0f;
    float textSize = 86.0f;
    float textOffsetY = -45.0f;
    

    static MenuOptionStyle MainMenuStyle() {
        MenuOptionStyle style;
        style.textSize = 86.0f;
        style.fallbackSelectorHeight = 80.0f;
        return style;
    }
    
    static MenuOptionStyle SubMenuStyle() {
        MenuOptionStyle style;
        style.textSize = 86.0f;
        style.fallbackSelectorHeight = 80.0f;
        return style;
    }
    
    static MenuOptionStyle SmallMenuStyle() {
        MenuOptionStyle style;
        style.textSize = 86.0f;
        style.fallbackSelectorHeight = 80.0f;
        return style;
    }
};

void drawGameLogo(sf::RenderWindow& window, const sf::Font& titleFont, bool fontLoaded, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    if (useTextures && textures.find(TextureType::TesseraLogo) != textures.end()) {
        sf::Sprite logoSprite(textures.at(TextureType::TesseraLogo));
        sf::Vector2u logoSize = textures.at(TextureType::TesseraLogo).getSize();
        
        float logoScale = 0.45f;
        logoSprite.setScale(sf::Vector2f(logoScale, logoScale));
        
        float hueShift = std::fmod(elapsedTime / 5.0f, 1.0f);
        
        static sf::Shader hueShader;
        static bool shaderLoaded = false;
        if (!shaderLoaded) {
            if (hueShader.loadFromFile("Assets/Shaders/hue_shift.frag", sf::Shader::Type::Fragment)) {
                shaderLoaded = true;
            }
        }

        float scaledWidth = logoSize.x * logoScale;
        float scaledHeight = logoSize.y * logoScale;
        logoSprite.setPosition(sf::Vector2f(centerX - scaledWidth/2, centerY - 350));
        
        if (shaderLoaded && sf::Shader::isAvailable()) {
            hueShader.setUniform("texture", sf::Shader::CurrentTexture);
            hueShader.setUniform("hueShift", hueShift);
            window.draw(logoSprite, &hueShader);
        } else {
            window.draw(logoSprite);
        }
    } else if (fontLoaded) {
        sf::Text titleText(titleFont, "Tessera");
        titleText.setCharacterSize(128);
        titleText.setFillColor(sf::Color(100, 255, 150));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 350));
        window.draw(titleText);
    }
}

float drawMenuButton(sf::RenderWindow& window,
                     float centerX, float y,
                     bool isSelected,
                     const std::map<TextureType, sf::Texture>& textures,
                     bool useTextures,
                     const MenuOptionStyle& style = MenuOptionStyle(),
                     bool debugMode = false,
                     float width = 300.0f) {

    if (debugMode) {
        sf::RectangleShape hitbox;
        hitbox.setSize(sf::Vector2f(width, style.fallbackSelectorHeight));
        hitbox.setPosition(sf::Vector2f(centerX - width/2, y));
        hitbox.setFillColor(sf::Color(255, 0, 0, 50));
        hitbox.setOutlineColor(sf::Color(255, 0, 0, 200));
        hitbox.setOutlineThickness(2);
        window.draw(hitbox);
    }
    return style.fallbackSelectorHeight;
}


float calculateButtonWidth(const sf::Font& font, const std::string& text, float textSize) {
    sf::Text tempText(font, text);
    tempText.setCharacterSize(static_cast<unsigned int>(textSize));
    return tempText.getLocalBounds().size.x + 40;
}

void drawMenuOption(sf::RenderWindow& window,
                    const sf::Font& font,
                    const std::string& text,
                    float centerX, float y,
                    bool isSelected,
                    const std::map<TextureType, sf::Texture>& textures,
                    bool useTextures,
                    const MenuOptionStyle& style = MenuOptionStyle(),
                    sf::Shader* hueShader = nullptr,
                    float hueShift = 0.0f,
                    bool debugMode = false) {
    

    float textWidth = calculateButtonWidth(font, text, style.textSize);

    float btnH = drawMenuButton(window, centerX, y, isSelected, textures, useTextures, style, debugMode, textWidth);
    

    sf::Text optionText(font, text);
    optionText.setCharacterSize(static_cast<unsigned int>(style.textSize));
    
    if (isSelected) {

        float h = hueShift;
        float s = 1.0f;
        float v = 1.0f;
        

        float c = v * s;
        float x = c * (1.0f - std::abs(std::fmod(h * 6.0f, 2.0f) - 1.0f));
        float m = v - c;
        
        float r, g, b;
        if (h < 1.0f/6.0f) {
            r = c; g = x; b = 0;
        } else if (h < 2.0f/6.0f) {
            r = x; g = c; b = 0;
        } else if (h < 3.0f/6.0f) {
            r = 0; g = c; b = x;
        } else if (h < 4.0f/6.0f) {
            r = 0; g = x; b = c;
        } else if (h < 5.0f/6.0f) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }
        
        optionText.setFillColor(sf::Color(
            static_cast<unsigned char>((r + m) * 255),
            static_cast<unsigned char>((g + m) * 255),
            static_cast<unsigned char>((b + m) * 255)
        ));
        optionText.setStyle(sf::Text::Bold);
    } else {
        optionText.setFillColor(sf::Color(255, 255, 255));
    }
    
    sf::FloatRect bounds = optionText.getLocalBounds();
    float textY = y + btnH/2 + style.textOffsetY;
    optionText.setPosition(sf::Vector2f(centerX - bounds.size.x/2, textY));
    window.draw(optionText);
}

void drawGameOver(sf::RenderWindow& window, int finalScore, int finalLines, int finalLevel, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Font& font, bool fontLoaded, const SaveData& saveData, int dropScore, int lineScore, int comboScore, ClassicDifficulty difficulty, bool isSprintMode, float sprintTime, int sprintTarget, bool sprintCompleted, bool isChallengeMode, bool isPracticeMode, float uiAlpha, sf::Color frameColor, float statsRevealTime, bool isNewHighScore, int previousHighScore, sf::Color backgroundColor) {
    if (uiAlpha < 0.0f) uiAlpha = 0.0f;
    if (uiAlpha > 1.0f) uiAlpha = 1.0f;


    (void)textures;
    (void)useTextures;
    (void)isSprintMode;
    (void)sprintTime;
    (void)sprintTarget;
    (void)sprintCompleted;
    (void)isChallengeMode;
    (void)isPracticeMode;

    auto scaleAlpha = [uiAlpha](sf::Color c) {
        c.a = static_cast<std::uint8_t>(static_cast<float>(c.a) * uiAlpha);
        return c;
    };

    sf::RectangleShape overlay;
    sf::Color overlayColor = backgroundColor;
    overlayColor.a = 200;
    overlay.setFillColor(scaleAlpha(overlayColor));
    overlay.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);

    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;


    sf::RectangleShape frame;
    frame.setSize(sf::Vector2f(1020, 1020));
    frame.setFillColor(scaleAlpha(sf::Color(20, 20, 30, 255)));
    frame.setOutlineColor(scaleAlpha(frameColor));
    frame.setOutlineThickness(4);
    frame.setPosition(sf::Vector2f(centerX - frame.getSize().x / 2.0f, centerY - frame.getSize().y / 2.0f));
    window.draw(frame);

    if (fontLoaded) {

        sf::Text title(font, "GAME OVER");
        title.setCharacterSize(90);
        title.setFillColor(scaleAlpha(sf::Color::Yellow));
        title.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = title.getLocalBounds();
        float textY = frame.getPosition().y + 100.0f;
        title.setPosition(sf::Vector2f(centerX - titleBounds.size.x / 2.0f, textY));
        window.draw(title);


        const float statsDuration = 1.0f;
        const float lineScoreStart = 1.5f;
        const float totalStart = lineScoreStart;
        const float comboStart = lineScoreStart + statsDuration;
        const float dropsStart = comboStart + statsDuration;

        auto animProgress = [statsDuration](float now, float start) {
            float t = (now - start) / statsDuration;
            t = std::clamp(t, 0.0f, 1.0f);
            return t;
        };

        float lineScoreProgress = animProgress(statsRevealTime, lineScoreStart);
        float comboProgress = animProgress(statsRevealTime, comboStart);
        float dropsProgress = animProgress(statsRevealTime, dropsStart);

        textY += 150.0f;
        const float lineSpacing = 46.0f;

        int shownLevel = finalLevel;
        int shownLines = finalLines;
        int shownLineScore = static_cast<int>(std::round(static_cast<float>(lineScore) * lineScoreProgress));
        int shownCombo = static_cast<int>(std::round(static_cast<float>(comboScore) * comboProgress));
        int shownDrops = static_cast<int>(std::round(static_cast<float>(dropScore) * dropsProgress));
        int shownTotal = shownLineScore + shownCombo + shownDrops;

        sf::Text levelText(font, "LEVEL " + std::to_string(shownLevel));
        levelText.setCharacterSize(48);
        levelText.setFillColor(scaleAlpha(sf::Color::White));
        sf::FloatRect levelBounds = levelText.getLocalBounds();
        levelText.setPosition(sf::Vector2f(centerX - levelBounds.size.x / 2.0f, textY));
        window.draw(levelText);

        textY += lineSpacing;
        sf::Text linesCountText(font, "LINES " + std::to_string(shownLines));
        linesCountText.setCharacterSize(48);
        linesCountText.setFillColor(scaleAlpha(sf::Color::White));
        sf::FloatRect linesCountBounds = linesCountText.getLocalBounds();
        linesCountText.setPosition(sf::Vector2f(centerX - linesCountBounds.size.x / 2.0f, textY));
        window.draw(linesCountText);

        textY += lineSpacing * 2.5f;
        if (statsRevealTime >= totalStart) {
            sf::Text totalText(font, "TOTAL SCORE: " + std::to_string(shownTotal));
            totalText.setCharacterSize(56);
            totalText.setFillColor(scaleAlpha(sf::Color::Yellow));
            sf::FloatRect totalBounds = totalText.getLocalBounds();
            totalText.setPosition(sf::Vector2f(centerX - totalBounds.size.x / 2.0f, textY));
            window.draw(totalText);
        }

        textY += lineSpacing;
        textY += lineSpacing;
        if (statsRevealTime >= lineScoreStart) {
            sf::Text linesText(font, "LINES - " + std::to_string(shownLineScore));
            linesText.setCharacterSize(32);
            linesText.setFillColor(scaleAlpha(sf::Color::White));
            sf::FloatRect linesBounds = linesText.getLocalBounds();
            linesText.setPosition(sf::Vector2f(centerX - linesBounds.size.x / 2.0f, textY));
            window.draw(linesText);
        }

        textY += lineSpacing;
        if (statsRevealTime >= comboStart) {
            sf::Text comboText(font, "COMBO - " + std::to_string(shownCombo));
            comboText.setCharacterSize(32);
            comboText.setFillColor(scaleAlpha(sf::Color::White));
            sf::FloatRect comboBounds = comboText.getLocalBounds();
            comboText.setPosition(sf::Vector2f(centerX - comboBounds.size.x / 2.0f, textY));
            window.draw(comboText);
        }

        textY += lineSpacing;
        if (statsRevealTime >= dropsStart) {
            sf::Text dropText(font, "DROPS - " + std::to_string(shownDrops));
            dropText.setCharacterSize(32);
            dropText.setFillColor(scaleAlpha(sf::Color::White));
            sf::FloatRect dropBounds = dropText.getLocalBounds();
            dropText.setPosition(sf::Vector2f(centerX - dropBounds.size.x / 2.0f, textY));
            window.draw(dropText);
        }

        int currentHighScore = 0;
        switch (difficulty) {
            case ClassicDifficulty::Normal:
                currentHighScore = saveData.highScoreClassicNormal;
                break;
            case ClassicDifficulty::Hard:
                currentHighScore = saveData.highScoreClassicHard;
                break;
        }


        int baselineHighScore = (isNewHighScore && previousHighScore > 0) ? previousHighScore : currentHighScore;
        bool highBeaten = isNewHighScore && (shownTotal >= baselineHighScore);

        textY += lineSpacing * 2.0f;
        if (highBeaten) {
            sf::Text highText(font, "NEW HIGHSCORE!");
            highText.setCharacterSize(40);
            highText.setFillColor(scaleAlpha(sf::Color::Yellow));
            highText.setStyle(sf::Text::Bold);
            sf::FloatRect highBounds = highText.getLocalBounds();
            highText.setPosition(sf::Vector2f(centerX - highBounds.size.x / 2.0f, frame.getPosition().y + frame.getSize().y - 120.0f));
            window.draw(highText);
        }

        if (statsRevealTime >= dropsStart) {
            sf::Text restartHint(font, "Press R to restart or Escape to go back to menu");
            restartHint.setCharacterSize(28);
            restartHint.setFillColor(scaleAlpha(sf::Color(200, 200, 220)));
            sf::FloatRect restartBounds = restartHint.getLocalBounds();
            float restartY = frame.getPosition().y + frame.getSize().y - 60.0f;
            restartHint.setPosition(sf::Vector2f(centerX - restartBounds.size.x / 2.0f, restartY));
            window.draw(restartHint);
        }
    }
}

void drawTesseraTitle(sf::RenderWindow& window, const sf::Font& font, bool fontLoaded) {
    if (!fontLoaded) return;
    
    sf::Text titleText(font, GAME_VERSION);
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color(250, 250, 250));
    titleText.setStyle(sf::Text::Bold);
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(2);
    
    titleText.setPosition(sf::Vector2f(10, SCREEN_HEIGHT - 40));
    
    window.draw(titleText);
}

void drawPauseMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, PauseOption selectedOption, const sf::Color& frameColor, const sf::Color& backgroundColor) {
    sf::RectangleShape overlay;
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    overlay.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    sf::RectangleShape menuBg;
    menuBg.setFillColor(sf::Color(backgroundColor.r, backgroundColor.g, backgroundColor.b, 240));
    menuBg.setOutlineColor(frameColor);
    menuBg.setOutlineThickness(4);
    menuBg.setSize(sf::Vector2f(400, 350));
    menuBg.setPosition(sf::Vector2f(centerX - 200, centerY - 175));
    window.draw(menuBg);
    
    if (fontLoaded) {
        sf::Text pausedText(menuFont, "Tessera");
        pausedText.setCharacterSize(48);
        pausedText.setFillColor(frameColor);
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
    overlay.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    overlay.setPosition(sf::Vector2f(0, 0));
    window.draw(overlay);
    
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    

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
        
        sf::Text questionText2(menuFont, "all your data (scores, times & achievements)?");
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

void drawFirstTimeSetup(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ControlScheme selectedScheme, const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    if (!fontLoaded) return;
    

    sf::Text titleText(titleFont, "Welcome to Tessera");
    titleText.setCharacterSize(96);
    titleText.setFillColor(sf::Color(100, 255, 150));
    titleText.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
    titleText.setPosition(sf::Vector2f(centerX, centerY - 300.0f));
    window.draw(titleText);
    

    sf::Text controlsTitle(menuFont, "Choose Your Control Scheme");
    controlsTitle.setCharacterSize(56);
    controlsTitle.setFillColor(sf::Color(255, 200, 100));
    controlsTitle.setStyle(sf::Text::Bold);
    sf::FloatRect controlsTitleBounds = controlsTitle.getLocalBounds();
    controlsTitle.setPosition(sf::Vector2f(centerX - controlsTitleBounds.size.x / 2.0f, centerY - 250.0f));
    window.draw(controlsTitle);
    

    MenuOptionStyle schemeStyle = MenuOptionStyle::MainMenuStyle();
    schemeStyle.fallbackSelectorWidth = 600.0f;
    

    bool isClassicSelected = (selectedScheme == ControlScheme::Classic);
    float classicY = centerY - 50.0f;
    drawMenuButton(window, centerX - 250, classicY, isClassicSelected, textures, useTextures, schemeStyle);
    
    sf::Text classicText(menuFont, "CLASSIC");
    classicText.setCharacterSize(48);
    classicText.setFillColor(isClassicSelected ? sf::Color::Yellow : sf::Color::White);
    if (isClassicSelected) classicText.setStyle(sf::Text::Bold);
    sf::FloatRect classicBounds = classicText.getLocalBounds();
    classicText.setPosition(sf::Vector2f(centerX - 250 - classicBounds.size.x / 2.0f, classicY + 15));
    window.draw(classicText);
    

    sf::Text classicDesc1(menuFont, "Arrows - Move/Fast Fall");
    classicDesc1.setCharacterSize(22);
    classicDesc1.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect classicDesc1Bounds = classicDesc1.getLocalBounds();
    classicDesc1.setPosition(sf::Vector2f(centerX - 250 - classicDesc1Bounds.size.x / 2.0f, classicY + 80));
    window.draw(classicDesc1);
    
    sf::Text classicDesc2(menuFont, "Ctrl / Up - Rotate");
    classicDesc2.setCharacterSize(22);
    classicDesc2.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect classicDesc2Bounds = classicDesc2.getLocalBounds();
    classicDesc2.setPosition(sf::Vector2f(centerX - 250 - classicDesc2Bounds.size.x / 2.0f, classicY + 110));
    window.draw(classicDesc2);
    
    sf::Text classicDesc3(menuFont, "Shift - Hold | Z - Bomb");
    classicDesc3.setCharacterSize(22);
    classicDesc3.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect classicDesc3Bounds = classicDesc3.getLocalBounds();
    classicDesc3.setPosition(sf::Vector2f(centerX - 250 - classicDesc3Bounds.size.x / 2.0f, classicY + 140));
    window.draw(classicDesc3);
    


    bool isAltSelected = (selectedScheme == ControlScheme::Alternative);
    float altY = centerY - 50.0f;
    drawMenuButton(window, centerX + 250, altY, isAltSelected, textures, useTextures, schemeStyle);
    
    sf::Text altText(menuFont, "ALTERNATIVE");
    altText.setCharacterSize(48);
    altText.setFillColor(isAltSelected ? sf::Color::Yellow : sf::Color::White);
    if (isAltSelected) altText.setStyle(sf::Text::Bold);
    sf::FloatRect altBounds = altText.getLocalBounds();
    altText.setPosition(sf::Vector2f(centerX + 250 - altBounds.size.x / 2.0f, altY + 15));
    window.draw(altText);
    

    sf::Text altDesc1(menuFont, "A / D - Move | S - Fast Fall");
    altDesc1.setCharacterSize(22);
    altDesc1.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect altDesc1Bounds = altDesc1.getLocalBounds();
    altDesc1.setPosition(sf::Vector2f(centerX + 250 - altDesc1Bounds.size.x / 2.0f, altY + 80));
    window.draw(altDesc1);
    
    sf::Text altDesc2(menuFont, "J / K - Rotate");
    altDesc2.setCharacterSize(22);
    altDesc2.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect altDesc2Bounds = altDesc2.getLocalBounds();
    altDesc2.setPosition(sf::Vector2f(centerX + 250 - altDesc2Bounds.size.x / 2.0f, altY + 110));
    window.draw(altDesc2);
    
    sf::Text altDesc3(menuFont, "L - Hold | I - Bomb");
    altDesc3.setCharacterSize(22);
    altDesc3.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect altDesc3Bounds = altDesc3.getLocalBounds();
    altDesc3.setPosition(sf::Vector2f(centerX + 250 - altDesc3Bounds.size.x / 2.0f, altY + 140));
    window.draw(altDesc3);
    

    sf::Text versionText(menuFont, "First Time Setup - v1.0");
    versionText.setCharacterSize(20);
    versionText.setFillColor(sf::Color(100, 100, 100));
    versionText.setPosition(sf::Vector2f(20.0f, SCREEN_HEIGHT - 40.0f));
    window.draw(versionText);
}

void drawWelcomeScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    if (!fontLoaded) return;
    

    sf::Text titleText(titleFont, "Welcome to Tessera");
    titleText.setCharacterSize(96);
    titleText.setFillColor(sf::Color(100, 255, 150));
    titleText.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
    titleText.setPosition(sf::Vector2f(centerX, centerY - 150.0f));
    window.draw(titleText);
    

    sf::Text messageText(menuFont, "Thanks for playing Tessera");
    messageText.setCharacterSize(48);
    messageText.setFillColor(sf::Color::White);
    messageText.setStyle(sf::Text::Bold);
    
    sf::FloatRect messageBounds = messageText.getLocalBounds();
    messageText.setOrigin(sf::Vector2f(messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f));
    messageText.setPosition(sf::Vector2f(centerX, centerY));
    window.draw(messageText);
    

    sf::Text instructionText(menuFont, "Press any key to continue");
    instructionText.setCharacterSize(36);
    instructionText.setFillColor(sf::Color(200, 200, 200));
    
    sf::FloatRect instructionBounds = instructionText.getLocalBounds();
    instructionText.setOrigin(sf::Vector2f(instructionBounds.size.x / 2.0f, instructionBounds.size.y / 2.0f));
    instructionText.setPosition(sf::Vector2f(centerX, centerY + 100.0f));
    window.draw(instructionText);
}

void drawMainMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, MenuOption selectedOption, bool debugMode, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    

    if (useTextures && textures.find(TextureType::TesseraLogo) != textures.end()) {
        sf::Sprite logoSprite(textures.at(TextureType::TesseraLogo));
        sf::Vector2u logoSize = textures.at(TextureType::TesseraLogo).getSize();
        

        float logoScale = 0.45f;
        logoSprite.setScale(sf::Vector2f(logoScale, logoScale));
        

        float hueShift = std::fmod(elapsedTime / 5.0f, 1.0f);
        
        static sf::Shader hueShader;
        static bool shaderLoaded = false;
        if (!shaderLoaded) {
            if (hueShader.loadFromFile("Assets/Shaders/hue_shift.frag", sf::Shader::Type::Fragment)) {
                shaderLoaded = true;
            } else {
            }
        }

        float scaledWidth = logoSize.x * logoScale;
        float scaledHeight = logoSize.y * logoScale;
        logoSprite.setPosition(sf::Vector2f(centerX - scaledWidth/2, centerY - 350));
        
        if (shaderLoaded && sf::Shader::isAvailable()) {
            hueShader.setUniform("texture", sf::Shader::CurrentTexture);
            hueShader.setUniform("hueShift", hueShift);
            window.draw(logoSprite, &hueShader);
        } else {
            window.draw(logoSprite);
        }
    } else if (fontLoaded) {

        sf::Text titleText(titleFont, "Tessera");
        titleText.setCharacterSize(128);
        titleText.setFillColor(sf::Color(100, 255, 150));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 350));
        window.draw(titleText);
    }
    
    if (fontLoaded) {
        sf::Text versionText(menuFont, GAME_VERSION);
        versionText.setCharacterSize(24);
        versionText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect versionBounds = versionText.getLocalBounds();
        versionText.setPosition(sf::Vector2f(centerX - versionBounds.size.x/2, centerY - 140));
        window.draw(versionText);
        
        MenuOptionStyle mainStyle = MenuOptionStyle::MainMenuStyle();
        
        float hueShift = std::fmod(elapsedTime / 5.0f, 1.0f);
        
        drawMenuOption(window, menuFont, "START", centerX, centerY - 60,
                      selectedOption == MenuOption::Start, textures, useTextures, mainStyle, nullptr, hueShift, debugMode);
        
        drawMenuOption(window, menuFont, "EXTRAS", centerX, centerY + 50,
                      selectedOption == MenuOption::Extras, textures, useTextures, mainStyle, nullptr, hueShift, debugMode);
        
        drawMenuOption(window, menuFont, "OPTIONS", centerX, centerY + 160,
                      selectedOption == MenuOption::Options, textures, useTextures, mainStyle, nullptr, hueShift, debugMode);
        
        drawMenuOption(window, menuFont, "EXIT", centerX, centerY + 270,
                      selectedOption == MenuOption::Exit, textures, useTextures, mainStyle, nullptr, hueShift, debugMode);
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
        debugText.setPosition(sf::Vector2f(SCREEN_WIDTH - 180.0f, SCREEN_HEIGHT - 40.0f));
        window.draw(debugText);
    }
}

void drawGameModeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, GameModeOption selectedOption, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime, bool debugMode) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    

    drawGameLogo(window, titleFont, fontLoaded, textures, useTextures, elapsedTime);
    
    if (fontLoaded) {
        std::vector<std::pair<std::string, GameModeOption>> options = {
            {"CLASSIC", GameModeOption::Classic},
            {"BLITZ", GameModeOption::Sprint},
            {"CHALLENGE", GameModeOption::Challenge}
        };
        
        float startY = centerY - 80;
        float spacing = 90;
        MenuOptionStyle style = MenuOptionStyle::MainMenuStyle();
        
        for (size_t i = 0; i < options.size(); ++i) {
            bool isDisabled = false;
            bool isSelected = (selectedOption == options[i].second);
            
            float hueShift = std::fmod(elapsedTime / 5.0f, 1.0f);
            drawMenuOption(window, menuFont, options[i].first, centerX, startY + i * spacing - 5,
                          isSelected, textures, useTextures, style, nullptr, hueShift, debugMode);
            
            if (isDisabled) {
                sf::Text disabledText(menuFont, "(Coming Soon)");
                disabledText.setCharacterSize(24);
                disabledText.setFillColor(sf::Color(120, 120, 120));
                sf::FloatRect disabledBounds = disabledText.getLocalBounds();
                disabledText.setPosition(sf::Vector2f(centerX - disabledBounds.size.x/2, startY + i * spacing + 50));
                window.draw(disabledText);
            }
        }
    }
}

void drawClassicDifficultyMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ClassicDifficulty selectedOption, const SaveData& saveData, const std::map<TextureType, sf::Texture>& textures, bool useTextures, bool debugMode) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
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
            {"HARD", ClassicDifficulty::Hard, saveData.highScoreClassicHard},
            {"NORMAL", ClassicDifficulty::Normal, saveData.highScoreClassicNormal}
        };
        
        float startY = centerY - 80;
        float spacing = 90;
        MenuOptionStyle style = MenuOptionStyle::MainMenuStyle();
        style.fallbackSelectorWidth = 550.0f;
        
        for (size_t i = 0; i < options.size(); ++i) {
            bool isSelected = (selectedOption == std::get<1>(options[i]));
            float btnH = drawMenuButton(window, centerX, startY + i * spacing - 5, isSelected, textures, useTextures, style, debugMode, 550.0f);
            

            sf::Text optionText(menuFont, std::get<0>(options[i]));
            optionText.setCharacterSize(48);
            optionText.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
            if (isSelected) optionText.setStyle(sf::Text::Bold);
            float textY = startY + i * spacing + btnH/2 - 20;
            optionText.setPosition(sf::Vector2f(centerX - 200, textY));
            window.draw(optionText);
            

            int highScore = std::get<2>(options[i]);
            sf::Text scoreText(menuFont, "Best: " + std::to_string(highScore));
            scoreText.setCharacterSize(32);
            scoreText.setFillColor(sf::Color(180, 180, 180));
            scoreText.setPosition(sf::Vector2f(centerX + 80, textY + 8));
            window.draw(scoreText);
        }
        
    }
}

void drawSprintLinesMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, SprintLines selectedOption, const SaveData& saveData, bool debugMode, const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
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
        
        float startY = centerY - 80;
        float spacing = 90;
        MenuOptionStyle style = MenuOptionStyle::MainMenuStyle();
        style.fallbackSelectorWidth = 550.0f;
        
        for (size_t i = 0; i < options.size(); ++i) {
            bool isSelected = (selectedOption == std::get<1>(options[i]));
            float btnH = drawMenuButton(window, centerX, startY + i * spacing - 5, isSelected, textures, useTextures, style, debugMode, 550.0f);
            

            sf::Text optionText(menuFont, std::get<0>(options[i]));
            optionText.setCharacterSize(48);
            optionText.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
            if (isSelected) optionText.setStyle(sf::Text::Bold);
            float textY = startY + i * spacing + btnH/2 - 20;
            optionText.setPosition(sf::Vector2f(centerX - 180, textY));
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
            timeText.setPosition(sf::Vector2f(centerX + 80, textY + 8));
            window.draw(timeText);
        }
    }
}

void drawChallengeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ChallengeMode selectedOption, bool debugMode, const SaveData& saveData, const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "CHALLENGE MODE");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color(255, 100, 255));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 350));
        window.draw(titleText);
        
        std::vector<std::pair<std::string, ChallengeMode>> options;
        
        options.push_back({"THE FOREST", ChallengeMode::TheForest});
        options.push_back({"RANDOMNESS", ChallengeMode::Randomness});
        options.push_back({"NON STRAIGHT", ChallengeMode::NonStraight});
        options.push_back({"ONE ROTATION", ChallengeMode::OneRot});
        options.push_back({"THE CURSE", ChallengeMode::ChristopherCurse});
        options.push_back({"VANISHING", ChallengeMode::Vanishing});
        options.push_back({"AUTO DROP", ChallengeMode::AutoDrop});
        options.push_back({"GRAVITY FLIP", ChallengeMode::GravityFlip});
        options.push_back({"PETRIFY", ChallengeMode::Petrify});
        

        float columnSpacing = 450.0f;
        float leftColumnX = centerX - columnSpacing / 2;
        float rightColumnX = centerX + columnSpacing / 2;
        float startY = centerY - 150;
        float spacing = 100;
        float buttonWidth = 400.0f;
        MenuOptionStyle challengeStyle = MenuOptionStyle::MainMenuStyle();
        
        int itemsPerColumn = (options.size() + 1) / 2;
        
        for (size_t i = 0; i < options.size(); ++i) {
            bool isCompleted = false;
            switch (options[i].second) {
                case ChallengeMode::TheForest:
                    isCompleted = saveData.bestTimeChallengeTheForest > 0.0f;
                    break;
                case ChallengeMode::Randomness:
                    isCompleted = saveData.bestTimeChallengeRandomness > 0.0f;
                    break;
                case ChallengeMode::NonStraight:
                    isCompleted = saveData.bestTimeChallengeNonStraight > 0.0f;
                    break;
                case ChallengeMode::OneRot:
                    isCompleted = saveData.bestTimeChallengeOneRot > 0.0f;
                    break;
                case ChallengeMode::ChristopherCurse:
                    isCompleted = saveData.bestTimeChallengeChristopherCurse > 0.0f;
                    break;
                case ChallengeMode::Vanishing:
                    isCompleted = saveData.bestTimeChallengeVanishing > 0.0f;
                    break;
                case ChallengeMode::AutoDrop:
                    isCompleted = saveData.bestTimeChallengeAutoDrop > 0.0f;
                    break;
                case ChallengeMode::GravityFlip:
                    isCompleted = saveData.bestTimeChallengeGravityFlip > 0.0f;
                    break;
                case ChallengeMode::Petrify:
                    isCompleted = saveData.bestTimeChallengePetrify > 0.0f;
                    break;
            }
            

            int column = i < static_cast<size_t>(itemsPerColumn) ? 0 : 1;
            int row = column == 0 ? i : i - itemsPerColumn;
            float columnX = column == 0 ? leftColumnX : rightColumnX;
            float buttonY = startY + row * spacing - 5;
            
            bool isSelected = (selectedOption == options[i].second);
            float btnH = drawMenuButton(window, columnX, buttonY, isSelected, textures, useTextures, challengeStyle, debugMode, buttonWidth);
            

            sf::Text optionText(menuFont, options[i].first);
            optionText.setCharacterSize(40);
            
            if (isSelected) {
                optionText.setFillColor(sf::Color::Yellow);
                optionText.setStyle(sf::Text::Bold);
            } else if (isCompleted) {
                optionText.setFillColor(sf::Color(100, 255, 100));
            } else {
                optionText.setFillColor(sf::Color::White);
            }
            
            sf::FloatRect optionBounds = optionText.getLocalBounds();
            float textY = buttonY + btnH/2 - 20;
            optionText.setPosition(sf::Vector2f(columnX - optionBounds.size.x/2, textY));
            window.draw(optionText);
        }
        
    }
}

void drawPracticeMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, PracticeDifficulty selectedDifficulty, PracticeLineGoal selectedLineGoal, bool infiniteBombs, PracticeStartLevel selectedStartLevel, int selectedOption, const std::map<TextureType, sf::Texture>& textures, bool useTextures, bool debugMode) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, "PRACTICE MODE");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color(150, 200, 255));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY - 380));
        window.draw(titleText);
        

        float startY = centerY - 220;
        float spacing = 85;
        MenuOptionStyle practiceStyle = MenuOptionStyle::MainMenuStyle();
        practiceStyle.fallbackSelectorWidth = 700.0f;
        

        {
            sf::Text label(menuFont, "DIFFICULTY:");
            label.setCharacterSize(36);
            label.setFillColor(selectedOption == 0 ? sf::Color::Yellow : sf::Color(180, 180, 180));
            label.setPosition(sf::Vector2f(centerX - 300, startY));
            window.draw(label);
            
            std::string diffText;
            switch (selectedDifficulty) {
                case PracticeDifficulty::VeryEasy: diffText = "VERY EASY"; break;
                case PracticeDifficulty::Easy: diffText = "EASY"; break;
                case PracticeDifficulty::Medium: diffText = "MEDIUM"; break;
                case PracticeDifficulty::Hard: diffText = "HARD"; break;
            }
            
            bool isSelected = (selectedOption == 0);
            drawMenuButton(window, centerX, startY - 5, isSelected, textures, useTextures, practiceStyle, debugMode, 700.0f);
            
            sf::Text value(menuFont, diffText);
            value.setCharacterSize(40);
            value.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
            value.setStyle(isSelected ? sf::Text::Bold : sf::Text::Regular);
            value.setPosition(sf::Vector2f(centerX + 100, startY - 5));
            window.draw(value);
        }
        

        {
            sf::Text label(menuFont, "LINE GOAL:");
            label.setCharacterSize(36);
            label.setFillColor(selectedOption == 1 ? sf::Color::Yellow : sf::Color(180, 180, 180));
            label.setPosition(sf::Vector2f(centerX - 300, startY + spacing));
            window.draw(label);
            
            std::string goalText;
            switch (selectedLineGoal) {
                case PracticeLineGoal::Infinite: goalText = "INFINITE"; break;
                case PracticeLineGoal::Lines24: goalText = "24 LINES"; break;
                case PracticeLineGoal::Lines48: goalText = "48 LINES"; break;
                case PracticeLineGoal::Lines96: goalText = "96 LINES"; break;
            }
            
            bool isSelected = (selectedOption == 1);
            drawMenuButton(window, centerX, startY + spacing - 5, isSelected, textures, useTextures, practiceStyle, debugMode, 700.0f);
            
            sf::Text value(menuFont, goalText);
            value.setCharacterSize(40);
            value.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
            value.setStyle(isSelected ? sf::Text::Bold : sf::Text::Regular);
            value.setPosition(sf::Vector2f(centerX + 100, startY + spacing - 5));
            window.draw(value);
        }

        {
            sf::Text label(menuFont, "INFINITE BOMBS:");
            label.setCharacterSize(36);
            label.setFillColor(selectedOption == 2 ? sf::Color::Yellow : sf::Color(180, 180, 180));
            label.setPosition(sf::Vector2f(centerX - 300, startY + spacing * 2));
            window.draw(label);
            
            std::string bombText = infiniteBombs ? "YES" : "NO";
            
            bool isSelected = (selectedOption == 2);
            drawMenuButton(window, centerX, startY + spacing * 2 - 5, isSelected, textures, useTextures, practiceStyle, debugMode, 700.0f);
            
            sf::Text value(menuFont, bombText);
            value.setCharacterSize(40);
            value.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
            value.setStyle(isSelected ? sf::Text::Bold : sf::Text::Regular);
            value.setPosition(sf::Vector2f(centerX + 100, startY + spacing * 2 - 5));
            window.draw(value);
        }

        {
            sf::Text label(menuFont, "START LEVEL:");
            label.setCharacterSize(36);
            label.setFillColor(selectedOption == 3 ? sf::Color::Yellow : sf::Color(180, 180, 180));
            label.setPosition(sf::Vector2f(centerX - 300, startY + spacing * 3));
            window.draw(label);
            
            std::string levelText = "LEVEL " + std::to_string(static_cast<int>(selectedStartLevel));
            
            bool isSelected = (selectedOption == 3);
            drawMenuButton(window, centerX, startY + spacing * 3 - 5, isSelected, textures, useTextures, practiceStyle, debugMode, 700.0f);
            
            sf::Text value(menuFont, levelText);
            value.setCharacterSize(40);
            value.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
            value.setStyle(isSelected ? sf::Text::Bold : sf::Text::Regular);
            value.setPosition(sf::Vector2f(centerX + 100, startY + spacing * 3 - 5));
            window.draw(value);
        }

        {
            MenuOptionStyle startStyle = MenuOptionStyle::MainMenuStyle();
            drawMenuOption(window, menuFont, "START PRACTICE", centerX, startY + spacing * 4 + 25,
                          selectedOption == 4, textures, useTextures, startStyle, nullptr, 0.0f, debugMode);
        }
    }
}

void drawOptionsMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, bool debugMode, OptionsMenuOption selectedOption, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    

    drawGameLogo(window, menuFont, fontLoaded, textures, useTextures, elapsedTime);
    
    if (fontLoaded) {
        MenuOptionStyle optionsStyle = MenuOptionStyle::MainMenuStyle();
        float hueShift = std::fmod(elapsedTime / 5.0f, 1.0f);
        
        drawMenuOption(window, menuFont, MenuConfig::OptionsMenu::AUDIO, centerX, 
                      centerY + MenuConfig::OptionsMenu::OPTION1_Y_OFFSET,
                      selectedOption == OptionsMenuOption::Audio, textures, useTextures, optionsStyle, nullptr, hueShift, debugMode);
        
        drawMenuOption(window, menuFont, MenuConfig::OptionsMenu::REBIND_KEYS, centerX, 
                      centerY + MenuConfig::OptionsMenu::OPTION2_Y_OFFSET,
                      selectedOption == OptionsMenuOption::RebindKeys, textures, useTextures, optionsStyle, nullptr, hueShift, debugMode);
        
        drawMenuOption(window, menuFont, MenuConfig::OptionsMenu::CLEAR_ALL_DATA, centerX, 
                      centerY + MenuConfig::OptionsMenu::OPTION3_Y_OFFSET,
                      selectedOption == OptionsMenuOption::ClearScores, textures, useTextures, optionsStyle, nullptr, hueShift, debugMode);
    } else {
        sf::RectangleShape titleBar;
        titleBar.setFillColor(sf::Color(100, 255, 150));
        titleBar.setSize(sf::Vector2f(400, 80));
        titleBar.setPosition(sf::Vector2f(centerX - 200, centerY - 250));
        window.draw(titleBar);
        
        float optionY = centerY - 130;
        for (int i = 0; i < 3; i++) {
            sf::RectangleShape option;
            option.setSize(sf::Vector2f(400, 60));
            if ((i == 0 && selectedOption == OptionsMenuOption::Audio) || 
                (i == 1 && selectedOption == OptionsMenuOption::RebindKeys) ||
                (i == 2 && selectedOption == OptionsMenuOption::ClearScores)) {
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
        debugText.setPosition(sf::Vector2f(SCREEN_WIDTH - 180.0f, SCREEN_HEIGHT - 40.0f));
        window.draw(debugText);
    }
}

void drawCustomizationMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, GameThemeChoice hoveredTheme, GameThemeChoice selectedTheme, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    drawGameLogo(window, menuFont, fontLoaded, textures, useTextures, elapsedTime);
    
    if (fontLoaded) {

        sf::Text titleText(menuFont);
        titleText.setString(MenuConfig::CustomizationMenu::TITLE);
        titleText.setCharacterSize(48);
        titleText.setFillColor(sf::Color(200, 200, 220));
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, centerY + MenuConfig::CustomizationMenu::TITLE_Y_OFFSET));
        window.draw(titleText);
        
        MenuOptionStyle style = MenuOptionStyle::MainMenuStyle();
        float hueShift = std::fmod(elapsedTime / 5.0f, 1.0f);
        

        std::string classicLabel = std::string(MenuConfig::CustomizationMenu::CLASSIC);
        if (selectedTheme == GameThemeChoice::Classic) {
            classicLabel += " [SELECTED]";
        }
        drawMenuOption(window, menuFont, classicLabel.c_str(), centerX, 
                      centerY + MenuConfig::CustomizationMenu::OPTION1_Y_OFFSET,
                      hoveredTheme == GameThemeChoice::Classic, textures, useTextures, style, nullptr, hueShift, false);
        

        std::string forestLabel = std::string(MenuConfig::CustomizationMenu::FOREST);
        if (selectedTheme == GameThemeChoice::Forest) {
            forestLabel += " [SELECTED]";
        }
        drawMenuOption(window, menuFont, forestLabel.c_str(), centerX, 
                      centerY + MenuConfig::CustomizationMenu::OPTION2_Y_OFFSET,
                      hoveredTheme == GameThemeChoice::Forest, textures, useTextures, style, nullptr, hueShift, false);
        

        std::string racerLabel = std::string(MenuConfig::CustomizationMenu::RACER);
        if (selectedTheme == GameThemeChoice::Racer) {
            racerLabel += " [SELECTED]";
        }
        drawMenuOption(window, menuFont, racerLabel.c_str(), centerX, 
                      centerY + MenuConfig::CustomizationMenu::OPTION3_Y_OFFSET,
                      hoveredTheme == GameThemeChoice::Racer, textures, useTextures, style, nullptr, hueShift, false);
        

        sf::Text infoText(menuFont);
        infoText.setString("Theme affects Classic and Blitz modes");
        infoText.setCharacterSize(32);
        infoText.setFillColor(sf::Color(150, 150, 170));
        sf::FloatRect infoBounds = infoText.getLocalBounds();
        infoText.setPosition(sf::Vector2f(centerX - infoBounds.size.x/2, centerY + 400.0f));
        window.draw(infoText);
    }
}

void drawVolumeSlider(sf::RenderWindow& window, const sf::Font& font, float centerX, float centerY, 
                      const std::string& label, float value, bool isSelected, bool debugMode) {
    using namespace MenuConfig::AudioMenu;
    

    sf::Text labelText(font);
    labelText.setString(label);
    labelText.setCharacterSize(36);
    labelText.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
    labelText.setStyle(sf::Text::Bold);
    sf::FloatRect labelBounds = labelText.getLocalBounds();
    labelText.setPosition(sf::Vector2f(centerX - SLIDER_WIDTH/2 - labelBounds.size.x - 40, centerY - labelBounds.size.y/2 - 5));
    window.draw(labelText);
    

    sf::RectangleShape sliderTrack;
    sliderTrack.setSize(sf::Vector2f(SLIDER_WIDTH, SLIDER_HEIGHT));
    sliderTrack.setPosition(sf::Vector2f(centerX - SLIDER_WIDTH/2, centerY - SLIDER_HEIGHT/2));
    sliderTrack.setFillColor(sf::Color(60, 60, 60));
    sliderTrack.setOutlineColor(isSelected ? sf::Color::Yellow : sf::Color(100, 100, 100));
    sliderTrack.setOutlineThickness(2);
    window.draw(sliderTrack);
    

    float filledWidth = (value / 100.0f) * SLIDER_WIDTH;
    sf::RectangleShape filledTrack;
    filledTrack.setSize(sf::Vector2f(filledWidth, SLIDER_HEIGHT));
    filledTrack.setPosition(sf::Vector2f(centerX - SLIDER_WIDTH/2, centerY - SLIDER_HEIGHT/2));
    filledTrack.setFillColor(isSelected ? sf::Color(255, 200, 50) : sf::Color(100, 180, 100));
    window.draw(filledTrack);
    

    float handleX = centerX - SLIDER_WIDTH/2 + filledWidth - SLIDER_HANDLE_WIDTH/2;
    sf::RectangleShape handle;
    handle.setSize(sf::Vector2f(SLIDER_HANDLE_WIDTH, SLIDER_HANDLE_HEIGHT));
    handle.setPosition(sf::Vector2f(handleX, centerY - SLIDER_HANDLE_HEIGHT/2));
    handle.setFillColor(isSelected ? sf::Color::White : sf::Color(200, 200, 200));
    handle.setOutlineColor(sf::Color::Black);
    handle.setOutlineThickness(2);
    window.draw(handle);
    

    sf::Text valueText(font);
    valueText.setString(std::to_string(static_cast<int>(value)) + "%");
    valueText.setCharacterSize(32);
    valueText.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
    valueText.setStyle(sf::Text::Bold);
    sf::FloatRect valueBounds = valueText.getLocalBounds();
    valueText.setPosition(sf::Vector2f(centerX + SLIDER_WIDTH/2 + 20, centerY - valueBounds.size.y/2 - 5));
    window.draw(valueText);
    

    if (debugMode) {
        sf::RectangleShape hitbox;
        hitbox.setSize(sf::Vector2f(SLIDER_WIDTH, SLIDER_HANDLE_HEIGHT + 20));
        hitbox.setPosition(sf::Vector2f(centerX - SLIDER_WIDTH/2, centerY - SLIDER_HANDLE_HEIGHT/2 - 10));
        hitbox.setFillColor(sf::Color::Transparent);
        hitbox.setOutlineColor(sf::Color::Red);
        hitbox.setOutlineThickness(1);
        window.draw(hitbox);
    }
}

void drawAudioMenu(sf::RenderWindow& window, const sf::Font& menuFont, bool fontLoaded, bool debugMode,
                   AudioOption selectedOption, float mainVolume, float musicVolume, float sfxVolume,
                   const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    drawGameLogo(window, menuFont, fontLoaded, textures, useTextures, elapsedTime);
    
    if (fontLoaded) {
        

        drawVolumeSlider(window, menuFont, centerX + 100, centerY + MenuConfig::AudioMenu::OPTION1_Y_OFFSET,
                        MenuConfig::AudioMenu::MAIN_VOLUME, mainVolume, 
                        selectedOption == AudioOption::MainVolume, debugMode);
        
        drawVolumeSlider(window, menuFont, centerX + 100, centerY + MenuConfig::AudioMenu::OPTION2_Y_OFFSET,
                        MenuConfig::AudioMenu::MUSIC_VOLUME, musicVolume, 
                        selectedOption == AudioOption::MusicVolume, debugMode);
        
        drawVolumeSlider(window, menuFont, centerX + 100, centerY + MenuConfig::AudioMenu::OPTION3_Y_OFFSET,
                        MenuConfig::AudioMenu::SFX_VOLUME, sfxVolume, 
                        selectedOption == AudioOption::SfxVolume, debugMode);
    }
    
    if (debugMode && fontLoaded) {
        sf::Text debugText(menuFont);
        debugText.setString("DEBUG MODE");
        debugText.setCharacterSize(24);
        debugText.setFillColor(sf::Color::Yellow);
        debugText.setPosition(sf::Vector2f(SCREEN_WIDTH - 180.0f, SCREEN_HEIGHT - 40.0f));
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

void drawRebindingScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const KeyBindings& bindings, ControlScheme selectedScheme, ControlScheme hoveredScheme, ControlScheme appliedScheme, int selectedBinding, bool waitingForKey, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime, bool debugMode) {
    const float centerX = SCREEN_WIDTH / 2.0f;
    const float centerY = SCREEN_HEIGHT / 2.0f;
    
    if (!fontLoaded) return;
    

    MenuOptionStyle buttonStyle = MenuOptionStyle::MainMenuStyle();
    buttonStyle.fallbackSelectorWidth = 300.0f;
    
    float buttonY = 180.0f;
    float buttonSpacing = 350.0f;
    

    bool isClassicHovered = (hoveredScheme == ControlScheme::Classic);
    bool isClassicSelected = (selectedScheme == ControlScheme::Classic);
    bool isClassicApplied = (appliedScheme == ControlScheme::Classic);
    
    if (isClassicApplied) {
        sf::RectangleShape appliedBg;
        appliedBg.setSize(sf::Vector2f(320, 70));
        appliedBg.setFillColor(sf::Color(100, 255, 100, 50));
        appliedBg.setOutlineColor(sf::Color(100, 255, 100));
        appliedBg.setOutlineThickness(2);
        appliedBg.setPosition(sf::Vector2f(centerX - buttonSpacing - 160, buttonY - 5));
        window.draw(appliedBg);
    }
    
    drawMenuButton(window, centerX - buttonSpacing, buttonY, isClassicHovered, textures, useTextures, buttonStyle, debugMode, 320);
    
    sf::Text classicText(menuFont, "CLASSIC");
    classicText.setCharacterSize(40);
    classicText.setFillColor(isClassicSelected ? sf::Color::Yellow : (isClassicHovered ? sf::Color::White : sf::Color(180, 180, 180)));
    if (isClassicSelected) classicText.setStyle(sf::Text::Bold);
    sf::FloatRect classicBounds = classicText.getLocalBounds();
    classicText.setPosition(sf::Vector2f(centerX - buttonSpacing - classicBounds.size.x / 2.0f, buttonY + 10));
    window.draw(classicText);
    

    bool isAltHovered = (hoveredScheme == ControlScheme::Alternative);
    bool isAltSelected = (selectedScheme == ControlScheme::Alternative);
    bool isAltApplied = (appliedScheme == ControlScheme::Alternative);
    
    if (isAltApplied) {
        sf::RectangleShape appliedBg;
        appliedBg.setSize(sf::Vector2f(320, 70));
        appliedBg.setFillColor(sf::Color(100, 255, 100, 50));
        appliedBg.setOutlineColor(sf::Color(100, 255, 100));
        appliedBg.setOutlineThickness(2);
        appliedBg.setPosition(sf::Vector2f(centerX - 160, buttonY - 5));
        window.draw(appliedBg);
    }
    
    drawMenuButton(window, centerX, buttonY, isAltHovered, textures, useTextures, buttonStyle, debugMode, 320);
    
    sf::Text altText(menuFont, "ALTERNATIVE");
    altText.setCharacterSize(40);
    altText.setFillColor(isAltSelected ? sf::Color::Yellow : (isAltHovered ? sf::Color::White : sf::Color(180, 180, 180)));
    if (isAltSelected) altText.setStyle(sf::Text::Bold);
    sf::FloatRect altBounds = altText.getLocalBounds();
    altText.setPosition(sf::Vector2f(centerX - altBounds.size.x / 2.0f, buttonY + 10));
    window.draw(altText);
    

    bool isCustomHovered = (hoveredScheme == ControlScheme::Custom);
    bool isCustomSelected = (selectedScheme == ControlScheme::Custom);
    bool isCustomApplied = (appliedScheme == ControlScheme::Custom);
    
    if (isCustomApplied) {
        sf::RectangleShape appliedBg;
        appliedBg.setSize(sf::Vector2f(320, 70));
        appliedBg.setFillColor(sf::Color(100, 255, 100, 50));
        appliedBg.setOutlineColor(sf::Color(100, 255, 100));
        appliedBg.setOutlineThickness(2);
        appliedBg.setPosition(sf::Vector2f(centerX + buttonSpacing - 160, buttonY - 5));
        window.draw(appliedBg);
    }
    
    drawMenuButton(window, centerX + buttonSpacing, buttonY, isCustomHovered, textures, useTextures, buttonStyle, debugMode, 320);
    
    sf::Text customText(menuFont, "CUSTOM");
    customText.setCharacterSize(40);
    customText.setFillColor(isCustomSelected ? sf::Color::Yellow : (isCustomHovered ? sf::Color::White : sf::Color(180, 180, 180)));
    if (isCustomSelected) customText.setStyle(sf::Text::Bold);
    sf::FloatRect customBounds = customText.getLocalBounds();
    customText.setPosition(sf::Vector2f(centerX + buttonSpacing - customBounds.size.x / 2.0f, buttonY + 10));
    window.draw(customText);
    

    float contentStartY = 320.0f;
    
    if (selectedScheme == ControlScheme::Custom) {

        const float lineHeight = 50.0f;
        
        std::vector<std::pair<std::string, std::string>> keyList = {
            {"Move Left", getKeyName(bindings.moveLeft)},
            {"Move Right", getKeyName(bindings.moveRight)},
            {"Rotate Left", getKeyName(bindings.rotateLeft)},
            {"Rotate Right", getKeyName(bindings.rotateRight)},
            {"Fast Fall", getKeyName(bindings.quickFall)},
            {"Drop", getKeyName(bindings.drop)},
            {"Hold", getKeyName(bindings.hold)},
            {"Bomb", getKeyName(bindings.bomb)},
            {"Restart", getKeyName(bindings.restart)},
            {"Mute Music", getKeyName(bindings.mute)},
            {"Music Volume Down", getKeyName(bindings.volumeDown)},
            {"Music Volume Up", getKeyName(bindings.volumeUp)},
        };
        
        for (size_t i = 0; i < keyList.size(); i++) {
            float yPos = contentStartY + i * lineHeight;
            
            if (static_cast<int>(i) == selectedBinding) {
                sf::RectangleShape selector;
                selector.setSize(sf::Vector2f(700, 45));
                selector.setFillColor(sf::Color(100, 200, 255, 128));
                selector.setPosition(sf::Vector2f(centerX - 350, yPos - 5));
                window.draw(selector);
            }
            
            sf::Text labelText(menuFont, keyList[i].first);
            labelText.setCharacterSize(32);
            labelText.setFillColor(sf::Color::White);
            labelText.setPosition(sf::Vector2f(centerX - 330, yPos));
            window.draw(labelText);
            
            sf::Text keyText(menuFont);
            if (static_cast<int>(i) == selectedBinding && waitingForKey) {
                keyText.setString("Press key...");
                keyText.setFillColor(sf::Color::Yellow);
            } else {
                keyText.setString(keyList[i].second);
                keyText.setFillColor(sf::Color(100, 255, 100));
            }
            keyText.setCharacterSize(32);
            keyText.setStyle(sf::Text::Bold);
            keyText.setPosition(sf::Vector2f(centerX + 100, yPos));
            window.draw(keyText);
        }
        
    } else {

        std::vector<std::pair<std::string, std::string>> controls;
        
        if (selectedScheme == ControlScheme::Classic) {
            controls = {
                {"Move Left", "Left Arrow"},
                {"Move Right", "Right Arrow"},
                {"Rotate Left", "Left Ctrl"},
                {"Rotate Right", "Up Arrow"},
                {"Fast Fall", "Down Arrow"},
                {"Drop", "Space"},
                {"Hold", "Left Shift"},
                {"Bomb", "Z"}
            };
        } else {
            controls = {
                {"Move Left", "A"},
                {"Move Right", "D"},
                {"Rotate Left", "J"},
                {"Rotate Right", "K"},
                {"Fast Fall", "S"},
                {"Drop", "Space"},
                {"Hold", "L"},
                {"Bomb", "I"}
            };
        }

        controls.push_back({"Restart", "R"});
        controls.push_back({"Mute Music", "M"});
        controls.push_back({"Music Volume Down", ","});
        controls.push_back({"Music Volume Up", "."});
        
        const float lineHeight = 55.0f;
        for (size_t i = 0; i < controls.size(); i++) {
            float yPos = contentStartY + i * lineHeight;
            
            sf::Text labelText(menuFont, controls[i].first);
            labelText.setCharacterSize(32);
            labelText.setFillColor(sf::Color::White);
            labelText.setPosition(sf::Vector2f(centerX - 300, yPos));
            window.draw(labelText);
            
            sf::Text keyText(menuFont, controls[i].second);
            keyText.setCharacterSize(32);
            keyText.setStyle(sf::Text::Bold);
            keyText.setFillColor(sf::Color(100, 255, 100));
            keyText.setPosition(sf::Vector2f(centerX + 100, yPos));
            window.draw(keyText);
        }
    }
}

void drawExplosionEffects(sf::RenderWindow& window, const std::vector<ExplosionEffect>& explosions) {
    for (const auto& explosion : explosions) {

        float worldX = explosion.x;
        float worldY = explosion.y;
        
        sf::RectangleShape explosionBox;
        explosionBox.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        

        explosionBox.setOrigin(sf::Vector2f(CELL_SIZE / 2.0f, CELL_SIZE / 2.0f));
        explosionBox.setPosition(sf::Vector2f(worldX + CELL_SIZE / 2.0f, worldY + CELL_SIZE / 2.0f));
        explosionBox.setRotation(sf::degrees(explosion.rotation));
        explosionBox.setFillColor(sf::Color::White);
        
        window.draw(explosionBox);
    }
}

void drawGlowEffects(sf::RenderWindow& window, const std::vector<GlowEffect>& glowEffects, const std::map<TextureType, sf::Texture>& textures) {
    for (const auto& glow : glowEffects) {
        if (textures.find(TextureType::GenericBlock) != textures.end()) {
            sf::Sprite glowSprite(textures.at(TextureType::GenericBlock));
            
            sf::Vector2u textureSize = textures.at(TextureType::GenericBlock).getSize();
            float scale = (CELL_SIZE * 1.0f) / textureSize.x;
            

            glowSprite.setOrigin(sf::Vector2f(textureSize.x / 2.0f, textureSize.y / 2.0f));
            glowSprite.setPosition(sf::Vector2f(glow.worldX + CELL_SIZE / 2.0f, glow.worldY + CELL_SIZE / 2.0f));
            glowSprite.setScale(sf::Vector2f(scale, scale));
            glowSprite.setRotation(sf::degrees(glow.rotation));
            
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
            

            glowBox.setOrigin(sf::Vector2f(CELL_SIZE / 2.0f, CELL_SIZE / 2.0f));
            glowBox.setPosition(sf::Vector2f(glow.worldX + CELL_SIZE / 2.0f, glow.worldY + CELL_SIZE / 2.0f));
            glowBox.setRotation(sf::degrees(glow.rotation));
            
            sf::Color glowColor = glow.color;
            glowColor.a = static_cast<std::uint8_t>(glow.alpha);
            glowBox.setFillColor(glowColor);
            
            window.draw(glowBox);
        }
    }
}

void drawExtrasMenu(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, ExtrasOption selectedOption, const std::map<TextureType, sf::Texture>& textures, bool useTextures, float elapsedTime, bool debugMode) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    

    drawGameLogo(window, titleFont, fontLoaded, textures, useTextures, elapsedTime);
    
    if (fontLoaded) {
        std::vector<std::pair<std::string, ExtrasOption>> options = {
            {MenuConfig::ExtrasMenu::ACHIEVEMENTS, ExtrasOption::Achievements},
            {MenuConfig::ExtrasMenu::STATISTICS, ExtrasOption::Statistics},
            {MenuConfig::ExtrasMenu::BEST_SCORES, ExtrasOption::BestScores}
        };
        
        float startY = centerY + MenuConfig::ExtrasMenu::START_Y_OFFSET;
        float spacing = MenuConfig::ExtrasMenu::SPACING;
        MenuOptionStyle extrasStyle = MenuOptionStyle::MainMenuStyle();
        float hueShift = std::fmod(elapsedTime / 5.0f, 1.0f);
        
        for (size_t i = 0; i < options.size(); ++i) {
            bool isSelected = (selectedOption == options[i].second);
            drawMenuOption(window, menuFont, options[i].first, centerX, startY + i * spacing - 5,
                          isSelected, textures, useTextures, extrasStyle, nullptr, hueShift, debugMode);
        }
    }
}

void drawAchievementsScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData, int hoveredAchievement, bool debugMode) {
    float centerX = SCREEN_WIDTH / 2.0f;
    
    if (fontLoaded) {
        sf::Text titleText(titleFont, MenuConfig::ScreenTitles::ACHIEVEMENTS);
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color(255, 215, 0));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, 80));
        window.draw(titleText);
        

        std::vector<AchievementInfo> achievements = getAllAchievementsInfo();
        

        const int COLS = 5;
        const int ROWS = 5;
        const float CELL_SIZE = 128.0f;
        const float SPACING = 15.0f;
        const float GRID_WIDTH = COLS * CELL_SIZE + (COLS - 1) * SPACING;
        const float START_X = 150.0f;
        const float START_Y = 220;
        

        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                int achievementId = row * COLS + col;
                if (achievementId >= 25) break;
                
                float x = START_X + col * (CELL_SIZE + SPACING);
                float y = START_Y + row * (CELL_SIZE + SPACING);
                
                sf::RectangleShape achievementBox;
                achievementBox.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                achievementBox.setPosition(sf::Vector2f(x, y));
                

                bool isImplemented = (achievementId < TOTAL_ACHIEVEMENTS);
                bool isUnlocked = isImplemented ? saveData.achievements[achievementId] : false;
                

                if (achievementId == hoveredAchievement) {
                    if (isUnlocked) {

                        achievementBox.setFillColor(sf::Color(80, 120, 80, 220));
                        achievementBox.setOutlineColor(sf::Color(100, 255, 100));
                    } else if (isImplemented) {

                        achievementBox.setFillColor(sf::Color(80, 80, 90, 220));
                        achievementBox.setOutlineColor(sf::Color(255, 215, 0));
                    } else {

                        achievementBox.setFillColor(sf::Color(50, 50, 55, 220));
                        achievementBox.setOutlineColor(sf::Color(100, 100, 110));
                    }
                    achievementBox.setOutlineThickness(4);
                } else {
                    if (isUnlocked) {

                        achievementBox.setFillColor(sf::Color(40, 60, 40, 200));
                        achievementBox.setOutlineColor(sf::Color(80, 200, 80));
                    } else if (isImplemented) {

                        achievementBox.setFillColor(sf::Color(60, 60, 70, 200));
                        achievementBox.setOutlineColor(sf::Color(120, 120, 130));
                    } else {

                        achievementBox.setFillColor(sf::Color(30, 30, 35, 200));
                        achievementBox.setOutlineColor(sf::Color(50, 50, 60));
                    }
                    achievementBox.setOutlineThickness(2);
                }
                
                window.draw(achievementBox);
                

                if (isUnlocked) {

                    sf::Text wonText(menuFont, "W");
                    wonText.setCharacterSize(60);
                    wonText.setFillColor(sf::Color(100, 255, 100));
                    wonText.setStyle(sf::Text::Bold);
                    sf::FloatRect wonBounds = wonText.getLocalBounds();
                    wonText.setPosition(sf::Vector2f(x + CELL_SIZE/2 - wonBounds.size.x/2, y + CELL_SIZE/2 - wonBounds.size.y/2 - 15));
                    window.draw(wonText);
                } else if (isImplemented) {

                    sf::Text lockedText(menuFont, "L");
                    lockedText.setCharacterSize(60);
                    lockedText.setFillColor(sf::Color(180, 180, 190));
                    lockedText.setStyle(sf::Text::Bold);
                    sf::FloatRect lockedBounds = lockedText.getLocalBounds();
                    lockedText.setPosition(sf::Vector2f(x + CELL_SIZE/2 - lockedBounds.size.x/2, y + CELL_SIZE/2 - lockedBounds.size.y/2 - 15));
                    window.draw(lockedText);
                } else {

                    sf::Text emptyText(menuFont, "X");
                    emptyText.setCharacterSize(70);
                    emptyText.setFillColor(sf::Color(80, 80, 85));
                    emptyText.setStyle(sf::Text::Bold);
                    sf::FloatRect emptyBounds = emptyText.getLocalBounds();
                    emptyText.setPosition(sf::Vector2f(x + CELL_SIZE/2 - emptyBounds.size.x/2, y + CELL_SIZE/2 - emptyBounds.size.y/2 - 15));
                    window.draw(emptyText);
                }
            }
        }
        

        if (hoveredAchievement >= 0 && hoveredAchievement < 25) {
            const float PANEL_X = START_X + GRID_WIDTH + 80;
            const float PANEL_WIDTH = 600.0f;
            const float PANEL_Y = START_Y;
            const float PANEL_HEIGHT = 700.0f;
            

            sf::RectangleShape infoBg;
            infoBg.setSize(sf::Vector2f(PANEL_WIDTH, PANEL_HEIGHT));
            infoBg.setFillColor(sf::Color(20, 20, 40, 240));
            infoBg.setOutlineColor(sf::Color(255, 215, 0));
            infoBg.setOutlineThickness(3);
            infoBg.setPosition(sf::Vector2f(PANEL_X, PANEL_Y));
            window.draw(infoBg);
            

            bool isImplemented = (hoveredAchievement < TOTAL_ACHIEVEMENTS);
            bool isUnlocked = isImplemented ? saveData.achievements[hoveredAchievement] : false;
            

            AchievementInfo achInfo;
            if (isImplemented) {
                achInfo = getAchievementInfo(static_cast<Achievement>(hoveredAchievement), isUnlocked);
            } else {
                achInfo = achievements[hoveredAchievement];
            }
            

            float iconY = PANEL_Y + 80;
            if (isUnlocked) {

                sf::Text bigWon(menuFont, "W");
                bigWon.setCharacterSize(100);
                bigWon.setFillColor(sf::Color(100, 255, 100));
                bigWon.setStyle(sf::Text::Bold);
                sf::FloatRect bigWonBounds = bigWon.getLocalBounds();
                bigWon.setPosition(sf::Vector2f(PANEL_X + PANEL_WIDTH/2 - bigWonBounds.size.x/2, iconY - bigWonBounds.size.y/2 - 20));
                window.draw(bigWon);
            } else if (isImplemented) {

                sf::Text bigLocked(menuFont, "L");
                bigLocked.setCharacterSize(100);
                bigLocked.setFillColor(sf::Color(180, 180, 190));
                bigLocked.setStyle(sf::Text::Bold);
                sf::FloatRect bigLockedBounds = bigLocked.getLocalBounds();
                bigLocked.setPosition(sf::Vector2f(PANEL_X + PANEL_WIDTH/2 - bigLockedBounds.size.x/2, iconY - bigLockedBounds.size.y/2 - 20));
                window.draw(bigLocked);
            } else {

                sf::Text bigEmpty(menuFont, "X");
                bigEmpty.setCharacterSize(120);
                bigEmpty.setFillColor(sf::Color(80, 80, 85));
                bigEmpty.setStyle(sf::Text::Bold);
                sf::FloatRect bigEmptyBounds = bigEmpty.getLocalBounds();
                bigEmpty.setPosition(sf::Vector2f(PANEL_X + PANEL_WIDTH/2 - bigEmptyBounds.size.x/2, iconY - bigEmptyBounds.size.y/2 - 20));
                window.draw(bigEmpty);
            }
            

            sf::Text nameText(menuFont, achInfo.title);
            nameText.setCharacterSize(36);
            nameText.setFillColor(isUnlocked ? sf::Color(255, 215, 0) : sf::Color(120, 120, 120));
            nameText.setStyle(sf::Text::Bold);
            sf::FloatRect nameBounds = nameText.getLocalBounds();
            nameText.setPosition(sf::Vector2f(PANEL_X + PANEL_WIDTH/2 - nameBounds.size.x/2, PANEL_Y + 200));
            window.draw(nameText);
            

            sf::Text subtitleText(menuFont, achInfo.subtitle);
            subtitleText.setCharacterSize(22);
            subtitleText.setFillColor(isUnlocked ? sf::Color(150, 200, 255) : sf::Color(80, 80, 80));
            subtitleText.setStyle(sf::Text::Italic);
            sf::FloatRect subtitleBounds = subtitleText.getLocalBounds();
            subtitleText.setPosition(sf::Vector2f(PANEL_X + PANEL_WIDTH/2 - subtitleBounds.size.x/2, PANEL_Y + 250));
            window.draw(subtitleText);
            

            sf::Text descText(menuFont, achInfo.description);
            descText.setCharacterSize(26);
            descText.setFillColor(isUnlocked ? sf::Color(200, 200, 200) : sf::Color(100, 100, 100));
            sf::FloatRect descBounds = descText.getLocalBounds();
            descText.setPosition(sf::Vector2f(PANEL_X + PANEL_WIDTH/2 - descBounds.size.x/2, PANEL_Y + 300));
            window.draw(descText);
            

            std::string statusStr;
            sf::Color statusColor;
            if (isUnlocked) {
                statusStr = "UNLOCKED";
                statusColor = sf::Color(100, 255, 100);
            } else if (isImplemented) {
                statusStr = "LOCKED";
                statusColor = sf::Color(255, 100, 100);
            } else {
                statusStr = "NOT IMPLEMENTED";
                statusColor = sf::Color(120, 120, 120);
            }
            
            sf::Text statusText(menuFont, statusStr);
            statusText.setCharacterSize(28);
            statusText.setFillColor(statusColor);
            statusText.setStyle(sf::Text::Bold);
            sf::FloatRect statusBounds = statusText.getLocalBounds();
            statusText.setPosition(sf::Vector2f(PANEL_X + PANEL_WIDTH/2 - statusBounds.size.x/2, PANEL_Y + 360));
            window.draw(statusText);
        }
    }
}

void drawStatisticsScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData, bool debugMode) {
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    
    if (fontLoaded) {

        sf::Text titleText(titleFont, MenuConfig::ScreenTitles::STATISTICS);
        titleText.setCharacterSize(70);
        titleText.setFillColor(sf::Color(100, 200, 255));
        titleText.setStyle(sf::Text::Bold);
        titleText.setOutlineColor(sf::Color::Black);
        titleText.setOutlineThickness(4);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(centerX - titleBounds.size.x/2, 100));
        window.draw(titleText);
        

        sf::RectangleShape panel;
        panel.setSize(sf::Vector2f(1200, 700));
        panel.setFillColor(sf::Color(30, 30, 40, 200));
        panel.setOutlineColor(sf::Color(100, 200, 255));
        panel.setOutlineThickness(3);
        panel.setPosition(sf::Vector2f(centerX - 600, 230));
        window.draw(panel);
        

        float startY = 270;
        float lineSpacing = 70;
        

        auto formatTime = [](float seconds) -> std::string {
            int hours = static_cast<int>(seconds) / 3600;
            int minutes = (static_cast<int>(seconds) % 3600) / 60;
            int secs = static_cast<int>(seconds) % 60;
            
            if (hours > 0) {
                return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(secs) + "s";
            } else if (minutes > 0) {
                return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
            } else {
                return std::to_string(secs) + "s";
            }
        };
        

        auto formatNumber = [](int number) -> std::string {
            std::string str = std::to_string(number);
            int insertPosition = str.length() - 3;
            while (insertPosition > 0) {
                str.insert(insertPosition, ",");
                insertPosition -= 3;
            }
            return str;
        };
        

        struct Stat {
            std::string label;
            std::string value;
            sf::Color color;
        };
        

        int challengesCompleted = 0;
        int totalChallenges = 8;
        if (saveData.bestTimeChallengeTheForest > 0.0f) challengesCompleted++;
        if (saveData.bestTimeChallengeRandomness > 0.0f) challengesCompleted++;
        if (saveData.bestTimeChallengeNonStraight > 0.0f) challengesCompleted++;
        if (saveData.bestTimeChallengeOneRot > 0.0f) challengesCompleted++;
        if (saveData.bestTimeChallengeChristopherCurse > 0.0f) challengesCompleted++;
        if (saveData.bestTimeChallengeVanishing > 0.0f) challengesCompleted++;
        if (saveData.bestTimeChallengeAutoDrop > 0.0f) challengesCompleted++;
        if (saveData.bestTimeChallengeGravityFlip > 0.0f) challengesCompleted++;
        
        std::string challengesText = std::to_string(challengesCompleted) + "/" + std::to_string(totalChallenges);
        
        std::vector<Stat> stats = {
            {"Games Played", formatNumber(saveData.totalGamesPlayed), sf::Color(255, 200, 100)},
            {"Total Lines Cleared", formatNumber(saveData.totalLinesCleared), sf::Color(100, 255, 100)},
            {"Total Pieces Placed", formatNumber(saveData.totalPiecesPlaced), sf::Color(150, 150, 255)},
            {"Total Score", formatNumber(saveData.totalScore), sf::Color(255, 255, 100)},
            {"Challenges Completed", challengesText, sf::Color(255, 150, 255)},
            {"Max Combo", std::to_string(saveData.maxComboEver), sf::Color(255, 100, 255)},
            {"Bombs Used", formatNumber(saveData.totalBombsUsed), sf::Color(255, 100, 100)},
            {"Rotations", formatNumber(saveData.totalRotations), sf::Color(150, 255, 200)},
            {"Holds Used", formatNumber(saveData.totalHolds), sf::Color(200, 150, 255)},
            {"Perfect Clears", formatNumber(saveData.totalPerfectClears), sf::Color(255, 200, 50)},
            {"Total Playtime", formatTime(saveData.totalPlayTimeSeconds), sf::Color(100, 200, 255)}
        };
        

        int col1Count = (stats.size() + 1) / 2;
        float col1X = centerX - 500;
        float col2X = centerX + 50;
        
        for (size_t i = 0; i < stats.size(); ++i) {
            float x = (i < col1Count) ? col1X : col2X;
            float y = startY + ((i < col1Count) ? i : (i - col1Count)) * lineSpacing;
            

            sf::Text labelText(menuFont, stats[i].label + ":");
            labelText.setCharacterSize(32);
            labelText.setFillColor(sf::Color(200, 200, 200));
            labelText.setPosition(sf::Vector2f(x, y));
            window.draw(labelText);
            

            sf::Text valueText(menuFont, stats[i].value);
            valueText.setCharacterSize(36);
            valueText.setFillColor(stats[i].color);
            valueText.setStyle(sf::Text::Bold);
            valueText.setPosition(sf::Vector2f(x, y + 35));
            window.draw(valueText);
        }
        

    }
}


void drawBestScoresScreen(sf::RenderWindow& window, const sf::Font& titleFont, const sf::Font& menuFont, bool fontLoaded, const SaveData& saveData, bool debugMode) {
    if (!fontLoaded) return;
    
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;

    sf::Text titleText(titleFont, MenuConfig::ScreenTitles::BEST_SCORES);
    titleText.setCharacterSize(100);
    titleText.setFillColor(sf::Color(255, 215, 0));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(sf::Vector2f(centerX - 300, 80));
    window.draw(titleText);
    

    auto formatTime = [](float seconds) -> std::string {
        if (seconds <= 0.0f) return "-";
        int minutes = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        int millis = static_cast<int>((seconds - static_cast<int>(seconds)) * 100);
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d:%02d.%02d", minutes, secs, millis);
        return std::string(buffer);
    };
    

    auto formatScore = [](int score) -> std::string {
        if (score <= 0) return "-";
        std::string str = std::to_string(score);
        std::string formatted;
        int count = 0;
        for (int i = str.length() - 1; i >= 0; i--) {
            if (count > 0 && count % 3 == 0) formatted = "," + formatted;
            formatted = str[i] + formatted;
            count++;
        }
        return formatted;
    };
    

    float leftX = 550;
    float leftStartY = 220;
    float lineHeight = 45;
    

    float rightX = 1050;
    float rightStartY = 220;
    

    sf::Text classicHeader(menuFont, "=== CLASSIC ===");
    classicHeader.setCharacterSize(48);
    classicHeader.setFillColor(sf::Color(100, 200, 255));
    classicHeader.setStyle(sf::Text::Bold);
    classicHeader.setPosition(sf::Vector2f(leftX, leftStartY));
    window.draw(classicHeader);
    leftStartY += lineHeight + 10;
    
    struct ScoreEntry {
        std::string mode;
        std::string value1;
        std::string value2;
        sf::Color color;
    };
    
    std::vector<ScoreEntry> entries = {
        {"Normal", formatScore(saveData.highScoreClassicNormal) + " (Lv " + std::to_string(saveData.topScoresNormal[0].level) + ")", "", sf::Color(150, 255, 150)},
        {"Hard", formatScore(saveData.highScoreClassicHard) + " (Lv " + std::to_string(saveData.topScoresHard[0].level) + ")", "", sf::Color(255, 100, 100)}
    };
    
    for (const auto& entry : entries) {
        sf::Text modeText(menuFont, entry.mode);
        modeText.setCharacterSize(36);
        modeText.setFillColor(sf::Color::White);
        modeText.setPosition(sf::Vector2f(leftX + 30, leftStartY));
        window.draw(modeText);
        
        sf::Text value1Text(menuFont, entry.value1);
        value1Text.setCharacterSize(36);
        value1Text.setFillColor(entry.color);
        value1Text.setStyle(sf::Text::Bold);
        value1Text.setPosition(sf::Vector2f(leftX + 200, leftStartY));
        window.draw(value1Text);
        
        leftStartY += lineHeight;
    }
    

    leftStartY += 20;
    sf::Text sprintHeader(menuFont, "=== SPRINT ===");
    sprintHeader.setCharacterSize(48);
    sprintHeader.setFillColor(sf::Color(255, 150, 255));
    sprintHeader.setStyle(sf::Text::Bold);
    sprintHeader.setPosition(sf::Vector2f(leftX, leftStartY));
    window.draw(sprintHeader);
    leftStartY += lineHeight + 10;
    
    entries = {
        {"Sprint 1", formatTime(saveData.bestTimeSprint1), "", sf::Color(200, 255, 200)},
        {"Sprint 24", formatTime(saveData.bestTimeSprint24), "", sf::Color(200, 255, 200)},
        {"Sprint 48", formatTime(saveData.bestTimeSprint48), "", sf::Color(200, 255, 200)},
        {"Sprint 96", formatTime(saveData.bestTimeSprint96), "", sf::Color(200, 255, 200)}
    };
    
    for (const auto& entry : entries) {
        sf::Text modeText(menuFont, entry.mode);
        modeText.setCharacterSize(36);
        modeText.setFillColor(sf::Color::White);
        modeText.setPosition(sf::Vector2f(leftX + 30, leftStartY));
        window.draw(modeText);
        
        sf::Text value1Text(menuFont, entry.value1);
        value1Text.setCharacterSize(36);
        value1Text.setFillColor(entry.color);
        value1Text.setStyle(sf::Text::Bold);
        value1Text.setPosition(sf::Vector2f(leftX + 240, leftStartY));
        window.draw(value1Text);
        
        leftStartY += lineHeight;
    }
    

    sf::Text challengeHeader(menuFont, "=== CHALLENGES ===");
    challengeHeader.setCharacterSize(48);
    challengeHeader.setFillColor(sf::Color(255, 200, 100));
    challengeHeader.setStyle(sf::Text::Bold);
    challengeHeader.setPosition(sf::Vector2f(rightX, rightStartY));
    window.draw(challengeHeader);
    rightStartY += lineHeight + 10;
    
    entries = {
        {"The Forest", formatTime(saveData.bestTimeChallengeTheForest), "", sf::Color(150, 255, 150)},
        {"Randomness", formatTime(saveData.bestTimeChallengeRandomness), "", sf::Color(150, 255, 150)},
        {"Non-Straight", formatTime(saveData.bestTimeChallengeNonStraight), "", sf::Color(150, 255, 150)},
        {"One Rotation", formatTime(saveData.bestTimeChallengeOneRot), "", sf::Color(150, 255, 150)},
        {"The Curse", formatTime(saveData.bestTimeChallengeChristopherCurse), "", sf::Color(150, 255, 150)},
        {"Vanishing", formatTime(saveData.bestTimeChallengeVanishing), "", sf::Color(150, 255, 150)},
        {"Auto Drop", formatTime(saveData.bestTimeChallengeAutoDrop), "", sf::Color(150, 255, 150)},
        {"Gravity Flip", formatTime(saveData.bestTimeChallengeGravityFlip), "", sf::Color(150, 255, 150)},
        {"Petrify", formatTime(saveData.bestTimeChallengePetrify), "", sf::Color(150, 255, 150)}
    };
    
    for (const auto& entry : entries) {
        sf::Text modeText(menuFont, entry.mode);
        modeText.setCharacterSize(36);
        modeText.setFillColor(sf::Color::White);
        modeText.setPosition(sf::Vector2f(rightX + 30, rightStartY));
        window.draw(modeText);
        
        sf::Text value1Text(menuFont, entry.value1);
        value1Text.setCharacterSize(36);
        value1Text.setFillColor(entry.color);
        value1Text.setStyle(sf::Text::Bold);
        value1Text.setPosition(sf::Vector2f(rightX + 330, rightStartY));
        window.draw(value1Text);
        
        rightStartY += lineHeight;
    }
    

    sf::Text escText(menuFont, "Press ESC to go back");
    escText.setCharacterSize(32);
    escText.setFillColor(sf::Color(150, 150, 150));
    escText.setPosition(sf::Vector2f(centerX - 150, SCREEN_HEIGHT - 80));
    window.draw(escText);
}


void updateBackgroundPieces(std::vector<BackgroundPiece>& pieces, float deltaTime) {
    for (auto& piece : pieces) {
        piece.update(deltaTime);
    }
    
    pieces.erase(
        std::remove_if(pieces.begin(), pieces.end(),
            [](const BackgroundPiece& p) { return p.isOffScreen(); }),
        pieces.end()
    );
}


void drawBackgroundPieces(sf::RenderWindow& window, const std::vector<BackgroundPiece>& pieces, 
                         const std::map<TextureType, sf::Texture>& textures, bool useTextures) {
    for (const auto& piece : pieces) {
        PieceShape shape = getPieceShape(piece.type);
        
        sf::Transform transform;
        transform.translate(sf::Vector2f(piece.x, piece.y));
        transform.rotate(sf::degrees(piece.rotation));
        
        float cellSize = 32.0f;
        
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float localX = j * cellSize - (shape.width * cellSize) / 2.0f;
                    float localY = i * cellSize - (shape.height * cellSize) / 2.0f;
                    

                    sf::Color tintColor = shape.color;
                    tintColor.r = static_cast<std::uint8_t>(std::min(255.0f, tintColor.r * piece.brightness));
                    tintColor.g = static_cast<std::uint8_t>(std::min(255.0f, tintColor.g * piece.brightness));
                    tintColor.b = static_cast<std::uint8_t>(std::min(255.0f, tintColor.b * piece.brightness));
                    tintColor.a = static_cast<std::uint8_t>(piece.opacity * 255);


                    TextureType textureToUse = getTextureType(piece.type);
                    
                    if (useTextures && textures.find(textureToUse) != textures.end()) {
                        sf::Sprite sprite(textures.at(textureToUse));
                        sprite.setPosition(sf::Vector2f(localX, localY));
                        

                        if (piece.type == PieceType::A_Bomb) {

                            std::uint8_t brightnessValue = static_cast<std::uint8_t>(255.0f * piece.brightness);
                            sprite.setColor(sf::Color(brightnessValue, brightnessValue, brightnessValue, static_cast<std::uint8_t>(piece.opacity * 255)));
                        } else {
                            sprite.setColor(tintColor);
                        }
                        
                        sf::Vector2u textureSize = textures.at(textureToUse).getSize();
                        sprite.setScale(sf::Vector2f(cellSize / textureSize.x, cellSize / textureSize.y));
                        window.draw(sprite, transform);
                    } else {

                        sf::RectangleShape block(sf::Vector2f(cellSize - 2, cellSize - 2));
                        block.setPosition(sf::Vector2f(localX + 1, localY + 1));
                        block.setFillColor(tintColor);
                        window.draw(block, transform);
                    }
                }
            }
        }
    }
}


void drawBackgroundPiecesWithExplosions(sf::RenderWindow& window, const std::vector<BackgroundPiece>& pieces, 
                                        const std::vector<ExplosionEffect>& explosions,
                                        const std::map<TextureType, sf::Texture>& textures, bool useTextures) {

    for (const auto& piece : pieces) {
        PieceShape shape = getPieceShape(piece.type);
        

        for (const auto& explosion : explosions) {
            if (explosion.zDepth > 0 && explosion.zDepth < piece.y) {

                float worldX, worldY;
                if (explosion.x > 50 || explosion.y > 50) {
                    worldX = explosion.x;
                    worldY = explosion.y;
                } else {
                    continue;
                }
                
                sf::RectangleShape explosionBox;
                explosionBox.setSize(sf::Vector2f(32.0f, 32.0f));
                explosionBox.setOrigin(sf::Vector2f(16.0f, 16.0f));
                explosionBox.setPosition(sf::Vector2f(worldX + 16.0f, worldY + 16.0f));
                explosionBox.setRotation(sf::degrees(explosion.rotation));
                explosionBox.setFillColor(sf::Color::White);
                window.draw(explosionBox);
            }
        }
        

        sf::Transform transform;
        transform.translate(sf::Vector2f(piece.x, piece.y));
        transform.rotate(sf::degrees(piece.rotation));
        
        float cellSize = 32.0f;
        
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float localX = j * cellSize - (shape.width * cellSize) / 2.0f;
                    float localY = i * cellSize - (shape.height * cellSize) / 2.0f;
                    
                    sf::Color tintColor = shape.color;
                    tintColor.r = static_cast<std::uint8_t>(std::min(255.0f, tintColor.r * piece.brightness));
                    tintColor.g = static_cast<std::uint8_t>(std::min(255.0f, tintColor.g * piece.brightness));
                    tintColor.b = static_cast<std::uint8_t>(std::min(255.0f, tintColor.b * piece.brightness));
                    tintColor.a = static_cast<std::uint8_t>(piece.opacity * 255);


                    TextureType textureToUse = getTextureType(piece.type);
                    
                    if (useTextures && textures.find(textureToUse) != textures.end()) {
                        sf::Sprite sprite(textures.at(textureToUse));
                        sprite.setPosition(sf::Vector2f(localX, localY));
                        

                        if (piece.type == PieceType::A_Bomb) {

                            std::uint8_t brightnessValue = static_cast<std::uint8_t>(255.0f * piece.brightness);
                            sprite.setColor(sf::Color(brightnessValue, brightnessValue, brightnessValue, static_cast<std::uint8_t>(piece.opacity * 255)));
                        } else {
                            sprite.setColor(tintColor);
                        }
                        
                        sf::Vector2u textureSize = textures.at(textureToUse).getSize();
                        sprite.setScale(sf::Vector2f(cellSize / textureSize.x, cellSize / textureSize.y));
                        window.draw(sprite, transform);
                    } else {
                        sf::RectangleShape block(sf::Vector2f(cellSize - 2, cellSize - 2));
                        block.setPosition(sf::Vector2f(localX + 1, localY + 1));
                        block.setFillColor(tintColor);
                        window.draw(block, transform);
                    }
                }
            }
        }
    }
    

    for (const auto& explosion : explosions) {
        if (explosion.zDepth == 0 || explosion.zDepth >= (pieces.empty() ? 0 : pieces.back().y)) {
            float worldX, worldY;
            if (explosion.x > 50 || explosion.y > 50) {
                worldX = explosion.x;
                worldY = explosion.y;
            } else {
                continue;
            }
            
            sf::RectangleShape explosionBox;
            explosionBox.setSize(sf::Vector2f(32.0f, 32.0f));
            explosionBox.setOrigin(sf::Vector2f(16.0f, 16.0f));
            explosionBox.setPosition(sf::Vector2f(worldX + 16.0f, worldY + 16.0f));
            explosionBox.setRotation(sf::degrees(explosion.rotation));
            explosionBox.setFillColor(sf::Color::White);
            window.draw(explosionBox);
        }
    }
}


int checkBombClick(const std::vector<BackgroundPiece>& pieces, float mouseX, float mouseY) {

    for (int i = pieces.size() - 1; i >= 0; --i) {
        const auto& piece = pieces[i];
        

        if (piece.type != PieceType::A_Bomb) continue;
        
        PieceShape shape = getPieceShape(piece.type);
        float cellSize = 32.0f;
        


        float pieceWidth = shape.width * cellSize;
        float pieceHeight = shape.height * cellSize;
        float radius = std::max(pieceWidth, pieceHeight) / 2.0f;
        
        float dx = mouseX - piece.x;
        float dy = mouseY - piece.y;
        float distanceSquared = dx * dx + dy * dy;
        
        if (distanceSquared <= radius * radius) {
            return i;
        }
    }
    return -1;
}


void spawnBackgroundPiece(std::vector<BackgroundPiece>& pieces) {
    float x = static_cast<float>(rand() % static_cast<int>(SCREEN_WIDTH));
    float y = -100.0f;
    float speed = 16.0f + (rand() % 256);
    

    int rarityRoll = rand() % 100;
    PieceType randomType;
    if (rarityRoll < 1) {

        randomType = PieceType::A_Bomb;
    } else if (rarityRoll < 10) {

        PieceType hardTypes[] = {
            PieceType::I_Hard, PieceType::O_Hard, PieceType::T_Hard, PieceType::S_Hard, 
            PieceType::Z_Hard, PieceType::J_Hard, PieceType::L_Hard
        };
        randomType = hardTypes[rand() % 7];
    } else if (rarityRoll < 30) {

        PieceType mediumTypes[] = {
            PieceType::I_Medium, PieceType::O_Medium, PieceType::T_Medium, PieceType::S_Medium, 
            PieceType::Z_Medium, PieceType::J_Medium, PieceType::L_Medium
        };
        randomType = mediumTypes[rand() % 7];
    } else {

        PieceType basicTypes[] = {
            PieceType::I_Basic, PieceType::O_Basic, PieceType::T_Basic, PieceType::S_Basic, 
            PieceType::Z_Basic, PieceType::J_Basic, PieceType::L_Basic
        };
        randomType = basicTypes[rand() % 7];
    }
    
    pieces.emplace_back(x, y, speed, randomType);
}


void updateGameBackgroundPieces(std::vector<BackgroundPiece>& pieces, float deltaTime) {
    for (auto& piece : pieces) {

        piece.y += piece.speed * deltaTime * 0.4f;
        piece.rotation += piece.rotationSpeed * deltaTime * 0.3f;
    }
    
    pieces.erase(
        std::remove_if(pieces.begin(), pieces.end(), 
            [](const BackgroundPiece& p) { return p.isOffScreen(); }),
        pieces.end()
    );
}

void drawGameBackgroundPieces(sf::RenderWindow& window, const std::vector<BackgroundPiece>& pieces, const std::map<TextureType, sf::Texture>& textures, bool useTextures, const sf::Color& pieceColor) {
    for (const auto& piece : pieces) {
        PieceShape shape = getPieceShape(piece.type);
        

        float scale = piece.brightness / 3.0f;
        float cellSize = 32.0f * scale;
        
        sf::Transform transform;
        transform.translate(sf::Vector2f(piece.x, piece.y));
        transform.rotate(sf::degrees(piece.rotation));
        
        for (int i = 0; i < shape.height; ++i) {
            for (int j = 0; j < shape.width; ++j) {
                if (shape.blocks[i][j]) {
                    float localX = j * cellSize - (shape.width * cellSize) / 2.0f;
                    float localY = i * cellSize - (shape.height * cellSize) / 2.0f;
                    

                    sf::Color tintColor(pieceColor.r, pieceColor.g, pieceColor.b, static_cast<std::uint8_t>(piece.opacity * 255));

                    TextureType textureToUse = getTextureType(piece.type);
                    
                    if (useTextures && textures.find(textureToUse) != textures.end()) {
                        sf::Sprite sprite(textures.at(textureToUse));
                        sprite.setPosition(sf::Vector2f(localX, localY));
                        sprite.setColor(tintColor);
                        sf::Vector2u textureSize = textures.at(textureToUse).getSize();
                        sprite.setScale(sf::Vector2f(cellSize / textureSize.x, cellSize / textureSize.y));
                        window.draw(sprite, transform);
                    } else {
                        sf::RectangleShape block(sf::Vector2f(cellSize - 2, cellSize - 2));
                        block.setPosition(sf::Vector2f(localX + 1, localY + 1));
                        block.setFillColor(tintColor);
                        window.draw(block, transform);
                    }
                }
            }
        }
    }
}

void spawnGameBackgroundPiece(std::vector<BackgroundPiece>& pieces) {
    float x = static_cast<float>(rand() % static_cast<int>(SCREEN_WIDTH));
    float y = -150.0f;

    float speed = 40.0f + (rand() % 60);
    

    PieceType basicTypes[] = {
        PieceType::I_Basic, PieceType::O_Basic, PieceType::T_Basic, PieceType::S_Basic, 
        PieceType::Z_Basic, PieceType::J_Basic, PieceType::L_Basic
    };
    PieceType randomType = basicTypes[rand() % 7];
    
    pieces.emplace_back(x, y, speed, randomType);
    

    pieces.back().brightness = 1.0f + (rand() % 5);

    pieces.back().opacity = 0.08f + (rand() % 12) / 100.0f;
}


