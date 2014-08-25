#ifndef POLARITY_GRAPHICS_LAYER_COLLECTION_HPP__
#define POLARITY_GRAPHICS_LAYER_COLLECTION_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "animation.hpp"
#include "display_list.hpp"
#include "rect.hpp"

#include "tmxparser.h"

namespace Polarity {
class Canvas;

class Tileset : tmxparser::TmxTileset {
    Tileset() = delete;
    Tileset(const Tileset&x) = delete;
    Tileset& operator=(const Tileset&x) = delete;
public:
    Tileset(Canvas *canvas,
            const std::string& directory,
            const tmxparser::TmxTileset& tileset);
    unsigned int getTileWidth()const {
        return tileWidth;
    }
    unsigned int getTileHeight()const {
        return tileHeight;
    }
    Rect positionInImage(int tileindex);
    Image::BlitDescription drawTile(int tileindex, int x, int y);
    Rect getTileOutputBounds(int tileindex, int x, int y);
    std::shared_ptr<Image> image;
};

class LayerCollection;

class Layer : public tmxparser::TmxLayer {
public:
    explicit Layer(Canvas *canvas,
            const std::string& directory,
            LayerCollection*layers,
            const tmxparser::TmxLayer &tmxLayer);

    void draw(Canvas* screen, int x, int y);
    float xparallax;
    float yparallax;
    LayerCollection *layers;

    std::shared_ptr<Image> backgroundImage;
    void makeDisplayLists(Canvas *screen);
    std::vector<std::unique_ptr<DisplayList> > displayLists;
};

class LayerCollection : public tmxparser::TmxMap {
    LayerCollection() = delete;
    LayerCollection(const LayerCollection&x) = delete;
    LayerCollection& operator=(const LayerCollection&x) = delete;
public:
    explicit LayerCollection(
            Canvas *canvas,
            const std::string& directory,
            const tmxparser::TmxMap &tmxMap);

    std::vector<std::unique_ptr<Tileset>> tilesets;
    std::vector<std::unique_ptr<Layer>> layers;
};

}

#endif
