//
// Created by dev on 4/15/26.
//

#ifndef MYENGINE_UTILS_H
#define MYENGINE_UTILS_H
#include <string>

#include "RenderContext.h"
#include "SDL3/SDL_render.h"


class GameObject;
struct State;

class Utils {
public:
    static void globalPosition(SDL_FPoint &position, GameObject *object);
    static void viewPosition(SDL_FPoint &position, GameObject *object, RenderContext context);

    template<class T>
    static T square(T toSquare);

    static bool rect_tri_intersect(SDL_FRect rect, SDL_FPoint a, SDL_FPoint b, SDL_FPoint c);
    static void rect(SDL_FRect &rect, SDL_FPoint pos, SDL_FPoint size);
    static void rect(SDL_FRect &rect, SDL_FPoint pos, float w, float h);
    static float dot(SDL_FPoint a, SDL_FPoint b);
    static SDL_FColor premultiply(SDL_FColor color);
    static SDL_FPoint rotateAroundOrigin(SDL_FPoint vertex, float angleDegrees);
};


#endif //ENGINE_UTILS_H