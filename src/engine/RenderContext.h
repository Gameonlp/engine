//
// Created by dev on 5/3/26.
//

#ifndef MYENGINE_RENDERCONTEXT_H
#define MYENGINE_RENDERCONTEXT_H
#include "SDL3/SDL_render.h"

class Camera;

struct RenderContext {
    SDL_Renderer* renderer;
    Camera* camera;
};

#endif //MYENGINE_RENDERCONTEXT_H