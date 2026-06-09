//
// Created by dev on 6/2/26.
//

#ifndef MYENGINE_GPUSAMPLER_H
#define MYENGINE_GPUSAMPLER_H
#include "SDL3/SDL_gpu.h"
#include "utility/Movable.h"

struct GPUSampler {
    SDL_GPUSamplerCreateInfo info = {};
    Movable<SDL_GPUSampler *>sampler = nullptr;
    SDL_GPUDevice *device = nullptr;

    GPUSampler(const SDL_GPUSamplerCreateInfo &info) {
        this->info = info;
        sampler = nullptr;
    }

    void create(SDL_GPUDevice *device) {
        this->device = device;
        sampler = SDL_CreateGPUSampler(device, &info);
    }

    GPUSampler(GPUSampler &&) noexcept = default;
    GPUSampler& operator=(GPUSampler&& other) noexcept {
        if (this != &other) {
            if (sampler && device) SDL_ReleaseGPUSampler(device, sampler);
            sampler = std::move(other.sampler);
            device = other.device;
            other.device = nullptr;
        }
        return *this;
    }

    ~GPUSampler() {
        if (sampler && device) {
            SDL_ReleaseGPUSampler(device, sampler);
        }
    }
};

#endif //MYENGINE_GPUSAMPLER_H