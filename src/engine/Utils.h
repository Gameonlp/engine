//
// Created by dev on 4/15/26.
//

#ifndef ENGINE_UTILS_H
#define ENGINE_UTILS_H
#include <string>

#include "SDL3/SDL_render.h"


class GameObject;
struct State;

class Utils {
public:
    static void globalPosition(SDL_FPoint &position, GameObject *object);

    template<class T>
    static T square(T toSquare);

    static void rect(SDL_FRect &rect, SDL_FPoint pos, SDL_FPoint size);
    static void rect(SDL_FRect &rect, SDL_FPoint pos, float w, float h);
};


#endif //ENGINE_UTILS_H