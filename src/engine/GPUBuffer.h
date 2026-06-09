//
// Created by dev on 5/25/26.
//

#ifndef MYENGINE_GPUBUFFER_H
#define MYENGINE_GPUBUFFER_H
#include "SDL3/SDL_gpu.h"
#include "utility/Movable.h"

struct GPUBuffer {
    Movable<SDL_GPUBuffer *>buffer = nullptr;
    SDL_GPUDevice *device = nullptr;
    size_t size = 0;

    explicit GPUBuffer(SDL_GPUDevice *device, const SDL_GPUBufferCreateInfo *info) {
        this->device = device;
        buffer = SDL_CreateGPUBuffer(device, info);
        size = info->size;
    }

    [[nodiscard]] bool valid() const {
        return buffer && device;
    }

    void upload(SDL_GPUCopyPass *pass, const SDL_GPUTransferBufferLocation *location, const Uint32 size, const Uint32 offset = 0, const bool cycle = false) const {
        if (valid()) {
            const SDL_GPUBufferRegion region{.buffer = buffer, .offset = offset, .size = size};
            SDL_UploadToGPUBuffer(pass, location, &region, cycle);
        }
    }

    GPUBuffer(GPUBuffer&&) noexcept = default;
    GPUBuffer& operator=(GPUBuffer&& other) noexcept {
        if (this != &other) {
            if (valid()) SDL_ReleaseGPUBuffer(device, buffer);
            buffer = std::move(other.buffer);
            device = other.device;
            other.device = nullptr;
        }
        return *this;
    }

    ~GPUBuffer() {
        if (valid()) {
            SDL_ReleaseGPUBuffer(device, buffer);
        }
    }
};

#endif //MYENGINE_GPUBUFFER_H