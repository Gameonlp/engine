//
// Created by dev on 4/15/26.
//

#include "Utils.h"

#include <algorithm>
#include <math.h>

#include "Camera.h"
#include "SDLExtensions.h"
#include "GameObject.h"

void Utils::globalPosition(SDL_FPoint &position, GameObject *object) {
    position.x = 0;
    position.y = 0;

    const GameObject *current = object;
    while (current) {
        const SDL_FPoint curPos = current->getPosition();
        position += curPos;
        current = current->getParent();
    }
}

void Utils::viewPosition(SDL_FPoint &position, GameObject *object, RenderContext context) {
    globalPosition(position, object);
    if (const auto *camera = context.camera) {
        position -= camera->cameraPosition();
    }
}

template <typename T>
T Utils::square(T toSquare) {
    return toSquare * toSquare;
}

bool Utils::rect_tri_intersect(const SDL_FRect rect, const SDL_FPoint a, const SDL_FPoint b, const SDL_FPoint c) {
    const SDL_FPoint extent = {rect.w / 2, rect.h / 2};
    const SDL_FPoint centerVector = {-rect.x - extent.x, -rect.y - extent.y };

    SDL_FPoint centerA = a + centerVector, centerB = b + centerVector, centerC = c + centerVector;

    const SDL_FPoint edges[3] = {centerA - centerB, centerB - centerC, centerC - centerA};

    if (std::max({centerA.x, centerB.x, centerC.x}) < -extent.x || std::min({centerA.x, centerB.x, centerC.x}) > extent.x) {
        return false;
    }
    if (std::max({centerA.y, centerB.y, centerC.y}) < -extent.y || std::min({centerA.y, centerB.y, centerC.y}) > extent.y) {
        return false;
    }

    const bool allEdgesOverlap = std::ranges::all_of(edges, [&](const auto& edge) {
        const SDL_FPoint axis = {-edge.y, edge.x};

        float p0 = dot(centerA, axis);
        float p1 = dot(centerB, axis);
        float p2 = dot(centerC, axis);

        const float triMin = std::min({p0, p1, p2});
        const float triMax = std::max({p0, p1, p2});

        const float boxRadius = extent.x * std::abs(axis.x) + extent.y * std::abs(axis.y);

        return !(triMax < -boxRadius || triMin > boxRadius);
    });

    return allEdgesOverlap;
}

void Utils::rect(SDL_FRect &rect, const SDL_FPoint pos, const SDL_FPoint size) {
    Utils::rect(rect, pos, size.x, size.y);
}

void Utils::rect(SDL_FRect &rect, const SDL_FPoint pos, const float w, const float h) {
    rect.x = pos.x;
    rect.y = pos.y;
    rect.w = w;
    rect.h = h;
}

float Utils::dot(SDL_FPoint a, SDL_FPoint b) {
    return a.x * b.x + a.y * b.y;
}

SDL_FColor Utils::premultiply(const SDL_FColor color) {
    SDL_FColor other;
    other.r = color.r * color.a;
    other.g = color.g * color.a;
    other.b = color.b * color.a;
    other.a = color.a;
    return other;
}

SDL_FPoint Utils::rotateAroundOrigin(const SDL_FPoint vertex, const float angleDegrees) {
    const float radians = (angleDegrees * std::numbers::pi_v<float>) / 180.0f;
    const float cosA = std::cos(radians);
    const float sinA = std::sin(radians);

    SDL_FPoint rotated_pt;
    rotated_pt.x = vertex.x * cosA - vertex.y * sinA;
    rotated_pt.y = vertex.x * sinA + vertex.y * cosA;

    return rotated_pt;
}

uint32_t hash(std::string toHash) {
    return std::hash<std::string>{}(toHash);
}
