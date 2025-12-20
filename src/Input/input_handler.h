#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SFML/Window/Keyboard.hpp>
#include "types.h"





struct DASConfig {
    float delay = 0.2f;
    float repeat = 0.04f;
};





struct InputState {

    bool leftPressed = false;
    bool rightPressed = false;
    bool downPressed = false;
    

    float leftHoldTime = 0.0f;
    float rightHoldTime = 0.0f;
    float dasTimer = 0.0f;
    

    bool fastFallActive = false;
    
    void reset() {
        leftPressed = false;
        rightPressed = false;
        downPressed = false;
        leftHoldTime = 0.0f;
        rightHoldTime = 0.0f;
        dasTimer = 0.0f;
        fastFallActive = false;
    }
    
    void update(float deltaTime, const DASConfig& config) {

        if (leftPressed) {
            leftHoldTime += deltaTime;
        } else {
            leftHoldTime = 0.0f;
        }
        
        if (rightPressed) {
            rightHoldTime += deltaTime;
        } else {
            rightHoldTime = 0.0f;
        }
    }
};








inline std::string getKeyName(sf::Keyboard::Key key) {
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
        case sf::Keyboard::Key::Num0: return "0";
        case sf::Keyboard::Key::Num1: return "1";
        case sf::Keyboard::Key::Num2: return "2";
        case sf::Keyboard::Key::Num3: return "3";
        case sf::Keyboard::Key::Num4: return "4";
        case sf::Keyboard::Key::Num5: return "5";
        case sf::Keyboard::Key::Num6: return "6";
        case sf::Keyboard::Key::Num7: return "7";
        case sf::Keyboard::Key::Num8: return "8";
        case sf::Keyboard::Key::Num9: return "9";
        case sf::Keyboard::Key::Space: return "SPACE";
        case sf::Keyboard::Key::Enter: return "ENTER";
        case sf::Keyboard::Key::Escape: return "ESC";
        case sf::Keyboard::Key::Left: return "LEFT";
        case sf::Keyboard::Key::Right: return "RIGHT";
        case sf::Keyboard::Key::Up: return "UP";
        case sf::Keyboard::Key::Down: return "DOWN";
        case sf::Keyboard::Key::LShift: return "L-SHIFT";
        case sf::Keyboard::Key::RShift: return "R-SHIFT";
        case sf::Keyboard::Key::LControl: return "L-CTRL";
        case sf::Keyboard::Key::RControl: return "R-CTRL";
        case sf::Keyboard::Key::LAlt: return "L-ALT";
        case sf::Keyboard::Key::RAlt: return "R-ALT";
        case sf::Keyboard::Key::Tab: return "TAB";
        case sf::Keyboard::Key::Backspace: return "BACKSPACE";
        default: return "?";
    }
}

#endif
