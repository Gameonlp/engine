//
// Created by dev on 4/14/26.
//

#include "GameObject.h"

#include <memory>

#include "Utils.h"

SDL_FPoint& GameObject::position() {
    return _position;
}

const SDL_FPoint& GameObject::getPosition() const {
    return _position;
}

void GameObject::queueAddChild(std::unique_ptr<GameObject> child) {
    child->parent = this;
    child->updating = true;
    newChildren.push_back(std::move(child));
}

void GameObject::queueRemove() {
    markedForRemoval = true;
    for (auto& child : children) {
        child->queueRemove();
    }
}

inline RenderContext GameObject::modifyRenderContext(RenderContext ctx) {
    return ctx;
}

Root *GameObject::getRoot() {
    if (getParent() != nullptr) {
        return getParent()->getRoot();
    }
    return nullptr;
}

GameObject* GameObject::getParent() const {
    return parent;
}

const std::vector<std::unique_ptr<GameObject>> &GameObject::getChildren() const {
    return children;
}

void GameObject::setUpdating(bool updating) {
    this->updating = updating;
}

void GameObject::_update(float dt) {
    if (!updating) {
        return;
    }
    if (!initialized) {
        initialize();
        initialized = true;
    }
    update(dt);
    for (const auto& child : children) {
        child->_update(dt);
    }
}

void GameObject::_draw(RenderContext ctx) {
    ctx = modifyRenderContext(ctx);
    if (absoluteZIndex) {
        ctx.zIndex = this->zIndex;
    } else {
        ctx.zIndex += this->zIndex;
    }
    draw(ctx);
    for (const auto& child : children) {
        child->_draw(ctx);
    }
}

void GameObject::_addChildren() {
    children.insert(children.end(),
           std::make_move_iterator(newChildren.begin()),
           std::make_move_iterator(newChildren.end()));
    for (const auto& child : children) {
        child->_addChildren();
    }
}

void GameObject::_clean() {
    for (auto it = children.begin(); it != children.end(); ) {
        it->get()->_clean();
        if (it->get()->markedForRemoval) {
            it = children.erase(it);
        } else {
            ++it;
        }
    }
}
