#ifndef POLARITY_GRAPHICS_CANVAS_HPP__
#define POLARITY_GRAPHICS_CANVAS_HPP__

#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"

#include "image.hpp"

namespace Polarity {
class Canvas {
    Canvas& operator= (const Canvas&) = delete;
    Canvas(const Canvas&) = delete;
public:
    Canvas(SDL_Surface* surf)
        : screen(surf) {
    }
    Canvas(int width, int height) {
        screen = SDL_SetVideoMode(
                width, height, 0,
                SDL_HWSURFACE | SDL_RESIZABLE);
    }

    virtual int width() { return screen->w; }
    virtual int height() { return screen->h; }

    virtual ~Canvas() {}

private:
    static SDL_Rect rectToSDL(const Rect& rect) {
        SDL_Rect out;
        out.x = rect.x;
        out.y = rect.y;
        out.w = rect.w;
        out.h = rect.h;
        return out;
    }

public:

    static SDL_Surface *loadImage(const std::string &filename) {
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

    virtual void drawImage(Image *image, const Rect &src, const Rect &dst) {
        SDL_Rect sdlsrc (rectToSDL(src));
        SDL_Rect sdldest (rectToSDL(dst));
        SDL_BlitSurface(image->surf, &sdlsrc, screen, &sdldest);
    }

    virtual void swapBuffers() {
#ifndef EMSCRIPTEN
        SDL_Flip(screen);
#endif
    }

    SDL_Surface* screen;
};

}

#endif
