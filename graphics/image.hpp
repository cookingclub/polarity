#ifndef POLARITY_GRAPHICS_IMAGE_HPP__
#define POLARITY_GRAPHICS_IMAGE_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"

namespace Polarity {

class Image {
protected:
    friend class Animation;
    Image(){surf = nullptr;};
    Image(const Image&x) = delete;
    Image& operator=(const Image&x) = delete;
    explicit Image(const std::string &filename);
public:
    ~Image();

    static std::shared_ptr<Image> get(const std::string &filename);

    void draw(SDL_Surface *screen, int x, int y);
    void draw(SDL_Surface *screen, SDL_Rect *src, int x, int y);

    int width() { return surf->w; }
    int height() { return surf->h; }
protected:
    SDL_Surface *surf;
};

}

#endif
