#include "graphics/graphics.hpp"
#include <unordered_map>

namespace Polarity {
SDL_Surface *loadImage(const std::string &filename) {
    SDL_Surface *image, *tmpImage;
    tmpImage = IMG_Load(filename.c_str());
    if (!tmpImage) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    image = SDL_DisplayFormatAlpha(tmpImage);
    SDL_FreeSurface(tmpImage);
    return image;
}

std::unordered_map<std::string, std::weak_ptr<Image>> imagesCache;

Image::Image(const std::string& filename) {
    surf = loadImage(filename);
}

std::shared_ptr<Image> Image::get(const std::string& filename) {
    auto it = imagesCache.find(filename);
    if (it != imagesCache.end()) {
        std::shared_ptr<Image> imPtr = it->second.lock();
        if (imPtr) {
            return imPtr;
        }
    }
    std::shared_ptr<Image> imPtr(new Image(filename));
    imagesCache[filename] = imPtr;
    return imPtr;
}

Image::~Image() {
    SDL_FreeSurface(surf);
}

void Image::draw(SDL_Surface *screen, SDL_Rect *src, int x, int y) {
    SDL_Rect dst;
    dst.w = surf->w;
    dst.h = surf->h;
    dst.x = x;
    dst.y = y;
    SDL_BlitSurface(surf, src, screen, &dst);
}

void Image::draw(SDL_Surface *screen, int x, int y) {
    SDL_Rect src;
    src.w = surf->w;
    src.h = surf->h;
    src.x = 0;
    src.y = 0;
    draw(screen, &src, x, y);
}
}
