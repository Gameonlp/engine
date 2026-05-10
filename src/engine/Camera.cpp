//
// Created by dev on 5/10/26.
//

#include "Camera.h"
#include "SDLExtensions.h"

void Camera::move(SDL_FPoint delta) {
    position() += delta;
}

RenderContext Camera::modifyRenderContext(RenderContext ctx) {
    ctx.camera = this;
    return ctx;
}
