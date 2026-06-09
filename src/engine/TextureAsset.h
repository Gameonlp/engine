//
// Created by dev on 5/16/26.
//

#ifndef MYENGINE_TEXTUREASSET_H
#define MYENGINE_TEXTUREASSET_H
#include <variant>

#include "SDL3/SDL_gpu.h"
struct TextureAsset {
    bool isReady;
    SDL_GPUTexture *texture;
    float w, h, d;
    SDL_GPUDevice *device;

    ~TextureAsset() {
        if (device && texture && isReady) {
            SDL_ReleaseGPUTexture(device, texture);
        }
    }
};

#endif //MYENGINE_TEXTUREASSET_H