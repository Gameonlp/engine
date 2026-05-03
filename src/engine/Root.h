//
// Created by dev on 4/19/26.
//

#ifndef MYENGINE_ROOT_H
#define MYENGINE_ROOT_H
#include <unordered_map>
#include <string>

#include "GameObject.h"
#include "SDL3/SDL_render.h"

class Root final : public GameObject {
public:
    Root(SDL_Renderer *renderer, SDL_Window *window) : GameObject({0, 0}), renderer(renderer), window(window) {
    }

    void update(float dt) override;

    void draw();
    void draw(RenderContext ctx) override;

    Root *getRoot() override;

    std::shared_ptr<SDL_Texture> getTexture(SDL_Renderer *renderer, const std::string &path);

    [[nodiscard]] SDL_Renderer * getRenderer() const;

    [[nodiscard]] SDL_Window * getWindow() const;

private:
    std::unordered_map<SDL_Renderer *, std::unordered_map<std::string, std::weak_ptr<SDL_Texture> >> rendererTextureCache;
    SDL_Renderer *renderer;
    SDL_Window *window;
};


#endif //ENGINE_ROOT_H
