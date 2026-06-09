//
// Created by dev on 5/3/26.
//

#ifndef MYENGINE_RENDERCONTEXT_H
#define MYENGINE_RENDERCONTEXT_H
class Renderer;
class Camera;

struct RenderContext {
    Renderer* renderer;
    Camera* camera;
    int zIndex;
};

#endif //MYENGINE_RENDERCONTEXT_H