#include "graphics/graphics.hpp"

namespace Polarity {
SDL_Surface *loadImage(const std::string &filename) {
    SDL_Surface *image, *tmpImage;
    tmpImage = IMG_Load(filename.c_str());
    image = SDL_DisplayFormatAlpha(tmpImage);
    SDL_FreeSurface(tmpImage);
    return image;
}
}
