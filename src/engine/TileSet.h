//
// Created by dev on 5/11/26.
//

#ifndef MYENGINE_TILESET_H
#define MYENGINE_TILESET_H
#include <memory>
#include <vector>

#include "TileData.h"
#include "SDL3/SDL_render.h"

struct TileSet {
    std::shared_ptr<SDL_Texture> texture;
    std::vector<TileData> tiles;
};

#endif //MYENGINE_TILESET_H