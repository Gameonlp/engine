//
// Created by dev on 5/11/26.
//

#ifndef MYENGINE_TILEDATA_H
#define MYENGINE_TILEDATA_H
#include "SDL3/SDL_rect.h"

struct TileData {
    SDL_FRect atlasRegion;
    bool hasExtraData;
};
#endif //MYENGINE_TILEDATA_H