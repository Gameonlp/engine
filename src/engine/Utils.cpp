//
// Created by dev on 4/15/26.
//

#include "Utils.h"
#include "SDLExtensions.h"
#include "GameObject.h"

void Utils::globalPosition(SDL_FPoint &position, GameObject *object) {
    position.x = 0;
    position.y = 0;

    const GameObject *current = object;
    while (current) {
        const SDL_FPoint curPos = current->getPosition();
        position += curPos;
        current = current->getParent();
    }
}

template <typename T>
T Utils::square(T toSquare) {
    return toSquare * toSquare;
}

void Utils::rect(SDL_FRect &rect, const SDL_FPoint pos, const SDL_FPoint size) {
    rect.x = pos.x;
    rect.y = pos.y;
    rect.w = size.x;
    rect.h = size.y;
}

void Utils::rect(SDL_FRect &rect, const SDL_FPoint pos, const float w, const float h) {
    rect.x = pos.x;
    rect.y = pos.y;
    rect.w = w;
    rect.h = h;
}
