//
// Created by dev on 4/14/26.
//

#ifndef MYENGINE_GAMEOBJECT_H
#define MYENGINE_GAMEOBJECT_H
#include <memory>
#include <vector>

#include "RenderContext.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

class Root;
struct State;

class GameObject {
public:
    explicit GameObject(const SDL_FPoint position) : _position(position), parent(nullptr),
                                                                         markedForRemoval(false) {
    };
    virtual ~GameObject() = default;
    GameObject(GameObject&) = delete;
    GameObject& operator=(GameObject&) = delete;

    /**
     * First function called when child is in the Scene tree, before first update, will be called once updating is set to true
     */
    virtual void initialize() {};
    virtual void update(float dt) {};
    virtual void draw(RenderContext ctx) {};

    [[nodiscard]] SDL_FPoint& position();
    [[nodiscard]] const SDL_FPoint& getPosition() const;

    void queueAddChild(std::unique_ptr<GameObject> child);
    void queueRemove();

    virtual RenderContext modifyRenderContext(RenderContext ctx);

    virtual Root* getRoot();
    [[nodiscard]] GameObject* getParent() const;
    [[nodiscard]] const std::vector<std::unique_ptr<GameObject>>& getChildren() const;

    void setUpdating(bool updating);
private:
    friend class Root;
    bool initialized = false;
    SDL_FPoint _position;
    GameObject* parent;
    std::vector<std::unique_ptr<GameObject>> children;
    std::vector<std::unique_ptr<GameObject>> newChildren;
    bool markedForRemoval;
    bool updating;
    void _update(float dt);
    void _draw(RenderContext ctx);
    void _addChildren();
    void _clean();
};


#endif //ENGINE_GAMEOBJECT_H