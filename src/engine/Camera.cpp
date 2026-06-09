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

SDL_FPoint Camera::toCameraSpace(SDL_FPoint point) const {
    SDL_FPoint cameraPoint;
    const float halfWidth = rect.w / 2;
    const float halfHeight = rect.h / 2;
    cameraPoint.x = (point.x - (rect.x + halfWidth)) / halfWidth;
    cameraPoint.y = (point.y - (rect.y + halfHeight)) / halfHeight;
    return cameraPoint;
}

RenderContext Camera::modifyRenderContext(RenderContext ctx) {
    ctx.camera = this;
    return ctx;
}
