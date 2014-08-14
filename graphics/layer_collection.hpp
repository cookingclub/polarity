#ifndef POLARITY_GRAPHICS_LAYER_COLLECTION_HPP__
#define POLARITY_GRAPHICS_LAYER_COLLECTION_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "animation.hpp"
#include "rect.hpp"

#include "tmxparser.h"

namespace Polarity {

class Tileset : tmxparser::TmxTileset {
    Tileset() = delete;
    Tileset(const Tileset&x) = delete;
    Tileset& operator=(const Tileset&x) = delete;
public:
    Tileset(const std::string& directory,
            const tmxparser::TmxTileset& tileset);

    Rect positionInImage(int tileindex);
    void drawTile(int tileindex, Canvas *surf, int x, int y);

    std::shared_ptr<Image> image;
};

class LayerCollection;

class Layer : public tmxparser::TmxLayer {
public:
    explicit Layer(const std::string& directory,
            LayerCollection*layers,
            const tmxparser::TmxLayer &tmxLayer);

    void draw(Canvas* screen, int x, int y);
    float xparallax;
    float yparallax;
    LayerCollection *layers;

    std::shared_ptr<Image> backgroundImage;
};

class LayerCollection : public tmxparser::TmxMap {
    LayerCollection() = delete;
    LayerCollection(const LayerCollection&x) = delete;
    LayerCollection& operator=(const LayerCollection&x) = delete;
public:
    explicit LayerCollection(
            const std::string& directory,
            const tmxparser::TmxMap &tmxMap);

    std::vector<std::unique_ptr<Tileset>> tilesets;
    std::vector<std::unique_ptr<Layer>> layers;
};

}

#endif
