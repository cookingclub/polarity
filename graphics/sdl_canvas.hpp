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
#include "main/main.hpp"
namespace Polarity {

class SDLImage : public Image {
    friend class SDLCanvas;
    static void loadImageSDL(Image *super,
                             const std::string &filename,
                             const std::shared_ptr<DecodedImage> &image) {
        std::shared_ptr<Image>superShared = Image::getValidImage(filename);
        if (superShared.get() == super) {
            SDLImage *thus = static_cast<SDLImage*>(superShared.get());
            if (image->width == 0 || image->height == 0) {
                std::cerr << "Failed to load image: " << filename << std::endl;
                thus->stage = FAILED;
            } else {
                unsigned char rmask[4] = {0xff,0x0,0x0,0x0};
                unsigned char gmask[4] = {0x0,0xff,0x0,0x0};
                unsigned char bmask[4] = {0x0,0x0,0xff,0x0};
                unsigned char amask[4] = {0x0,0x0,0x0,0xff};
                Uint32 mask[4] = {0, 0, 0, 0};
                memcpy(&mask[0], rmask, 4);
                if (image->format == DecodedImage::LA || image->format == DecodedImage::L) {
                    memcpy(&mask[1], rmask, 4);
                    memcpy(&mask[2], rmask, 4);
                } else {
                    memcpy(&mask[1], gmask, 4);
                    memcpy(&mask[2], bmask, 4);
                }
                if (image->format == DecodedImage::LA || image->format == DecodedImage::RGBA) {
                    if (image->format == DecodedImage::LA) {
                        memcpy(&mask[3], gmask, 4);
                    } else {
                        memcpy(&mask[3], amask, 4);
                    }
                }
                SDL_Surface *tmpImage = SDL_CreateRGBSurface(0, //FLAGS
                                                              image->width,
                                                              image->height,
                                                              image->pixelSize() * 8,
                                                              mask[0], mask[1], mask[2], mask[3]);
                if (SDL_LockSurface(tmpImage) == 0) {
                    if (tmpImage->pixels) {
                        unsigned char * pixels = static_cast<unsigned char*>(tmpImage->pixels);
                        unsigned int pixelSize = image->pixelSize();
                        unsigned int height = image->height;
                        unsigned int pitch = pixelSize * image->width;
                        unsigned int dpitch = tmpImage->pitch;
                        for (unsigned int i = 0; i < height; ++i) {
                            memcpy(pixels + dpitch * i,
                                   &image->data[pitch * i],
                                   pitch);
                        }
                    }
                    SDL_UnlockSurface(tmpImage);
                } else {
                    std::cerr << "Unable to lock surface " << filename << std::endl;
                }
                //tmpImage = IMG_Load(filename.c_str());
                if (!tmpImage) {
                    thus->stage = FAILED;
                } else {
                    thus->surf = SDL_DisplayFormatAlpha(tmpImage);
                    SDL_FreeSurface(tmpImage);
                    thus->stage = COMPLETE;
                }
            }
        }
    }
    void downloadAndLoad(){
        Image * super = this;
        Polarity::asyncFileLoad(filename,
                                std::bind(Image::parseAndLoad,
                                          super,
                                          filename,
                                          &SDLImage::loadImageSDL,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
    }
public:
    //    explicit SDLImage(SDL_Surface* surf) : surf(surf) {}
    SDLImage(const std::string&filename) : Image(filename) {
        surf = nullptr;
        if (!filename.empty()) {
            downloadAndLoad();
        }
    }
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
        SDLImage *retval = new SDLImage(filename);
        return retval;/*
        SDL_Surface *image, *tmpImage;
        tmpImage = IMG_Load(filename.c_str());
        if (!tmpImage) {
            std::cerr << "Failed to load image: " << filename << std::endl;
            return nullptr;
        }
        image = SDL_DisplayFormatAlpha(tmpImage);
        SDL_FreeSurface(tmpImage);
        return new SDLImage(image);*/
    }

    virtual void drawImage(Image *image, const Rect &src, const Rect &dst) {
        if (image->isLoaded()) {
            SDLImage* sdl_image = static_cast<SDLImage*>(image);
            SDL_Rect sdlsrc (rectToSDL(src));
            SDL_Rect sdldest (rectToSDL(dst));
            SDL_BlitSurface(sdl_image->surf, &sdlsrc, screen, &sdldest);
        }
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
