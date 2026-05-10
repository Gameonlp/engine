//
// Created by dev on 5/10/26.
//

#ifndef MYENGINE_CAMERA_H
#define MYENGINE_CAMERA_H
#include "GameObject.h"
#include "SDL3/SDL_rect.h"

class Camera : public GameObject {
public:
    Camera(SDL_FPoint position, SDL_FPoint size) : GameObject(position), size(size){};

    void move(SDL_FPoint delta);

    RenderContext modifyRenderContext(RenderContext ctx) override;
private:
    SDL_FPoint size;
};


#endif //MYENGINE_CAMERA_H