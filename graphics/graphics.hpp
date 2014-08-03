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
    void draw(SDL_Surface *screen, SDL_Rect *src, int x, int y);
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

class LayerCollection;

class Layer : public tmxparser::TmxLayer {
public:
    explicit Layer(LayerCollection*layers,
            const tmxparser::TmxLayer &tmxLayer);

    void draw(SDL_Surface* screen, int x, int y);

    LayerCollection *layers;
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
