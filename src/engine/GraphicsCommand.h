//
// Created by dev on 5/25/26.
//

#ifndef MYENGINE_DRAWCOMMAND_H
#define MYENGINE_DRAWCOMMAND_H
#include <unordered_map>
#include <variant>
#include <vector>

#include "GPUBuffer.h"
#include "GPUSampler.h"
#include "RenderItem.h"
#include "SDL3/SDL_gpu.h"

struct ApplyArgs {
    SDL_GPUCommandBuffer *cmd;
    SDL_GPURenderPass *pass;
    std::unordered_map<VertexFormatID, GPUBuffer> *gpuBuffers;
    std::unordered_map<StringHash, GPUBuffer> *indexBuffers;
    std::unordered_map<StringHash, GPUSampler> *gpuSamplers;
};

struct BindPipeline {
    SDL_GPUGraphicsPipeline *pipeline;
    void apply(const ApplyArgs &applyArgs) const;
};

struct PushUniform {
    enum Kind {
        VERTEX,
        FRAG,
    };
    Kind kind;
    uint32_t slot;
    std::vector<uint8_t> data;
    void apply(const ApplyArgs &applyArgs) const;
};

struct BindVertexBuffers {
    std::vector<std::pair<VertexFormatID, int>> bindingData;
    int firstSlot;
    void apply(const ApplyArgs &applyArgs) const;
};

struct BindIndexBuffer {
    StringHash indexBuffer;
    int offset;
    SDL_GPUIndexElementSize elementSize;
    void apply(const ApplyArgs &applyArgs) const;
};

struct BindFragmentSampler {
    std::vector<std::pair<std::shared_ptr<TextureAsset>, StringHash>> bindingData;
    int firstSlot;
    void apply(const ApplyArgs &applyArgs) const;
};

struct DrawPrimitive {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
    void apply(const ApplyArgs &applyArgs) const;
};

struct DrawIndexedPrimitive {
    uint32_t indexPerInstance;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
    void apply(const ApplyArgs &applyArgs) const;
};

typedef std::variant<BindPipeline, PushUniform, BindVertexBuffers, BindIndexBuffer, BindFragmentSampler, DrawPrimitive, DrawIndexedPrimitive> GraphicsCommand;

#endif //MYENGINE_DRAWCOMMAND_H