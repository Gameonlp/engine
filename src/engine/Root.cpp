//
// Created by dev on 4/19/26.
//

#include "Root.h"

#include <string>

#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_log.h"

void Root::update(const float dt) {
    for (const auto &child: children)
        child->_update(dt);
    _clean();
    _addChildren();
}

void Root::draw() {
    draw({renderer});
}

void Root::draw(const RenderContext ctx) {
    SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx.renderer);
    for (const auto &child: children)
        child->_draw(ctx);
    SDL_RenderPresent(ctx.renderer);
}

Root *Root::getRoot() {
    return this;
}

std::shared_ptr<SDL_Texture> Root::getTexture(SDL_Renderer *renderer, const std::string &path) {
    if (!renderer) {
        renderer = this->renderer;
    }
    if (auto renderIt = rendererTextureCache.find(renderer); renderIt != rendererTextureCache.end()) {
        const auto &textureCache = renderIt->second;
        if (auto it = textureCache.find(path); it != textureCache.end()) {
            if (auto instance = it->second.lock()) {
                return instance;
            }
        }
    }
    auto surface = SDL_LoadPNG(path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", path.c_str(), SDL_GetError());
        return nullptr;
    }
    SDL_Texture *rawTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!rawTexture) {
        SDL_Log("Failed to load image %s: %s", path.c_str(), SDL_GetError());
        return nullptr;
    }

    std::shared_ptr<SDL_Texture> texture(
        rawTexture,
        SDL_DestroyTexture
    );
    if (texture) {
        rendererTextureCache[renderer][path] = texture;
    }
    return texture;
}

SDL_Renderer *Root::getRenderer() const {
    return renderer;
}

SDL_Window *Root::getWindow() const {
    return window;
}
