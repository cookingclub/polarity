#include "graphics/image.hpp"
#include "graphics/sdl_canvas.hpp"
#include <unordered_map>

namespace Polarity {
std::unordered_map<std::string, std::weak_ptr<Image>> imagesCache;

std::shared_ptr<Image> Image::get(Canvas *canvas, const std::string& filename) {
    auto it = imagesCache.find(filename);
    if (it != imagesCache.end()) {
        std::shared_ptr<Image> imPtr = it->second.lock();
        if (imPtr) {
            return imPtr;
        }
    }
    std::shared_ptr<Image> imPtr(canvas->loadImage(filename));
    imagesCache[filename] = imPtr;
    return imPtr;
}

void Image::drawSubimage(Canvas *screen, const Rect& src, int x, int y) {
    Rect dest(x, y, src.width(), src.height());
    screen->drawImage(this, src, dest);
}

void Image::draw(Canvas *screen, int x, int y) {
    Rect src (0, 0, width(), height());
    drawSubimage(screen, src, x, y);
}

}
