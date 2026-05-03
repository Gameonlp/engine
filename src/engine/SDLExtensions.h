//
// Created by dev on 4/19/26.
//

#ifndef MYENGINE_SDLEXTENSIONS_H
#define MYENGINE_SDLEXTENSIONS_H

#include "SDL3/SDL_rect.h"

inline SDL_FPoint operator+(const SDL_FPoint& a, const SDL_FPoint& b) {
    return SDL_FPoint{a.x + b.x, a.y + b.y};
}

inline SDL_FPoint& operator+=(SDL_FPoint& a, const SDL_FPoint& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline SDL_FPoint operator-(const SDL_FPoint& a, const SDL_FPoint& b) {
    return SDL_FPoint{a.x - b.x, a.y - b.y};
}

inline SDL_FPoint operator*(const SDL_FPoint& a, const float& b) {
    return SDL_FPoint{a.x * b, a.y * b};
}

#endif //ENGINE_SDLEXTENSIONS_H