#ifndef POLARITY_GRAPHICS_IMAGE_HPP__
#define POLARITY_GRAPHICS_IMAGE_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "rect.hpp"

#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"

namespace Polarity {

class Canvas;

class Image {
protected:
    friend class Animation;
    friend class Canvas;

    Image(){surf = nullptr;};
    Image(const Image&x) = delete;
    Image& operator=(const Image&x) = delete;
    explicit Image(SDL_Surface* surf) : surf(surf) {}
public:
    ~Image();

    static std::shared_ptr<Image> get(const std::string &filename);

    void draw(Canvas *screen, int x, int y);
    void draw(Canvas *screen, const Rect& src, int x, int y);

    int width() { return surf->w; }
    int height() { return surf->h; }
private:
    SDL_Surface *surf;
};

}

#endif
