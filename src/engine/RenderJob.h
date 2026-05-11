//
// Created by dev on 5/11/26.
//

#ifndef MYENGINE_RENDERJOB_H
#define MYENGINE_RENDERJOB_H
#include "RenderContext.h"

class GameObject;

struct RenderJob {
    GameObject* gameObject;
    RenderContext renderContext;
    int zIndex;
};

#endif //MYENGINE_RENDERJOB_H