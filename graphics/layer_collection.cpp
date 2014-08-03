#include "graphics/graphics.hpp"

#include <algorithm>

namespace Polarity {

Tileset::Tileset(const std::string& directory,
        const tmxparser::TmxTileset& tileset)
        : tmxparser::TmxTileset(tileset) {
    image = Image::get(directory + "/" + tileset.image.source);
}

void Tileset::positionInImage(int tileindex, SDL_Rect *outRect) {
    int effective_image_width = image->width() + 2 * tileMarginInImage + tileSpacingInImage;
    int xoff = tileindex * (tileWidth + tileSpacingInImage);
    int effective_img_xoff = xoff % effective_image_width;
    int effective_img_yoff = (((xoff - effective_img_xoff) / effective_image_width)
            * (tileHeight + tileSpacingInImage));
    int img_xoff = effective_img_xoff + tileMarginInImage - tileSpacingInImage;
    int img_yoff = effective_img_yoff + tileMarginInImage - tileSpacingInImage;
    outRect->x = img_xoff;
    outRect->y = img_yoff;
    outRect->w = tileWidth;
    outRect->h = tileHeight;
}

void Tileset::drawTile(int tileindex, SDL_Surface *surf, int x, int y) {
    SDL_Rect srcpos;
    positionInImage(tileindex, &srcpos);
    image->draw(surf, &srcpos, x, y);
}

Layer::Layer(LayerCollection* layers,
            const tmxparser::TmxLayer &tmxLayer)
        : tmxparser::TmxLayer(tmxLayer),
          layers(layers) {
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
    int layerWidth = (int)width;
    int layerHeight = (int)height;
    int endx = std::min(startx + screen->w, startx + (int)width);
    int endy = std::min(starty + screen->h, starty + (int)height);
    startx = std::max(0, startx);
    starty = std::max(0, starty);

    int tileid = 0;
    for (int layerX = 0; layerX < layerWidth; layerX += layers->tileWidth) {
        for (int layerY = 0; layerY < endy; layerY += layers->tileHeight,
                ++tileid) {
            tmxparser::TmxLayerTile& t = tiles[tileid];
            if (!t.gid) {
                continue;
            }
            Tileset& tileset = *layers->tilesets[t.tilesetIndex];
            tileset.drawTile(t.tileInTilesetIndex, screen,
                    startx + layerX, starty + layerY);
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
