//
// Created by dev on 4/19/26.
//

#ifndef MYENGINE_ROOT_H
#define MYENGINE_ROOT_H
#include <unordered_map>
#include <string>

#include "Camera.h"
#include "CommandBuffer.h"
#include "CopyPass.h"
#include "GameConfig.h"
#include "GameObject.h"
#include "GPUBuffer.h"
#include "GPUSampler.h"
#include "GraphicsCommand.h"
#include "Renderer.h"
#include "TextureAsset.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"

class Root final : public GameObject {
public:

    Root *getRoot() override;

    std::shared_ptr<TextureAsset> getTexture(const std::string &path);

    const GPUSampler *getGPUSampler(const std::string &name);

    const std::string *getIndexKey(std::string &name);

    [[nodiscard]] bool isValid() const;
private:
    friend class SDLMain;
    explicit Root(GameConfig config);
    void driveDraw();

    void initialize() override;
    void update(uint64_t dt) override;
    void draw(RenderContext ctx) override;

    void handleGeometryUploads(CopyPass &pass);

    void uploadVertices(CopyPass &pass, VertexFormatID id, const std::vector<uint8_t> &vertices);

    void handleUploads(CommandBuffer &buffer);
    void handleRenderPass(SDL_GPUColorTargetInfo target, const CommandBuffer &cmdBuffer);

    bool restartDevice(bool validAfter = true);

    bool initializeStaticResources(CommandBuffer &buffer);
    static constexpr uint8_t FRAMES_IN_FLIGHT = 3;
    uint8_t frame;

    Renderer renderer;
    std::unordered_map<std::string, std::weak_ptr<TextureAsset> > textureCache;
    std::unordered_map<StringHash, GPUBuffer> indexBuffers;
    std::array<std::unordered_map<VertexFormatID, GPUBuffer>, FRAMES_IN_FLIGHT> gpuBuffers;
    std::unordered_map<StringHash, GPUSampler> gpuSamplers;
    std::vector<GraphicsCommand> graphicsCommands;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    std::unique_ptr<SDL_GPUDevice, decltype(&SDL_DestroyGPUDevice)> device;
    bool valid;
    struct FlushState {
        SDL_GPUGraphicsPipeline *pipeline = nullptr;
        VertexFormatID vertexBuffer = 0;
        VertexFormatID instanceBuffer = 0;
        std::shared_ptr<TextureAsset> texture = nullptr;
        StringHash sampler = "";
        uint32_t numVertices = 0;
        uint32_t numIndices = 0;
        StringHash indexBuffer = "";
        std::unordered_map<VertexFormatID, std::pair<uint32_t, std::vector<uint8_t> > > vertexOffsets;
        std::unordered_map<VertexFormatID, std::pair<uint32_t, std::vector<uint8_t> > > instanceOffsets;
        std::unordered_map<std::string *, std::pair<uint32_t, std::vector<uint8_t> > > indexOffsets;
        bool indexed = false;
    };
    FlushState flushState;
    Camera camera;
};


#endif //ENGINE_ROOT_H
