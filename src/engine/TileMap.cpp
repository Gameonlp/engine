//
// Created by dev on 5/11/26.
//

#include "TileMap.h"

constexpr uint32_t FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
constexpr uint32_t FLIPPED_VERTICALLY_FLAG   = 0x40000000;
constexpr uint32_t FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

constexpr uint32_t TILE_ID_MASK              = 0x1FFFFFFF;
