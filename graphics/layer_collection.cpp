#include "graphics/image.hpp"
#include "graphics/animation.hpp"
#include "graphics/layer_collection.hpp"

#include <algorithm>

namespace Polarity {

Tileset::Tileset(Canvas *canvas,
        const std::string& directory,
        const tmxparser::TmxTileset& tileset)
        : tmxparser::TmxTileset(tileset) {
    image = Image::get(canvas, directory + "/" + tileset.image.source);
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
Rect Tileset::getTileOutputBounds(int tileindex, int x, int y) {
    return Rect(x, y - tileHeight, tileWidth, tileHeight);
}
Image::BlitDescription Tileset::drawTile(int tileindex, int x, int y) {
    Rect srcpos = positionInImage(tileindex);
    Image::BlitDescription bd = {srcpos,
                                 static_cast<float>(x + tileWidth / 2.0f),
                                 static_cast<float>(y + tileHeight / 2.0f - tileHeight),
                                 static_cast<float>(tileWidth),
                                 static_cast<float>(tileHeight), 0};
    return bd;
    //image->drawSubimage(surf, srcpos, x, y, 0); // DRH fixme - tileHeight
}

Layer::Layer(Canvas *canvas,
            const std::string& directory,
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
    backgroundImage = nullptr;
    if (tmxLayer.isImageLayer) {
        backgroundImage = Image::get(canvas, directory + "/" +
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

void Layer::draw(Canvas *screen, int startx, int starty) {
    startx *= xparallax;
    starty *= yparallax;
    if (backgroundImage) {
        backgroundImage->draw(screen, startx, starty, 0, 1);
        return;
    }
    if (displayLists.empty()) {
        makeDisplayLists(screen);
    }
    for (const std::unique_ptr<DisplayList>& dl : displayLists) {
        dl->draw(screen, startx, starty);
    }
}
void Layer::makeDisplayLists(Canvas *screen) {
    int layerWidth = (int)width * layers->tileWidth;
    int layerHeight = (int)height * layers->tileHeight;
    int tileid = 0;
    for (std::unique_ptr<Tileset> &tile : layers->tilesets) {
        if (!tile->image->isLoaded()) {
            std::cerr << "Awaiting Image load..."<<std::endl;
            return; // not loaded
        }
    }
    std::vector<std::vector<Image::BlitDescription> >displayListCoords(layers->tilesets.size());
    std::vector<Rect>bounds(layers->tilesets.size());
    for (int layerY = 0; layerY < layerHeight; layerY += layers->tileHeight) {
        for (int layerX = 0; layerX < layerWidth; layerX += layers->tileWidth, ++tileid) {
            tmxparser::TmxLayerTile& t = tiles[tileid];
            if (!t.gid) {
                continue;
            }
            if (t.tilesetIndex >= layers->tilesets.size()) {
                std::cerr << "Tileset index " << t.tilesetIndex << " out of bounds "<<std::endl;
                continue;
            }
            Tileset& tileset = *layers->tilesets[t.tilesetIndex];
            displayListCoords[t.tilesetIndex].push_back(
                tileset.drawTile(t.tileInTilesetIndex,
                                 layerX,
                                 layerY + layers->tileHeight)); // FIXME drh  !??!
            Rect tileImageBounds = tileset.getTileOutputBounds(t.tileInTilesetIndex,
                                                               layerX,
                                                               layerY + layers->tileHeight);
            if (bounds[t.tilesetIndex]) {
                bounds[t.tilesetIndex] = bounds[t.tilesetIndex].unionize(tileImageBounds);
            } else {
                bounds[t.tilesetIndex] = tileImageBounds;
            }
        }
    }
    size_t numTileSets = layers->tilesets.size();
    for (size_t i = 0; i < numTileSets; ++i) {
        if (!displayListCoords[i].empty()) {
            displayLists.emplace_back(screen->makeDisplayList(layers->tilesets[i]->image,
                                                              displayListCoords[i],
                                                              bounds[i]));
        }
    }
}

LayerCollection::LayerCollection(
            Canvas* canvas,
            const std::string& directory,
            const tmxparser::TmxMap &tmxMap)
        : tmxparser::TmxMap(tmxMap) {

    for (auto &it : tmxMap.tilesetCollection) {
        std::cerr << "layercol Found tileset: " << it.name << std::endl;
        tilesets.push_back(std::unique_ptr<Tileset>(
                    new Tileset(canvas, directory, it)));
    }
    for (auto &it : tmxMap.layerCollection) {
        std::cerr << "layercol Found layer: " << it.name << std::endl;
        layers.push_back(std::unique_ptr<Layer>(new Layer(canvas, directory, this, it)));
    }
}

}
