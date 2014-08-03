#include "graphics/graphics.hpp"

namespace Polarity {

Tileset::Tileset(const std::string& directory,
        const tmxparser::TmxTileset& tileset)
        : tmxparser::TmxTileset(tileset) {
    image = Image::get(directory + "/" + tileset.image.source);
}

LayerCollection::LayerCollection(
            const std::string& directory,
            const tmxparser::TmxTilesetCollection_t &tmxTilesets,
            const tmxparser::TmxLayerCollection_t &tmxLayers) {

    for (auto &it : tmxTilesets) {
        std::cerr << "layercol Found tileset: " << it.name << std::endl;
        tilesets.push_back(std::unique_ptr<Tileset>(
                    new Tileset(directory, it)));
    }
    for (auto &it : tmxLayers) {
        std::cerr << "layercol Found layer: " << it.name << std::endl;
    }
}

}
