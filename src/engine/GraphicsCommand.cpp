//
// Created by dev on 5/26/26.
//
#include "GraphicsCommand.h"

#include <ranges>

void BindPipeline::apply(const ApplyArgs &applyArgs) const {
    SDL_BindGPUGraphicsPipeline(applyArgs.pass, pipeline);
}

void PushUniform::apply(const ApplyArgs &applyArgs) const {
    void (*function)(SDL_GPUCommandBuffer *, Uint32, const void *, Uint32) = nullptr;
    switch (kind) {
        case VERTEX:
            function = &SDL_PushGPUVertexUniformData;
            break;
        case FRAG:
            function = &SDL_PushGPUFragmentUniformData;
            break;
    }
    if (function) {
        function(applyArgs.cmd, slot, data.data(), data.size());
    }
}

void BindVertexBuffers::apply(const ApplyArgs &applyArgs) const {
    std::vector<SDL_GPUBufferBinding> bindings;
    bindings.reserve(bindingData.size());
    for (const auto &[tag, offset] : bindingData) {
        if (auto it = applyArgs.gpuBuffers->find(tag); it != applyArgs.gpuBuffers->end()) {
            bindings.emplace_back(it->second.buffer, offset);
        }
    }
    SDL_BindGPUVertexBuffers(applyArgs.pass, firstSlot, bindings.data(), bindings.size());
}

void BindIndexBuffer::apply(const ApplyArgs &applyArgs) const {
    if (auto it = applyArgs.indexBuffers->find(indexBuffer); it != applyArgs.indexBuffers->end()) {
        SDL_GPUBufferBinding binding(it->second.buffer, offset);
        SDL_BindGPUIndexBuffer(applyArgs.pass, &binding, elementSize);
    }
}

void BindFragmentSampler::apply(const ApplyArgs &applyArgs) const {
    std::vector<SDL_GPUTextureSamplerBinding> bindings;
    bindings.reserve(bindingData.size());
    for (const auto &[texture, sampler] : bindingData) {
        if (!applyArgs.gpuSamplers->contains(sampler)) {
            // Log here
            return;
        }
        bindings.emplace_back(texture->texture, applyArgs.gpuSamplers->at(sampler).sampler);
    }
    SDL_BindGPUFragmentSamplers(applyArgs.pass, firstSlot, bindings.data(), bindings.size());
}

void DrawPrimitive::apply(const ApplyArgs &applyArgs) const {
    SDL_DrawGPUPrimitives(applyArgs.pass, vertexCount, instanceCount, firstVertex, firstInstance);
}

void DrawIndexedPrimitive::apply(const ApplyArgs &applyArgs) const {
    SDL_DrawGPUIndexedPrimitives(applyArgs.pass, indexPerInstance, instanceCount, firstIndex, vertexOffset, firstInstance);
}
