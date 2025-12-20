#ifndef PIECE_UTILS_H
#define PIECE_UTILS_H

#include "types.h"
#include <string>


PieceShape getPieceShape(PieceType type);


TextureType getTextureType(PieceType pieceType);


std::string pieceTypeToString(PieceType type);

#endif
