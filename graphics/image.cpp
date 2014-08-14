#include "graphics/image.hpp"
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
    if (surf != nullptr) {
        SDL_FreeSurface(surf);
    }
}

SDL_Rect rectToSDL(const Rect& rect) {
    SDL_Rect out;
    out.x = rect.x;
    out.y = rect.y;
    out.w = rect.w;
    out.h = rect.h;
    return out;
}

void Image::draw(SDL_Surface *screen, const Rect& src, int x, int y) {
    SDL_Rect dst;
    dst.w = surf->w;
    dst.h = surf->h;
    dst.x = x;
    dst.y = y;
    SDL_Rect sdlsrc (rectToSDL(src));
    SDL_BlitSurface(surf, &sdlsrc, screen, &dst);
}

void Image::draw(SDL_Surface *screen, int x, int y) {
    Rect src (0, 0, surf->w, surf->h);
    draw(screen, src, x, y);
}

}
