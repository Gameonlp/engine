//
// Created by dev on 5/17/26.
//

#ifndef MYENGINE_RENDERITEM_H
#define MYENGINE_RENDERITEM_H
#include <array>
#include <memory>

#include "Vertex.h"
#include "TextureAsset.h"
#include "GPUSampler.h"
#include "StringHash.h"

using VertexFormatID = uintptr_t;

template<typename T>
constexpr VertexFormatID vertexFormatID() {
    static char id;
    return reinterpret_cast<uintptr_t>(&id);
}

struct SentinelRenderItem {
    std::vector<SimpleVertex> vertices;
    uint32_t numVertices;
    uint32_t numIndices;
    StringHash indexKind;
    SDL_GPUGraphicsPipeline *pipeline;
    int zIndex;
    VertexFormatID vertexFormat = vertexFormatID<SentinelRenderItem>();
};

struct PrimitiveRenderItem {
    std::vector<SimpleVertex> vertices;
    uint32_t numVertices;
    uint32_t numIndices;
    StringHash indexKind;
    SDL_GPUGraphicsPipeline *pipeline;
    int zIndex;
    VertexFormatID vertexFormat = vertexFormatID<SimpleVertex>();
};

struct SpriteRenderItem {
    std::array<SimpleVertex, 4> vertices;
    uint32_t numVertices = 4;
    uint32_t numIndices = 6;
    StringHash indexKind = "quad";
    std::shared_ptr<TextureAsset> asset;
    StringHash gpuSampler;
    SDL_GPUGraphicsPipeline *pipeline;
    int zIndex;
    VertexFormatID vertexFormat = vertexFormatID<SimpleVertex>();
};

typedef std::variant<PrimitiveRenderItem, SpriteRenderItem, SentinelRenderItem> RenderItem;

#endif //MYENGINE_RENDERITEM_H