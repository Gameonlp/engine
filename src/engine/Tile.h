//
// Created by dev on 5/11/26.
//

#ifndef MYENGINE_TILE_H
#define MYENGINE_TILE_H
#include "TileData.h"
#include "TileSet.h"

struct Tile {
    uint32_t tileIdInTileset;
    const TileSet* tileSet;
    bool flipHorizontal;
    bool flipVertical;
    bool flipDiagonal;
};

#endif //MYENGINE_TILE_H