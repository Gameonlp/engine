//
// Created by dev on 5/11/26.
//

#ifndef MYENGINE_TILEMAP_H
#define MYENGINE_TILEMAP_H
#include <unordered_map>

#include "GameObject.h"
#include "TileLayer.h"
#include "TileSet.h"

class TileLayer;

class TileMap : public GameObject {
public:
    TileMap();

    static TileMap load(std::string path);

    int getTileSize();
private:
    std::vector<TileLayer> layers;
    std::vector<TileSet> tileSets;
    std::unordered_map<size_t, void*> extraData;
};


#endif //MYENGINE_TILEMAP_H