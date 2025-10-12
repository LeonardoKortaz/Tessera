#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H

#include "types.h"
#include <string>
#include <SFML/Graphics/Color.hpp>


struct TextureInfo {
    TextureType type;
    std::string filename;
    sf::Color fallbackColor;
};

#endif
