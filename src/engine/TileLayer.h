//
// Created by dev on 5/11/26.
//

#ifndef MYENGINE_TILELAYER_H
#define MYENGINE_TILELAYER_H
#include <unordered_map>
#include <vector>

#include "GameObject.h"
#include "Tile.h"

class TileMap;

class TileLayer : public GameObject {
public:
    TileLayer(const SDL_FPoint position, int width, int height, TileMap *tileMap) : GameObject({.position = position}),
        width(width), height(height), tileMap(tileMap) {
    };

    void draw(RenderContext ctx) override;

    float getTileSize();

private:
    bool dirty = true;
    struct PositionalTile {
        SDL_Point position;
        Tile *tile;
    };
    std::unordered_map<SDL_Texture *, std::vector<PositionalTile> > textureBuckets;
    int width, height;
    std::vector<Tile> tiles;
    TileMap *tileMap;

    void rebucketLayer();
};

#endif //MYENGINE_TILELAYER_H
