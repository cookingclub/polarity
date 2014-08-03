#include "graphics/graphics.hpp"

#include <algorithm>

namespace Polarity {

Tileset::Tileset(const std::string& directory,
        const tmxparser::TmxTileset& tileset)
        : tmxparser::TmxTileset(tileset) {
    image = Image::get(directory + "/" + tileset.image.source);
}

Layer::Layer(LayerCollection* layers,
            const tmxparser::TmxLayer &tmxLayer)
        : tmxparser::TmxLayer(tmxLayer),
          layers(layers) {
}

void Layer::draw(SDL_Surface* screen, int startx, int starty) {
    int endx = std::min(startx + screen->w, startx + (int)width);
    int endy = std::min(starty + screen->h, starty + (int)height);
    startx = std::max(0, startx);
    starty = std::max(0, starty);
    for (int x = startx; x < endx; x += layers->tileWidth) {
        for (int y = starty; y < endy; y += layers->tileHeight) {
        }
    }
}

LayerCollection::LayerCollection(
            const std::string& directory,
            const tmxparser::TmxMap &tmxMap)
        : tmxparser::TmxMap(tmxMap) {

    for (auto &it : tmxMap.tilesetCollection) {
        std::cerr << "layercol Found tileset: " << it.name << std::endl;
        tilesets.push_back(std::unique_ptr<Tileset>(
                    new Tileset(directory, it)));
    }
    for (auto &it : tmxMap.layerCollection) {
        std::cerr << "layercol Found layer: " << it.name << std::endl;
        layers.push_back(std::unique_ptr<Layer>(new Layer(this, it)));
    }
}

}
