#include "graphics/image.hpp"
#include "graphics/canvas.hpp"
#include <unordered_map>

namespace Polarity {
std::unordered_map<std::string, std::weak_ptr<Image>> imagesCache;

std::shared_ptr<Image> Image::get(const std::string& filename) {
    auto it = imagesCache.find(filename);
    if (it != imagesCache.end()) {
        std::shared_ptr<Image> imPtr = it->second.lock();
        if (imPtr) {
            return imPtr;
        }
    }
    std::shared_ptr<Image> imPtr(new Image(Canvas::loadImage(filename)));
    imagesCache[filename] = imPtr;
    return imPtr;
}

Image::~Image() {
    if (surf != nullptr) {
        SDL_FreeSurface(surf);
    }
}

void Image::draw(Canvas *screen, const Rect& src, int x, int y) {
    Rect dest(x, y, surf->w, surf->h);
    screen->drawImage(this, src, dest);
}

void Image::draw(Canvas *screen, int x, int y) {
    Rect src (0, 0, surf->w, surf->h);
    draw(screen, src, x, y);
}

}
