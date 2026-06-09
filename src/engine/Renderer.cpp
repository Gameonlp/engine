//
// Created by dev on 5/16/26.
//

#include "Renderer.h"

#include <ranges>

#include "Camera.h"
#include "Vertex.h"
#include "EmbeddedShaders.h"
#include "RenderContext.h"
#include "Root.h"
#include "Utils.h"
#include "SDLExtensions.h"


void Renderer::drawLine(RenderContext ctx, SDL_FPoint src, SDL_FPoint dst, SDL_FColor color) {
    const auto cameraRect = ctx.camera->cameraRect();
    if (SDL_GetRectAndLineIntersectionFloat(&cameraRect, &src.x, &src.y, &dst.x, &dst.y)) {
        const auto [srcX, srcY] = ctx.camera->toCameraSpace(src);
        const auto [dstX, dstY] = ctx.camera->toCameraSpace(dst);
        const auto [r, g, b, a] = Utils::premultiply(color);
        renderItems.emplace_back(PrimitiveRenderItem{
            .vertices = {
                SimpleVertex{srcX, srcY, 0, 0, 0, r, g, b, a},
                SimpleVertex{dstX, dstY, 0, 0, 0, r, g, b, a}
            },
            .pipeline = linePipeline,
            .zIndex = ctx.zIndex,
        });
    }
}

void Renderer::drawTriangle(RenderContext ctx, SDL_FPoint a, SDL_FPoint b, SDL_FPoint c, SDL_FColor color,
                            bool filled) {
    const auto cameraRect = ctx.camera->cameraRect();
    if (Utils::rect_tri_intersect(cameraRect, a, b, c)) {
        const auto [aX, aY] = ctx.camera->toCameraSpace(a);
        const auto [bX, bY] = ctx.camera->toCameraSpace(b);
        const auto [cX, cY] = ctx.camera->toCameraSpace(c);
        const auto [r, g, b, a] = Utils::premultiply(color);
        if (filled) {
            renderItems.emplace_back(PrimitiveRenderItem{
                .vertices = {
                    SimpleVertex{aX, aY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{bX, bY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{cX, cY, 0, 0, 0, r, g, b, a}
                },
                .pipeline = trianglePipeline,
                .zIndex = ctx.zIndex,
            });
        } else {
            renderItems.emplace_back(PrimitiveRenderItem{
                .vertices = {
                    SimpleVertex{aX, aY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{bX, bY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{bX, bY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{cX, cY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{cX, cY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{aX, aY, 0, 0, 0, r, g, b, a}
                },
                .pipeline = linePipeline,
                .zIndex = ctx.zIndex,
            });
        }
    }
}

void Renderer::drawRect(RenderContext ctx, SDL_FRect rect, SDL_FColor color, bool filled) {
    const auto cameraRect = ctx.camera->cameraRect();
    if (SDL_HasRectIntersectionFloat(&rect, &cameraRect)) {
        const auto [blX, blY] = ctx.camera->toCameraSpace({rect.x, rect.y + rect.h});
        const auto [brX, brY] = ctx.camera->toCameraSpace({rect.x + rect.w, rect.y + rect.h});
        const auto [trX, trY] = ctx.camera->toCameraSpace({rect.x + rect.w, rect.y});
        const auto [tlX, tlY] = ctx.camera->toCameraSpace({rect.x, rect.y});
        const auto [r, g, b, a] = Utils::premultiply(color);
        if (filled) {
            renderItems.emplace_back(PrimitiveRenderItem{
                .vertices = {
                    SimpleVertex{blX, blY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{brX, brY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{trX, trY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{tlX, tlY, 0, 0, 0, r, g, b, a},
                },
                .indexKind = "quad",
                .pipeline = trianglePipeline,
                .zIndex = ctx.zIndex,
            });
        } else {
            renderItems.emplace_back(PrimitiveRenderItem{
                .vertices = {
                    SimpleVertex{blX, blY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{brX, brY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{brX, brY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{trX, trY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{trX, trY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{tlX, tlY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{tlX, tlY, 0, 0, 0, r, g, b, a},
                    SimpleVertex{blX, blY, 0, 0, 0, r, g, b, a},
                },
                .pipeline = linePipeline,
                .zIndex = ctx.zIndex,
            });
        }
    }
}

void Renderer::drawTexture(RenderContext ctx, std::shared_ptr<TextureAsset> &texture, SDL_FRect srcRect, SDL_FRect dstRect,
                           float rotation,
                           bool hMirror, bool vMirror, SDL_FColor color, StringHash gpuSampler) {
    const auto cameraRect = ctx.camera->cameraRect();
    if (SDL_HasRectIntersectionFloat(&dstRect, &cameraRect)) {
        const auto [r, g, b, a] = Utils::premultiply(color);
        const auto center = SDL_FPoint{dstRect.x + dstRect.w / 2, dstRect.y + dstRect.h / 2};
        const auto [blX, blY] = ctx.camera->toCameraSpace(Utils::rotateAroundOrigin(SDL_FPoint{dstRect.x, dstRect.y + dstRect.h} - center, rotation) + center);
        const auto [brX, brY] = ctx.camera->toCameraSpace(Utils::rotateAroundOrigin(SDL_FPoint{dstRect.x + dstRect.w, dstRect.y + dstRect.h} - center, rotation) + center);
        const auto [tlX, tlY] = ctx.camera->toCameraSpace(Utils::rotateAroundOrigin(SDL_FPoint{dstRect.x, dstRect.y} - center, rotation) + center);
        const auto [trX, trY] = ctx.camera->toCameraSpace(Utils::rotateAroundOrigin(SDL_FPoint{dstRect.x + dstRect.w, dstRect.y} - center, rotation) + center);

        const auto [x, y] = SDL_FPoint{texture->w, texture->h};
        float uLeft   = srcRect.x / x;
        float uRight  = (srcRect.x + srcRect.w) / x;
        float vTop    = srcRect.y / y;
        float vBottom = (srcRect.y + srcRect.h) / y;

        if (hMirror) {
            std::swap(uLeft, uRight);
        }
        if (vMirror) {
            std::swap(vTop, vBottom);
        }

        renderItems.emplace_back(SpriteRenderItem{
            .vertices = {
                // Bottom-Left
                SimpleVertex{blX, blY, 0, uLeft,  vBottom, r, g, b, a},
                // Bottom-Right
                SimpleVertex{brX, brY, 0, uRight, vBottom, r, g, b, a},
                // Top-Right
                SimpleVertex{trX, trY, 0, uRight, vTop,    r, g, b, a},
                // Top-Left
                SimpleVertex{tlX, tlY, 0, uLeft,  vTop,    r, g, b, a},
            },
            .asset = texture,
            .gpuSampler = gpuSampler,
        });
    }
}

SDL_GPUColorTargetDescription Renderer::getColorTargetDescription(SDL_GPUDevice *device, SDL_Window *window) {
    SDL_GPUColorTargetDescription colorTargetDescription{};
    colorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(device, window);
    SDL_GPUColorTargetBlendState blendState{};
    blendState.enable_blend = true;

    blendState.color_blend_op = SDL_GPU_BLENDOP_ADD;
    blendState.alpha_blend_op = SDL_GPU_BLENDOP_ADD;

    blendState.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blendState.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    blendState.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blendState.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescription.blend_state = blendState;
    return colorTargetDescription;
}

SDL_GPUGraphicsPipeline *Renderer::createTrianglePipeline(SDL_GPUDevice *device, SDL_Window *window) {
    return createPrimitivePipeline(device, window, &Engine::Shaders::primitive_vert_spv,
                                   &Engine::Shaders::primitive_frag_spv, SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
}

SDL_GPUGraphicsPipeline *Renderer::createLinePipeline(SDL_GPUDevice *device, SDL_Window *window) {
    return createPrimitivePipeline(device, window, &Engine::Shaders::primitive_vert_spv,
                                   &Engine::Shaders::primitive_frag_spv, SDL_GPU_PRIMITIVETYPE_LINELIST);
}

//TODO add necessary checks to make safe
SDL_GPUGraphicsPipeline *Renderer::createPrimitivePipeline(SDL_GPUDevice *device, SDL_Window *window,
                                                           const std::vector<uint8_t> *vertexShaderData,
                                                           const std::vector<uint8_t> *fragShaderData,
                                                           SDL_GPUPrimitiveType type) {
    SDL_GPUGraphicsPipelineCreateInfo info{};
    // 1. Create Vertex Shader directly from memory
    SDL_GPUShaderCreateInfo vertInfo{};
    vertInfo.code = vertexShaderData->data();
    vertInfo.code_size = vertexShaderData->size();
    vertInfo.entrypoint = "main";
    vertInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    vertInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    SDL_GPUShader *vertexShader = SDL_CreateGPUShader(device, &vertInfo);

    // 2. Create Fragment Shader directly from memory
    SDL_GPUShaderCreateInfo fragInfo{};
    fragInfo.code = fragShaderData->data();
    fragInfo.code_size = fragShaderData->size();
    fragInfo.entrypoint = "main";
    fragInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    fragInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    SDL_GPUShader *fragmentShader = SDL_CreateGPUShader(device, &fragInfo);

    info.vertex_shader = vertexShader;
    info.fragment_shader = fragmentShader;

    info.primitive_type = type;

    SDL_GPUVertexBufferDescription bufferDesc{};
    bufferDesc.slot = 0;
    bufferDesc.pitch = sizeof(SimpleVertex);
    bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

    SDL_GPUVertexAttribute attrs[3]{};

    // vec3 position
    attrs[0].location = 0;
    attrs[0].buffer_slot = 0;
    attrs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    attrs[0].offset = offsetof(SimpleVertex, x);

    // vec2 uv
    attrs[1].location = 1;
    attrs[1].buffer_slot = 0;
    attrs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attrs[1].offset = offsetof(SimpleVertex, u);

    // vec4 color
    attrs[2].location = 2;
    attrs[2].buffer_slot = 0;
    attrs[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    attrs[2].offset = offsetof(SimpleVertex, r);

    info.vertex_input_state.num_vertex_buffers = 1;
    info.vertex_input_state.vertex_buffer_descriptions = &bufferDesc;

    info.vertex_input_state.num_vertex_attributes = 3;
    info.vertex_input_state.vertex_attributes = attrs;

    SDL_GPUColorTargetDescription colorTargetDescription = getColorTargetDescription(device, window);
    info.target_info.num_color_targets = 1;
    info.target_info.color_target_descriptions = &colorTargetDescription;

    info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;

    info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;

    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device, &info);
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);
    return pipeline;
}

void Renderer::restart(SDL_GPUDevice *device, SDL_Window *window) {
    linePipeline = createLinePipeline(device, window);
    trianglePipeline = createTrianglePipeline(device, window);
}
