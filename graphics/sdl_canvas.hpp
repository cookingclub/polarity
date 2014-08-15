#ifndef POLARITY_GRAPHICS_SDL_CANVAS_HPP__
#define POLARITY_GRAPHICS_SDL_CANVAS_HPP__

#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"

#include "image.hpp"
#include "canvas.hpp"

namespace Polarity {

class SDLImage : public Image {
    friend class SDLCanvas;
public:
    explicit SDLImage(SDL_Surface* surf) : surf(surf) {}
    SDLImage(){surf = nullptr;};
    virtual ~SDLImage() {
        if (surf != nullptr) {
            SDL_FreeSurface(surf);
        }
    }

    virtual int width() { return surf->w; }
    virtual int height() { return surf->h; }

private:
    SDL_Surface *surf;
};

class SDLCanvas : public Canvas {
public:
    SDLCanvas(SDL_Surface* surf)
        : screen(surf) {
    }
    SDLCanvas(int width, int height) {
        screen = SDL_SetVideoMode(
                width, height, 0,
                SDL_HWSURFACE | SDL_RESIZABLE);
    }

    virtual int width() { return screen->w; }
    virtual int height() { return screen->h; }

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

    SDLImage *loadImage(const std::string &filename) {
        SDL_Surface *image, *tmpImage;
        tmpImage = IMG_Load(filename.c_str());
        if (!tmpImage) {
            std::cerr << "Failed to load image: " << filename << std::endl;
            return nullptr;
        }
        image = SDL_DisplayFormatAlpha(tmpImage);
        SDL_FreeSurface(tmpImage);
        return new SDLImage(image);
    }

    virtual void drawImage(Image *image, const Rect &src, const Rect &dst) {
        SDLImage* sdl_image = static_cast<SDLImage*>(image);
        SDL_Rect sdlsrc (rectToSDL(src));
        SDL_Rect sdldest (rectToSDL(dst));
        SDL_BlitSurface(sdl_image->surf, &sdlsrc, screen, &sdldest);
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
