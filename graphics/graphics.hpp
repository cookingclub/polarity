#ifndef POLARITY_GRAPHICS_GRAPHICS_HPP__
#define POLARITY_GRAPHICS_GRAPHICS_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "tmxparser.h"

#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"

namespace Polarity {

class Image {
    Image() = delete;
    Image(const Image&x) = delete;
    Image& operator=(const Image&x) = delete;
    explicit Image(const std::string &filename);
public:
    ~Image();

    static std::shared_ptr<Image> get(const std::string &filename);

    void draw(SDL_Surface *surf, int x, int y);
private:
    SDL_Surface *surf;
};

class Tileset : tmxparser::TmxTileset {
    Tileset() = delete;
    Tileset(const Tileset&x) = delete;
    Tileset& operator=(const Tileset&x) = delete;
public:
    Tileset(const std::string& directory,
            const tmxparser::TmxTileset& tileset);

    std::shared_ptr<Image> image;
};

class LayerCollection {
    LayerCollection() = delete;
    LayerCollection(const LayerCollection&x) = delete;
    LayerCollection& operator=(const LayerCollection&x) = delete;
public:
    explicit LayerCollection(
            const std::string& directory,
            const tmxparser::TmxTilesetCollection_t &tmxTilesets,
            const tmxparser::TmxLayerCollection_t &tmxLayers);


private:

    std::vector<std::unique_ptr<Tileset>> tilesets;
};

}

#endif
