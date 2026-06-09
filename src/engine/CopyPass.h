//
// Created by dev on 5/30/26.
//

#ifndef MYENGINE_COPYPASS_H
#define MYENGINE_COPYPASS_H
#include "SDL3/SDL_gpu.h"
#include "utility/Movable.h"

struct CopyPass {
    Movable<SDL_GPUCopyPass *>pass = nullptr;

    explicit CopyPass(SDL_GPUCommandBuffer *buffer) {
        pass = SDL_BeginGPUCopyPass(buffer);
    }

    ~CopyPass() {
        if (pass) {
            SDL_EndGPUCopyPass(pass);
        }
    }
};
#endif //MYENGINE_COPYPASS_H