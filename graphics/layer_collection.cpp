#include "graphics/image.hpp"
#include "graphics/animation.hpp"
#include "graphics/layer_collection.hpp"

#include <algorithm>

namespace Polarity {

Tileset::Tileset(const std::string& directory,
        const tmxparser::TmxTileset& tileset)
        : tmxparser::TmxTileset(tileset) {
    image = Image::get(directory + "/" + tileset.image.source);
}

Rect Tileset::positionInImage(int tileindex) {
    int effective_image_width = image->width() + 2 * tileMarginInImage + tileSpacingInImage;
    int xoff = tileindex * (tileWidth + tileSpacingInImage);
    int effective_img_xoff = xoff % effective_image_width;
    int effective_img_yoff = (((xoff - effective_img_xoff) / effective_image_width)
            * (tileHeight + tileSpacingInImage));
    int img_xoff = effective_img_xoff + tileMarginInImage - tileSpacingInImage;
    int img_yoff = effective_img_yoff + tileMarginInImage - tileSpacingInImage;
    return Rect(img_xoff, img_yoff, tileWidth, tileHeight);
}

void Tileset::drawTile(int tileindex, SDL_Surface *surf, int x, int y) {
    Rect srcpos = positionInImage(tileindex);
    image->draw(surf, srcpos, x, y - tileHeight);
}

Layer::Layer(const std::string& directory,
            LayerCollection* layers,
            const tmxparser::TmxLayer &tmxLayer)
        : tmxparser::TmxLayer(tmxLayer),
          layers(layers) {
    xparallax = 1.0f;
    yparallax = 1.0f;
    auto whereX = tmxLayer.propertyMap.find("parallax");
    if (whereX != tmxLayer.propertyMap.end()) {
        sscanf(whereX->second.c_str(), "%f", &xparallax);
        yparallax = xparallax;
    }
    auto whereY = tmxLayer.propertyMap.find("yparallax");
    if (whereY != tmxLayer.propertyMap.end()) {
        sscanf(whereY->second.c_str(), "%f", &yparallax);
    }
    if (tmxLayer.isImageLayer) {
        backgroundImage = Image::get(directory + "/" +
                tmxLayer.backgroundImage.source);
    }
    std::cerr<< "Found the parallax "<<xparallax <<","<<yparallax<<std::endl;
}

/*
 *(0,0)
 *
 *        \/ startx
 * +==================+
 * |      +-----------| <-- starty
 * |      |           |
 * +======+===========+
 */
/*
 *(-startx,-starty)
 *
 *        \/ -startx
 * +------------------+
 * |      +===========| <-- -starty
 * |      |(0,0)      |
 * +------+-----------+
 */


void Layer::draw(SDL_Surface* screen, int startx, int starty) {
    startx *= xparallax;
    starty *= yparallax;
    if (backgroundImage) {
        backgroundImage->draw(screen, startx, starty);
        return;
    }
    int layerWidth = (int)width * layers->tileWidth;
    int layerHeight = (int)height * layers->tileHeight;
    int tileid = 0;
    for (int layerY = 0; layerY < layerHeight; layerY += layers->tileHeight) {
        for (int layerX = 0; layerX < layerWidth; layerX += layers->tileWidth, ++tileid) {
            tmxparser::TmxLayerTile& t = tiles[tileid];
            if (!t.gid) {
                continue;
            }
            Tileset& tileset = *layers->tilesets[t.tilesetIndex];
            tileset.drawTile(t.tileInTilesetIndex, screen,
                    startx + layerX, starty + layerY + layers->tileHeight);
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
        layers.push_back(std::unique_ptr<Layer>(new Layer(directory, this, it)));
    }
}

}
