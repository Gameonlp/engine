//
// Created by dev on 4/14/26.
//

#ifndef MYENGINE_GAMEOBJECT_H
#define MYENGINE_GAMEOBJECT_H
#include <memory>
#include <vector>

#include "RenderContext.h"
#include "RenderJob.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

class Root;
struct State;

class GameObject {
public:
    struct GameObjectConfig {
        SDL_FPoint position{0, 0};
        int zIndex{0};
        bool absoluteZIndex{false};
    };

    explicit GameObject(const GameObjectConfig &config) : _position(config.position), zIndex(config.zIndex),
                                                          absoluteZIndex(config.absoluteZIndex), initialized(true),
                                                          updating(false), parent(nullptr),
                                                          markedForRemoval(false) {
    };

    virtual ~GameObject() = default;

    GameObject(GameObject &) = delete;

    GameObject &operator=(GameObject &) = delete;

    /**
     * First function called when child is in the Scene tree, before first update, will be called once updating is set to true
     */
    virtual void initialize() {
    };

    virtual void update(float dt) {
    };

    virtual void draw(RenderContext ctx) {
    };

    [[nodiscard]] SDL_FPoint &position();

    [[nodiscard]] const SDL_FPoint &getPosition() const;

    void queueAddChild(std::unique_ptr<GameObject> child);

    void queueRemove();

    virtual RenderContext modifyRenderContext(RenderContext ctx);

    virtual Root *getRoot();

    [[nodiscard]] GameObject *getParent() const;

    [[nodiscard]] const std::vector<std::unique_ptr<GameObject> > &getChildren() const;

    void setUpdating(bool updating);
private:
    friend class Root;
    int zIndex;
    bool absoluteZIndex;
    bool initialized;
    SDL_FPoint _position;
    GameObject *parent;
    std::vector<std::unique_ptr<GameObject> > children;
    std::vector<std::unique_ptr<GameObject> > newChildren;
    bool markedForRemoval;
    bool updating;

    void _update(float dt);

    void _draw(RenderContext ctx, std::vector<RenderJob> &jobs, int zIndex);

    void _addChildren();

    void _clean();
};


#endif //ENGINE_GAMEOBJECT_H
