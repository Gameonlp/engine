//
// Created by dev on 5/10/26.
//

#ifndef MYENGINE_CAMERA_H
#define MYENGINE_CAMERA_H
#include "GameObject.h"
#include "SDL3/SDL_rect.h"

class Camera : public GameObject {
public:
    Camera(SDL_FRect rect) : GameObject({0, 0}), rect(rect) {};

    void move(SDL_FPoint delta);

    [[nodiscard]] SDL_FPoint cameraPosition() const;

    [[nodiscard]] SDL_FRect cameraRect() const;

    RenderContext modifyRenderContext(RenderContext ctx) override;
private:
    SDL_FRect rect;
};


#endif //MYENGINE_CAMERA_H