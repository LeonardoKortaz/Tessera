#include "piece_utils.h"

PieceShape getPieceShape(PieceType type) {
    PieceShape shape;
    switch(type){
        case PieceType::I_Basic:
            shape.blocks = {{false, false, false, false},{true, true, true, true},{false, false, false, false},{false, false, false, false}};
            shape.color = sf::Color(69, 255, 112);
            break;
        case PieceType::I_Medium:
            shape.blocks = {{false, false, false, false, false},{false, false, false, false, false},{true, true, true, true, true},{false, false, false, false, false},{false, false, false, false, false}};
            shape.color = sf::Color(69, 255, 112);
            break;
        case PieceType::I_Hard:
            shape.blocks = {{false, false, false, false, false},{false, false, false, false, false},{true, true, true, true, true},{false, false, true, false, false},{false, false, false, false, false}};
            shape.color = sf::Color(69, 255, 112);
            break;

        case PieceType::T_Basic:
            shape.blocks = {{false, true, false},{true, true, true},{false, false, false}};
            shape.color = sf::Color(255, 0, 80);
            break;
        case PieceType::T_Medium:
            shape.blocks = {{false, true, false},{false, true, false},{true, true, true}};
            shape.color = sf::Color(255, 0, 80);
            break;
        case PieceType::T_Hard:
            shape.blocks = {{false, true, false},{false, true, false},{true, true, true},{false, true, false},{false, false, false}};
            shape.color = sf::Color(255, 0, 80);
            break;

        case PieceType::L_Basic:
            shape.blocks = {{true, false, false},{true, true, true},{false, false, false}};
            shape.color = sf::Color(255, 249, 40);
            break;
        case PieceType::L_Medium:
            shape.blocks = {{false, false, false, false},{true, false, false, false},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(255, 249, 40);
            break;
        case PieceType::L_Hard:
            shape.blocks = {{false, false, false, false},{true, true, false, false},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(255, 249, 40);
            break;

        case PieceType::J_Basic:
            shape.blocks = {{false, false, true},{true, true, true},{false, false, false}};
            shape.color = sf::Color(255, 142, 0);
            break;
        case PieceType::J_Medium:
            shape.blocks = {{false, false, false, true},{true, true, true, true}};
            shape.color = sf::Color(255, 142, 0);
            break;
        case PieceType::J_Hard:
            shape.blocks = {{false, false, false, false},{false, false, true, true},{true, true, true, true},{false, false, false, false}};
            shape.color = sf::Color(255, 142, 0);
            break;

        case PieceType::O_Basic:
            shape.blocks = {{true, true},{true, true}};
            shape.color = sf::Color(90, 30, 10);
            break;
        case PieceType::O_Medium:
            shape.blocks = {{false, false, false},{true, true, true},{true, true, true}};
            shape.color = sf::Color(90, 30, 10);
            break;
        case PieceType::O_Hard:
            shape.blocks = {{false, true, true},{true, true, true},{true, true, false}};
            shape.color = sf::Color(90, 30, 10);
            break;

        case PieceType::S_Basic:
            shape.blocks = {{false, true, true},{true, true, false},{false, false, false}};
            shape.color = sf::Color(153, 15, 248);
            break;
        case PieceType::S_Medium:
            shape.blocks = {{false, true, false},{false, true, true},{true, true, false}};
            shape.color = sf::Color(153, 15, 248);
            break;
        case PieceType::S_Hard:
            shape.blocks = {{false, true, false},{false, true, true},{true, true, false},{false, true, false}};
            shape.color = sf::Color(153, 15, 248);
            break;

        case PieceType::Z_Basic:
            shape.blocks = {{true, true, false},{false, true, true},{false, false, false}};
            shape.color = sf::Color(0, 115, 255);
            break;
        case PieceType::Z_Medium:
            shape.blocks = {{false, true, false},{true, true, false},{false, true, true}};
            shape.color = sf::Color(0, 115, 255);
            break;
        case PieceType::Z_Hard:
            shape.blocks = {{false, true, false},{true, true, false},{false, true, true},{false, true, false}};
            shape.color = sf::Color(0, 115, 255);
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
