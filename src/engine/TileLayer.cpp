//
// Created by dev on 5/12/26.
//
#include "TileLayer.h"

#include <unordered_map>

#include "Renderer.h"
#include "TileMap.h"


void TileLayer::draw(RenderContext ctx) {
    if (dirty) {
        rebucketLayer();
        dirty = false;
    }

    float tileSize = getTileSize();
    for (const auto& textureBucket: textureBuckets) {
        for (auto &positionalTile: textureBucket.second) {
            Tile *tile = positionalTile.tile;
            auto [x, y] = positionalTile.position;
            SDL_FRect srcRect = tile->tileSet->tiles[tile->tileIdInTileset].atlasRegion;
            SDL_FRect dstrect{x * tileSize, y * tileSize, tileSize, tileSize};
        }
    }
}

float TileLayer::getTileSize() {
    return static_cast<float>(tileMap->getTileSize());
}

void TileLayer::rebucketLayer() {
    textureBuckets.clear();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile *tile = &tiles[y * width + x];
            textureBuckets[tile->tileSet->texture.get()].push_back({{x, y}, tile});
        }
    }
}
