//
// Created by dev on 5/10/26.
//

#include "Camera.h"
#include "SDLExtensions.h"

void Camera::move(SDL_FPoint delta) {
    rect.x += delta.x;
    rect.y += delta.y;
}

SDL_FPoint Camera::cameraPosition() const {
    return {rect.x, rect.y};
}

SDL_FRect Camera::cameraRect() const {
    return rect;
}

RenderContext Camera::modifyRenderContext(RenderContext ctx) {
    ctx.camera = this;
    return ctx;
}
