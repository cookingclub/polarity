#ifndef POLARITY_GRAPHICS_LAYER_COLLECTION_HPP__
#define POLARITY_GRAPHICS_LAYER_COLLECTION_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "animation.hpp"

#include "tmxparser.h"

#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"

namespace Polarity {

class Tileset : tmxparser::TmxTileset {
    Tileset() = delete;
    Tileset(const Tileset&x) = delete;
    Tileset& operator=(const Tileset&x) = delete;
public:
    Tileset(const std::string& directory,
            const tmxparser::TmxTileset& tileset);

    void positionInImage(int tileindex, SDL_Rect *outRect);
    void drawTile(int tileindex, SDL_Surface *surf, int x, int y);

    std::shared_ptr<Image> image;
};

class LayerCollection;

class Layer : public tmxparser::TmxLayer {
public:
    explicit Layer(const std::string& directory,
            LayerCollection*layers,
            const tmxparser::TmxLayer &tmxLayer);

    void draw(SDL_Surface* screen, int x, int y);
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
