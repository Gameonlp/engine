//
// Created by dev on 5/16/26.
//

#ifndef MYENGINE_RENDERER_H
#define MYENGINE_RENDERER_H
#include <vector>

#include "RenderItem.h"
#include "TextureAsset.h"
#include "SDL3/SDL_gpu.h"


struct RenderContext;

class Renderer {
public:
    void drawLine(RenderContext ctx, SDL_FPoint src, SDL_FPoint dst, SDL_FColor color);
    void drawTriangle(RenderContext ctx, SDL_FPoint a, SDL_FPoint b, SDL_FPoint c, SDL_FColor color, bool filled);
    void drawRect(RenderContext ctx, SDL_FRect rect, SDL_FColor color, bool filled);
    void drawTexture(RenderContext ctx, std::shared_ptr<TextureAsset> &texture, SDL_FRect srcRect, SDL_FRect dstRect, float rotation = 0, bool
                     hMirror =
                             false, bool
                     vMirror = false, SDL_FColor color = {1, 1, 1, 1}, StringHash gpuSampler = "linear");


private:
    friend class Root;
    static SDL_GPUGraphicsPipeline *createPrimitivePipeline(SDL_GPUDevice *device, SDL_Window *window, const std::vector<uint8_t> *vertexShaderData, const std::vector<uint8_t> *
                                                            fragShaderData, SDL_GPUPrimitiveType type);
    static SDL_GPUColorTargetDescription getColorTargetDescription(SDL_GPUDevice *device, SDL_Window *window);

    static SDL_GPUGraphicsPipeline *createTrianglePipeline(SDL_GPUDevice *device, SDL_Window *window);
    static SDL_GPUGraphicsPipeline *createLinePipeline(SDL_GPUDevice *device, SDL_Window *window);


    std::vector<RenderItem> renderItems;
    SDL_GPUGraphicsPipeline *linePipeline;
    SDL_GPUGraphicsPipeline *trianglePipeline;
    SDL_GPUGraphicsPipeline *texturePipeline;
    std::shared_ptr<TextureAsset> whiteTexture;

    void restart(SDL_GPUDevice *device, SDL_Window *window);
};


#endif //MYENGINE_RENDERER_H