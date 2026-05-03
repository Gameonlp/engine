//
// Created by dev on 4/24/26.
//

#ifndef MYENGINE_STATE_H
#define MYENGINE_STATE_H
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

class Game;
class Root;

struct State {
    Root *root;
    Game *game;
};

#endif //ENGINE_STATE_H