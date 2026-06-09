//
// Created by dev on 4/19/26.
//

#include "Root.h"

#include <algorithm>
#include <ranges>
#include <string>

#include "CommandBuffer.h"
#include "CopyPass.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"

#ifdef DEBUG_BUILD
bool DEBUG_GPU = true;
#else
bool DEBUG_GPU = false;
#endif

namespace {
    struct GPUTextureDeleter {
        SDL_GPUDevice *device = nullptr;

        void operator()(SDL_GPUTexture *texture) const {
            if (texture) {
                SDL_ReleaseGPUTexture(device, texture);
            }
        }
    };

    using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;
    using GPUTexturePtr = std::unique_ptr<SDL_GPUTexture, GPUTextureDeleter>;

    struct RenderPass {
        Movable<SDL_GPURenderPass *> pass = nullptr;

        explicit RenderPass(SDL_GPUCommandBuffer *buffer, SDL_GPUColorTargetInfo *info,
                            Uint32 numTargets = 1,
                            SDL_GPUDepthStencilTargetInfo *depthInfo = nullptr) {
            pass = SDL_BeginGPURenderPass(buffer, info, numTargets, depthInfo);
        }

        ~RenderPass() {
            if (pass) {
                SDL_EndGPURenderPass(pass);
            }
        }
    };

    struct UploadTransferBuffer {
        Movable<SDL_GPUTransferBuffer *> buf = nullptr;
        SDL_GPUDevice *device = nullptr;
        size_t currentSize = 0;
        bool mapped = false;

        UploadTransferBuffer(SDL_GPUDevice *dev, size_t initialSize)
            : device(dev), currentSize(initialSize) {
            Allocate(initialSize);
        }

        ~UploadTransferBuffer() {
            Release();
        }

        // Call this right before mapping to guarantee you have enough space
        void ensureCapacity(size_t requiredSize) {
            if (requiredSize <= currentSize) return;

            // Grow by 1.5x or scale up to exactly what is requested
            size_t newSize = currentSize + (currentSize >> 1);
            if (newSize < requiredSize) newSize = requiredSize;

#ifdef DEBUG_BUILD
            SDL_LogWarn(SDL_LOG_CATEGORY_RENDER,
                        "PERF WARNING: UploadTransferBuffer growing from %zu KB to %zu KB. "
                        "This causes a runtime stutter!", currentSize / 1024, newSize / 1024);
#endif

            Release();
            Allocate(newSize);
        }

        void *map(size_t requiredSize, bool cycle = false) {
            ensureCapacity(requiredSize);

            if (!buf) return nullptr;
            auto ptr = SDL_MapGPUTransferBuffer(device, buf, cycle);
            mapped = !!ptr;
            return ptr;
        }

        void unmap() {
            if (buf && mapped) {
                SDL_UnmapGPUTransferBuffer(device, buf);
                mapped = false;
            }
        }

    private:
        void Allocate(size_t size) {
            SDL_GPUTransferBufferCreateInfo info{};
            info.size = size;
            info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
            buf = SDL_CreateGPUTransferBuffer(device, &info);
            currentSize = size;
        }

        void Release() {
            if (mapped) unmap();
            if (buf) {
                SDL_ReleaseGPUTransferBuffer(device, buf);
                buf = nullptr;
            }
            currentSize = 0;
        }
    };

    bool InitializeEntryWithGPUTexture(const CopyPass &pass, const std::string &path, TextureAsset *asset) {
        const SurfacePtr surface = {SDL_LoadPNG(path.c_str()), SDL_DestroySurface};
        if (!surface) {
            SDL_Log("Failed to load image %s: %s", path.c_str(), SDL_GetError());
            return false;
        }

        const SurfacePtr convertedSurface = {
            SDL_ConvertSurface(surface.get(), SDL_PIXELFORMAT_RGBA8888), SDL_DestroySurface
        };
        if (!convertedSurface) {
            SDL_Log("Surface conversion failed: %s", SDL_GetError());
            return false;
        }

        SDL_GPUTextureCreateInfo info{};
        info.type = SDL_GPU_TEXTURETYPE_2D;
        info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        info.width = convertedSurface->w;
        info.height = convertedSurface->h;
        info.layer_count_or_depth = 1;
        info.num_levels = 1;
        info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

        GPUTexturePtr texture =
                {SDL_CreateGPUTexture(asset->device, &info), GPUTextureDeleter{asset->device}};

        if (!texture) {
            SDL_Log(
                "Failed to create GPU texture %s: %s",
                path.c_str(),
                SDL_GetError()
            );
            return false;
        }

        size_t size = convertedSurface->pitch * convertedSurface->h;

        UploadTransferBuffer transferBuffer{asset->device, size};
        if (!transferBuffer.buf) {
            SDL_Log("Transfer buffer acquisition failed: %s", SDL_GetError());
            return false;
        }

        void *mapped = transferBuffer.map(size);
        if (!mapped) {
            SDL_Log("Transfer buffer mapping failed: %s", SDL_GetError());
            return false;
        }

        SDL_memcpy(
            mapped,
            convertedSurface->pixels,
            size
        );

        transferBuffer.unmap();

        SDL_GPUTextureTransferInfo src{};
        src.transfer_buffer = transferBuffer.buf;

        SDL_GPUTextureRegion dst{};
        dst.texture = texture.get();
        dst.w = convertedSurface->w;
        dst.h = convertedSurface->h;
        dst.d = 1;
        asset->w = convertedSurface->w;
        asset->h = convertedSurface->h;
        asset->d = 1;

        SDL_UploadToGPUTexture(
            pass.pass,
            &src,
            &dst,
            false
        );
        asset->texture = texture.release();
        return true;
    }

    auto getZIndex(const RenderItem &item) {
        return std::visit([](const auto &i) { return i.zIndex; }, item);
    };

    auto getPipeline(const RenderItem &item) {
        return std::visit([](const auto &i) { return i.pipeline; }, item);
    };

    auto getIndexKind(const RenderItem &item) {
        return std::visit([](const auto &i) { return i.indexKind; }, item);
    }

    auto getVertexFormat(const RenderItem &item) {
        return std::visit([](const auto &i) { return i.vertexFormat; }, item);
    }

    auto getNumIndices(const RenderItem &item) {
        return std::visit([](const auto &i) { return i.numIndices; }, item);
    }

    auto getNumVertices(const RenderItem &item) {
        return std::visit([](const auto &i) { return i.numVertices; }, item);
    }

    auto getAsset(const RenderItem &item) {
        return std::visit([](const auto &i) { return i.asset; }, item);
    }

    auto getSampler(const RenderItem &item) {
        return std::visit([](const auto &i) { return i.gpuSampler; }, item);
    }

    auto getStride(VertexFormatID id) {
        if (id == vertexFormatID<SimpleVertex>()) {
            return sizeof(SimpleVertex);
        }
        return size_t{1};
    }
}


Root::Root(GameConfig config) : GameObject({}), window(nullptr, SDL_DestroyWindow),
                                device(nullptr, SDL_DestroyGPUDevice) {
    window.reset(
        SDL_CreateWindow(
            "Hello World",
            config.SCREEN_WIDTH,
            config.SCREEN_HEIGHT,
            SDL_WINDOW_FULLSCREEN
        )
    );

    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return;
    }

    gpuSamplers.emplace("nearest", GPUSampler{
                            SDL_GPUSamplerCreateInfo{
                                .min_filter = SDL_GPU_FILTER_NEAREST,
                                .mag_filter = SDL_GPU_FILTER_NEAREST,
                                .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
                                .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                                .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE
                            }
                        });

    gpuSamplers.emplace("linear", GPUSampler{
                            SDL_GPUSamplerCreateInfo{
                                .min_filter = SDL_GPU_FILTER_LINEAR,
                                .mag_filter = SDL_GPU_FILTER_LINEAR,
                                .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
                                .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                                .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE
                            }
                        });

    gpuSamplers.emplace("anisotropic", GPUSampler{
                            SDL_GPUSamplerCreateInfo{
                                .min_filter = SDL_GPU_FILTER_LINEAR,
                                .mag_filter = SDL_GPU_FILTER_LINEAR,
                                .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
                                .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                                .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                                .max_anisotropy = 8.0f,
                                .enable_anisotropy = true,
                            }
                        });;
}

void Root::initialize() {
    if (!restartDevice(false)) {
        return;
    }

    valid = true;
}

bool Root::restartDevice(bool validAfter) {
    valid = false;
    device.reset(
        SDL_CreateGPUDevice(
            SDL_GPU_SHADERFORMAT_SPIRV |
            SDL_GPU_SHADERFORMAT_DXIL |
            SDL_GPU_SHADERFORMAT_MSL,
            DEBUG_GPU,
            nullptr
        )
    );

    if (!device) {
        SDL_Log("GPU device creation failed: %s", SDL_GetError());
        return false;
    }

    if (!SDL_ClaimWindowForGPUDevice(device.get(), window.get())) {
        SDL_Log("ClaimWindowForGPUDevice failed: %s", SDL_GetError());
        return false;
    }
    for (auto &sampler: gpuSamplers | std::views::values) {
        sampler.create(device.get());
    }
    flushState = FlushState{};

    for (auto &cache: textureCache) {
        if (auto asset = cache.second.lock()) {
            asset->isReady = false;
            asset->device = device.get();
        }
    }
    if (CommandBuffer buffer(device.get()); !initializeStaticResources(buffer)) {
        return false;
    }

    valid = validAfter;
    return true;
}

bool Root::initializeStaticResources(CommandBuffer &buffer) {
    CopyPass combinedPass{buffer.cmd};
    std::vector<uint16_t> indexData;
    indexData.reserve(16384 * 6);

    for (uint16_t i = 0; i < 16384; i++) {
        uint16_t baseVertex = i * 4;
        indexData.push_back(baseVertex + 0);
        indexData.push_back(baseVertex + 1);
        indexData.push_back(baseVertex + 2);
        indexData.push_back(baseVertex + 2);
        indexData.push_back(baseVertex + 3);
        indexData.push_back(baseVertex + 0);
    }

    SDL_GPUBufferCreateInfo ibInfo{};
    ibInfo.size = indexData.size() * sizeof(uint16_t);
    ibInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;

    indexBuffers.insert_or_assign("quad", GPUBuffer(device.get(), &ibInfo));

    UploadTransferBuffer transferBuffer{device.get(), ibInfo.size};

    const auto mapped = transferBuffer.map(ibInfo.size);
    if (!mapped) {
        SDL_Log("Transfer buffer mapping failed: %s", SDL_GetError());
        return false;
    }

    SDL_memcpy(mapped, indexData.data(), ibInfo.size);

    transferBuffer.unmap();

    SDL_GPUTransferBufferLocation source{transferBuffer.buf, 0};
    SDL_GPUBufferRegion dest{indexBuffers.at("quad").buffer, 0, ibInfo.size};
    SDL_UploadToGPUBuffer(combinedPass.pass, &source, &dest, false);

    uint32_t white = 0xFFFFFFFF;
    SDL_GPUTextureCreateInfo texInfo{};
    texInfo.type = SDL_GPU_TEXTURETYPE_2D;
    texInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    texInfo.width = 1;
    texInfo.height = 1;
    texInfo.layer_count_or_depth = 1;
    texInfo.num_levels = 1;
    texInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

    SDL_GPUTexture *whiteTexture = SDL_CreateGPUTexture(device.get(), &texInfo);
    if (!whiteTexture) {
        SDL_Log("Failed to create white texture: %s", SDL_GetError());
        return false;
    }

    UploadTransferBuffer whiteTransfer{device.get(), sizeof(uint32_t)};
    void *whiteMapped = whiteTransfer.map(sizeof(uint32_t));
    if (!whiteMapped) {
        SDL_Log("Transfer buffer mapping failed for white texture: %s", SDL_GetError());
        SDL_ReleaseGPUTexture(device.get(), whiteTexture);
        return false;
    }
    SDL_memcpy(whiteMapped, &white, sizeof(uint32_t));
    whiteTransfer.unmap();

    SDL_GPUTextureTransferInfo whiteSrc{whiteTransfer.buf, 0, 4, 1};
    SDL_GPUTextureRegion whiteDst{whiteTexture, 0, 0, 0, 0, 1, 1, 1};
    SDL_UploadToGPUTexture(combinedPass.pass, &whiteSrc, &whiteDst, false);

    renderer.restart(device.get(), window.get());
    if (! (renderer.linePipeline && renderer.texturePipeline && renderer.trianglePipeline)) {
        SDL_ReleaseGPUTexture(device.get(), whiteTexture);
        valid = false;
        return false;
    }
    renderer.whiteTexture = std::make_shared<TextureAsset>(true, whiteTexture, 1, 1, 1, device.get());

    return true;
}

void Root::update(const uint64_t dt) {
    for (const auto &child: children) {
        child->_update(dt);
    }
    _addChildren();
    _clean();
}

void Root::handleGeometryUploads(CopyPass &pass) {
    if (!renderer.renderItems.empty()) {
        auto flush = [this](auto begin, auto end) {
            while (begin != end) {
                auto firstVertex = flushState.vertexOffsets[flushState.vertexBuffer].first;
                uint32_t lastIndex = flushState.vertexOffsets[flushState.vertexBuffer].second.size() / getStride(
                                         flushState.vertexBuffer);
                auto totalVertices = lastIndex - firstVertex;
                std::vector<GraphicsCommand> drawCommands;
                if (flushState.indexed) {
                    auto indexPerInstance = (totalVertices / flushState.numVertices) * flushState.numIndices;
                    int offset = 0;
                    while (indexPerInstance > 0) {
                        auto chunkSize = static_cast<uint32_t>(indexBuffers.at(flushState.indexBuffer).size / sizeof(uint16_t));
                        auto currentIndex = std::min(indexPerInstance, chunkSize);
                        drawCommands.emplace_back(
                            DrawIndexedPrimitive{
                                .indexPerInstance = currentIndex,
                                .instanceCount = 1,
                                .firstIndex = 0,
                                .vertexOffset = static_cast<int32_t>(firstVertex + offset * chunkSize),
                                .firstInstance = flushState.instanceOffsets[flushState.
                                    instanceBuffer].first
                            }
                        );
                        indexPerInstance -= currentIndex;
                        offset++;
                    }
                } else {
                    drawCommands.emplace_back(
                        DrawPrimitive{
                            .vertexCount = totalVertices,
                            .instanceCount = 1,
                            .firstVertex = firstVertex,
                            .firstInstance = flushState.instanceOffsets[flushState.
                                instanceBuffer].first,
                        }
                    );
                }
                std::vector<GraphicsCommand> newCommands;
                bool changed = false;
                auto pipeline = getPipeline(*begin);
                if (pipeline != flushState.pipeline) {
                    changed = true;
                    flushState.pipeline = pipeline;
                    newCommands.emplace_back(BindPipeline(flushState.pipeline));
                }
                auto vertexBuffer = getVertexFormat(*begin);
                if (vertexBuffer != flushState.vertexBuffer) {
                    changed = true;
                    flushState.vertexOffsets[flushState.vertexBuffer].first = lastIndex;
                    flushState.vertexBuffer = vertexBuffer;
                    newCommands.emplace_back(BindVertexBuffers{
                        .bindingData = {
                            {
                                flushState.vertexBuffer,
                                flushState.vertexOffsets[flushState.vertexBuffer].first * getStride(
                                    flushState.vertexBuffer)
                            }
                        },
                        .firstSlot = 0
                    });
                }
                auto numIndices = getNumIndices(*begin);
                if (numIndices != flushState.numIndices) {
                    changed = true;
                    flushState.numIndices = numIndices;
                }
                auto numVertices = getNumVertices(*begin);
                if (numVertices != flushState.numVertices) {
                    changed = true;
                    flushState.numVertices = numVertices;
                }
                auto indexBuffer = getIndexKind(*begin);
                if (flushState.indexed != (indexBuffer != StringHash(""))) {
                    changed = true;
                    flushState.indexed = (indexBuffer != StringHash(""));
                }
                if (indexBuffer != flushState.indexBuffer) {
                    changed = true;
                    flushState.indexBuffer = indexBuffer;
                    newCommands.emplace_back(BindIndexBuffer{
                        .indexBuffer = flushState.indexBuffer,
                        .offset = 0,
                        .elementSize = SDL_GPU_INDEXELEMENTSIZE_16BIT
                    });
                }
                auto sampler = getSampler(*begin);
                auto asset = getAsset(*begin);
                if (sampler != flushState.sampler || asset != flushState.
                    texture) {
                    changed = true;
                    flushState.texture = asset;
                    flushState.sampler = sampler;
                    newCommands.emplace_back(BindFragmentSampler{
                        .bindingData = {{flushState.texture, flushState.sampler}},
                        .firstSlot = 0
                    });
                }
                if (changed) {
                    if (totalVertices > 0) {
                        graphicsCommands.insert(graphicsCommands.end(), drawCommands.begin(), drawCommands.end());
                    }
                    if (!std::holds_alternative<SentinelRenderItem>(*begin)) {
                        graphicsCommands.insert(graphicsCommands.end(), newCommands.begin(), newCommands.end());
                    }
                }

                if (auto primitive = std::get_if<PrimitiveRenderItem>(&*begin)) {
                    auto &buffer = flushState.vertexOffsets[flushState.vertexBuffer].second;
                    auto *rawData = reinterpret_cast<const uint8_t *>(primitive->vertices.data());
                    size_t totalBytes = primitive->vertices.size() * sizeof(primitive->vertices[0]);
                    buffer.insert(buffer.end(), rawData, rawData + totalBytes);
                }
                if (auto sprite = std::get_if<SpriteRenderItem>(&*begin)) {
                    auto &buffer = flushState.vertexOffsets[flushState.vertexBuffer].second;
                    auto *rawData = reinterpret_cast<const uint8_t *>(sprite->vertices.data());
                    size_t totalBytes = sprite->vertices.size() * sizeof(sprite->vertices[0]);
                    buffer.insert(buffer.end(), rawData, rawData + totalBytes);
                }

                ++begin;
            }
        };

        renderer.renderItems.emplace_back(SentinelRenderItem{.pipeline = nullptr}); // sentinel
        flush(renderer.renderItems.begin(), renderer.renderItems.end());
        for (auto &[id, tuple]: flushState.vertexOffsets) {
            uploadVertices(pass, id, tuple.second);
            tuple.first = 0;
            tuple.second.clear();
        }
        renderer.renderItems.clear();
    }
}

void Root::uploadVertices(CopyPass &pass, VertexFormatID id, const std::vector<uint8_t> &vertices) {
    if (vertices.empty()) return;
    const size_t size = vertices.size();
    UploadTransferBuffer transferBuffer{device.get(), size};
    void *mapped = transferBuffer.map(size);
    if (!mapped) {
        SDL_Log("Transfer buffer mapping failed: %s", SDL_GetError());
        return;
    }
    SDL_memcpy(mapped, vertices.data(), size);
    transferBuffer.unmap();
    SDL_GPUBufferCreateInfo info{};
    info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    info.size = size;
    GPUBuffer buffer{device.get(), &info};
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer.buf;
    buffer.upload(pass.pass, &location, size, 0);
    gpuBuffers.insert_or_assign(id, std::move(buffer));
}

void Root::handleUploads(CommandBuffer &buffer) {
    CopyPass pass(buffer.cmd);
    if (!pass.pass) {
        SDL_Log("Starting copy pass failed: %s", SDL_GetError());
        valid = false;
        return;
    }

    for (auto &entry: textureCache) {
        if (auto asset = entry.second.lock()) {
            if (!asset->isReady) {
                if (InitializeEntryWithGPUTexture(pass, entry.first, asset.get())) {
                    asset->isReady = true;
                }
            }
        }
    }

    handleGeometryUploads(pass);
}

void Root::driveDraw() {
    draw({.renderer = &renderer, .zIndex = 0});
    if (!valid) {
        for (auto &val: gpuSamplers | std::views::values) {
            val.device = nullptr;
        }
        for (auto &val: gpuBuffers | std::views::values) {
            val.device = nullptr;
        }
        for (auto &renderItem: renderer.renderItems) {
            if (const auto spriteItem = std::get_if<SpriteRenderItem>(&renderItem)) {
                spriteItem->asset->isReady = false;
                spriteItem->asset->device = nullptr;
            }
        }
        renderer.renderItems.clear();
        restartDevice();
    }
    graphicsCommands.clear();
}

void Root::handleRenderPass(SDL_GPUColorTargetInfo target, const CommandBuffer &cmdBuffer) {
    RenderPass pass(cmdBuffer.cmd, &target);
    ApplyArgs args{};
    args.cmd = cmdBuffer.cmd;
    args.gpuBuffers = &gpuBuffers;
    args.indexBuffers = &indexBuffers;
    args.pass = pass.pass;
    for (auto &graphicsCommand: graphicsCommands) {
        std::visit([args](auto &val) { return val.apply(args); }, graphicsCommand);
    }
}

void Root::draw(RenderContext ctx) {
    CommandBuffer cmd(device.get());
    if (!cmd.cmd) {
        SDL_Log("Command buffer acquisition failed: %s", SDL_GetError());
        valid = false;
        return;
    }

    SDL_GPUTexture *swapchain = nullptr;

    SDL_WaitAndAcquireGPUSwapchainTexture(
        cmd.cmd,
        window.get(),
        &swapchain,
        nullptr,
        nullptr
    );

    if (!swapchain)
        return;

    renderer.renderItems.clear();
    for (const auto &child: children) {
        child->_draw(ctx);
    }
    auto &renderItems = renderer.renderItems;
    std::ranges::stable_sort(renderItems, [](const RenderItem &jobA, const RenderItem &jobB) {
        const auto aZindex = getZIndex(jobA);
        const auto bZindex = getZIndex(jobB);
        if (aZindex != bZindex) {
            return aZindex < bZindex;
        }
        const auto aPipeline = getPipeline(jobA);
        const auto bPipeline = getPipeline(jobB);
        if (aPipeline && bPipeline && aPipeline != bPipeline) {
            return aPipeline < bPipeline;
        }
        const auto aIndexKind = getIndexKind(jobA);
        const auto bIndexKind = getIndexKind(jobB);
        if (aIndexKind && bIndexKind && aIndexKind != bIndexKind) {
            return aIndexKind < bIndexKind;
        }
        const auto aNumIndices = getNumIndices(jobA);
        const auto bNumIndices = getNumIndices(jobB);
        if (aNumIndices && bNumIndices && aNumIndices != bNumIndices) {
            return aNumIndices < bNumIndices;
        }
        const auto aNumVertices = getNumVertices(jobA);
        const auto bNumVertices = getNumVertices(jobB);
        if (aNumVertices && bNumVertices && aNumVertices != bNumVertices) {
            return aNumVertices < bNumVertices;
        }
        const auto aAsset = getAsset(jobA);
        const auto bAsset = getAsset(jobB);
        if (aAsset && bAsset && aAsset != bAsset) {
            return aAsset->texture < bAsset->texture;
        }
        const auto aSampler = getSampler(jobA);
        const auto bSampler = getSampler(jobB);
        if (aSampler && bSampler && aSampler != bSampler) {
            return aSampler < bSampler;
        }
        return false;
    });

    handleUploads(cmd);
    if (!valid) {
        return;
    }

    SDL_GPUColorTargetInfo target{};
    target.texture = swapchain;
    target.load_op = SDL_GPU_LOADOP_CLEAR;
    target.store_op = SDL_GPU_STOREOP_STORE;
    target.clear_color = {0.f, 0.f, 0.f, 1.f};

    handleRenderPass(target, cmd);
    if (!valid) {
        return;
    }


    cmd.submit();
}

Root *Root::getRoot() {
    return this;
}

std::shared_ptr<TextureAsset> Root::getTexture(const std::string &path) {
    if (auto it = textureCache.find(path); it != textureCache.end()) {
        if (auto instance = it->second.lock()) {
            return instance;
        }
    }

    std::shared_ptr<TextureAsset> asset = std::make_shared<TextureAsset>();
    asset->isReady = false;
    asset->texture = nullptr;
    asset->device = device.get();

    textureCache[path] = asset;

    return asset;
}

const GPUSampler *Root::getGPUSampler(const std::string &name) {
    auto it = gpuSamplers.find(StringHash{name});
    if (it != gpuSamplers.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool Root::isValid() const {
    return valid;
}
