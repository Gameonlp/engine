//
// Created by dev on 5/17/26.
//

#ifndef MYENGINE_COMMANDBUFFER_H
#define MYENGINE_COMMANDBUFFER_H
#include "SDL3/SDL_gpu.h"
#include "utility/Movable.h"

#ifdef DEBUG_BUILD
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_log.h"
#endif

struct CommandBuffer {

    Movable<SDL_GPUCommandBuffer *>cmd = nullptr;
    bool submitted = false;

#ifdef DEBUG_BUILD
    uint64_t creationTime = 0;
#endif

    explicit CommandBuffer(SDL_GPUDevice *device) {
        cmd = SDL_AcquireGPUCommandBuffer(device);
#ifdef DEBUG_BUILD
        creationTime = SDL_GetTicksNS();
#endif
    }

    [[nodiscard]] SDL_GPUFence *submitWithFence() {
        if (cmd) {
            submitted = true;
#ifdef DEBUG_BUILD
            uint64_t start = SDL_GetTicksNS();
            SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
            ProfileSubmission(start);
            return fence;
#else
            return SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
#endif
        }
        return nullptr;
    }

    void submit() {
        if (cmd) {
            submitted = true;
#ifdef DEBUG_BUILD
            uint64_t start = SDL_GetTicksNS();
            SDL_SubmitGPUCommandBuffer(cmd);
            ProfileSubmission(start);
#else
            SDL_SubmitGPUCommandBuffer(cmd);
#endif
        }
    }

    ~CommandBuffer() {
        if (!submitted) {
            submit();
        }
    }

private:
#ifdef DEBUG_BUILD
    void ProfileSubmission(uint64_t startTime) const {
        uint64_t now = SDL_GetTicksNS();
        uint64_t submissionDuration = now - startTime;
        uint64_t totalLifetime = now - creationTime;

        constexpr uint64_t STALL_THRESHOLD_NS = 1000000; // 1ms

        if (submissionDuration > STALL_THRESHOLD_NS) {
            SDL_LogWarn(SDL_LOG_CATEGORY_RENDER,
                "PERF WARN: CommandBuffer submission stalled for %.2f ms!\n"
                "  > Total CPU Recording Time: %.2f ms\n"
                "  > High likelihood of missing 'cycle=true' on a modified resource.",
                (double)submissionDuration / 1000000.0,
                (double)totalLifetime / 1000000.0);
        }
    }
#endif
};

#endif //MYENGINE_COMMANDBUFFER_H
