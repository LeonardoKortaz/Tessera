#include "piece_utils.h"

PieceShape getPieceShape(PieceType type) {
    PieceShape shape;
    switch(type){
        case PieceType::I_Basic:
            shape.blocks = {{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color::Cyan;
            break;
        case PieceType::I_Medium:
            shape.blocks = {{false, false, false, false, false},{true, true, true, true, true},{false, false, false, false, false},{false, false, false, false, false}};
            shape.color = sf::Color(0, 200, 255);
            break;
        case PieceType::I_Hard:
            shape.blocks = {{false, false, false, false, false},{true, true, true, true, true},{false, false, true, false, false},{false, false, false, false, false}};
            shape.color = sf::Color(0, 150, 255);
            break;
        case PieceType::T_Basic:
            shape.blocks = {{false, true, false},{true, true, true},{false, false, false}};
            shape.color = sf::Color::Magenta;
            break;
        case PieceType::T_Medium:
            shape.blocks = {{false, true, false},{false, true, false},{true, true, true}};
            shape.color = sf::Color(255, 100, 255);
            break;
        case PieceType::T_Hard:
            shape.blocks = {{false, true, false},{false, true, false},{true, true, true},{false, true, false},{false, false, false}};
            shape.color = sf::Color(200, 50, 200);
            break;

        case PieceType::L_Basic:
            shape.blocks = {{true, false, false},{true, true, true},{false, false, false}};
            shape.color = sf::Color(255, 165, 0);
            break;
        case PieceType::L_Medium:
            shape.blocks = {{false, false, false, false},{true, false, false, false},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(255, 200, 50);
            break;
        case PieceType::L_Hard:
            shape.blocks = {{false, false, false, false},{true, true, false, false},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(200, 120, 0);
            break;

        case PieceType::J_Basic:
            shape.blocks = {{false, false, true},{true, true, true},{false, false, false}};
            shape.color = sf::Color::Blue;
            break;
        case PieceType::J_Medium:
            shape.blocks = {{false, false, false, true},{true, true, true, true}};
            shape.color = sf::Color(100, 100, 255);
            break;
        case PieceType::J_Hard:
            shape.blocks = {{false, false, false, false},{false, false, true, true},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(0, 0, 200);
            break;

        case PieceType::O_Basic:
            shape.blocks = {{true, true},{true, true}};
            shape.color = sf::Color::Yellow;
            break;
        case PieceType::O_Medium:
            shape.blocks = {{false, false, false},{true, true, true},{true, true, true}};
            shape.color = sf::Color(255, 255, 100);
            break;
        case PieceType::O_Hard:
            shape.blocks = {{false, true, true},{true, true, true},{true, true, false}};
            shape.color = sf::Color(200, 200, 0);
            break;

        case PieceType::S_Basic:
            shape.blocks = {{false, true, true},{true, true, false}};
            shape.color = sf::Color::Green;
            break;
        case PieceType::S_Medium:
            shape.blocks = {{false, true, false},{false, true, true},{true, true, false}};
            shape.color = sf::Color(100, 255, 100);
            break;
        case PieceType::S_Hard:
            shape.blocks = {{false, true, false},{false, true, true},{true, true, false},{false, true, false}};
            shape.color = sf::Color(0, 200, 0);
            break;

        case PieceType::Z_Basic:
            shape.blocks = {{true, true, false},{false, true, true}};
            shape.color = sf::Color::Red;
            break;
        case PieceType::Z_Medium:
            shape.blocks = {{false, true, false},{true, true, false},{false, true, true}};
            shape.color = sf::Color(255, 100, 100);
            break;
        case PieceType::Z_Hard:
            shape.blocks = {{false, true, false},{true, true, false},{false, true, true},{false, true, false}};
            shape.color = sf::Color(200, 0, 0);
            break;

        case PieceType::A_Bomb:
            shape.blocks = {{true}};
            shape.color = sf::Color(255, 255, 255);
            break;
    }
    shape.height = static_cast<int>(shape.blocks.size());
    shape.width = (shape.height > 0) ? static_cast<int>(shape.blocks[0].size()) : 0;
    return shape;
}

TextureType getTextureType(PieceType pieceType) {
    switch(pieceType) {
        case PieceType::I_Basic:
        case PieceType::T_Basic:
        case PieceType::L_Basic:
        case PieceType::J_Basic:
        case PieceType::O_Basic:
        case PieceType::S_Basic:
        case PieceType::Z_Basic:
            return TextureType::GenericBlock;

        case PieceType::I_Medium:
        case PieceType::T_Medium:
        case PieceType::L_Medium:
        case PieceType::J_Medium:
        case PieceType::O_Medium:
        case PieceType::S_Medium:
        case PieceType::Z_Medium:
            return TextureType::MediumBlock;

        case PieceType::I_Hard:
        case PieceType::T_Hard:
        case PieceType::L_Hard:
        case PieceType::J_Hard:
        case PieceType::O_Hard:
        case PieceType::S_Hard:
        case PieceType::Z_Hard:
            return TextureType::HardBlock;
        
        case PieceType::A_Bomb:
            return TextureType::A_Bomb;

        default:
            return TextureType::Empty;
    }
}

std::string pieceTypeToString(PieceType type) {
    switch(type) {
        case PieceType::I_Basic: return "I";
        case PieceType::T_Basic: return "T";
        case PieceType::L_Basic: return "L";
        case PieceType::J_Basic: return "J";
        case PieceType::O_Basic: return "O";
        case PieceType::S_Basic: return "S";
        case PieceType::Z_Basic: return "Z";

        case PieceType::I_Medium: return "I++";
        case PieceType::T_Medium: return "T++";
        case PieceType::L_Medium: return "L++";
        case PieceType::J_Medium: return "J++";
        case PieceType::O_Medium: return "O++";
        case PieceType::S_Medium: return "S++";
        case PieceType::Z_Medium: return "Z++";

        case PieceType::I_Hard: return "I#";
        case PieceType::T_Hard: return "T#";
        case PieceType::L_Hard: return "L#";
        case PieceType::J_Hard: return "J#";
        case PieceType::O_Hard: return "O#";
        case PieceType::S_Hard: return "S#";
        case PieceType::Z_Hard: return "Z#";
        
        default: return "Unknown";
    }
}
