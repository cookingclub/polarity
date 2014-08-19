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
    static SDL_Rect rectToRotatedSDL(const Rect& rect, SDL_Surface *src, SDL_Surface *dst) {
        SDL_Rect out;
        float halfWidth = rect.w * 0.5;
        float halfHeight = rect.h * 0.5;
        float centerX = rect.x + halfWidth;
        float centerY = rect.y + halfHeight;
        float oldHalfWidth = src->w * 0.5;
        float oldHalfHeight = src->h * 0.5;
        float newHalfWidth = dst->w * 0.5;
        float newHalfHeight = dst->h * 0.5;
        halfWidth *= newHalfWidth / oldHalfWidth;
        halfHeight *= newHalfHeight / oldHalfHeight;
        out.x = centerX - halfWidth;
        out.y = centerY - halfHeight;
        out.w = halfWidth * 2.0;
        out.h = halfHeight * 2.0;
        return out;
    }

public:

    SDLImage *loadImage(const std::string &filename) {
        SDLImage *retval = new SDLImage(filename);
        return retval;
    }
    static bool similar(float a, float b, float tol) {
        a -= b;
        if (a < 0) a = -a;
        return a < tol;
    }
    virtual void drawSprite(Image *image,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                            float angle) {
        if (!image->isLoaded()) {
            return;
        }
        if (similar(angle, 0, 3.1415926536 / 180.0)) {
            angle = 0; // if something is almost straight up: just optimize
        }
        SDLImage* sdl_image = static_cast<SDLImage*>(image);
        SDL_Surface * surf = sdl_image->surf;
        float zoom = scaleX != surf->w ? scaleX / surf->w : 1.0;
        if ((angle == 0 && zoom != 1.0) || !SDLCanvas::similar(scaleX * surf->h, scaleY * surf->w,
                                                               0.25f * (surf->h + surf->w))) {
#if 0
            // for some reason SDL doesn't support stretchBlit but does support zoom
            SDL_Surface *tmpImage = SDL_CreateRGBSurface(0, //FLAGS
                                                         scaleX,
                                                         scaleY,
                                                         surf->format->BytesPerPixel * 8,
                                                         surf->format->Rmask,
                                                         surf->format->Gmask,
                                                         surf->format->Bmask,
                                                         surf->format->Amask);
            if (tmpImage->format->format != surf->format->format) {
                std::cerr << "Made RGB surface the same way and got different format "
                          << tmpImage->format->format << " != " << surf->format->format
                          << std::endl;
            }
            SDL_Rect srcr;
            SDL_Rect dstr;
            srcr.x = 0; srcr.y = 0;
            srcr.w = surf->w; srcr.h = surf->h;
            dstr.x = 0; dstr.y = 0;
            dstr.w = tmpImage->w; dstr.h = tmpImage->h;
            SDL_SoftStretch(surf, &srcr, tmpImage, &dstr);
            std::cerr << "Soft stretch not supported" << std::endl;
            surf = tmpImage;
#else
            // std::cerr<< "Zoom " <<scaleX / surf->w<<","<< scaleY / surf->h << std::endl;
            surf = zoomSurface(surf, scaleX / surf->w, scaleY / surf->h, true);
#endif
            zoom = 1.0;
        }
        if (angle != 0 || zoom != 1.0) {
            // std::cerr<< "Zoom " <<zoom<<", rot "<< angle << std::endl;
            SDL_Surface * tmpImage = rotozoomSurface(surf, angle * (180./ 3.1415926536), zoom, 1);
            if (surf != sdl_image->surf) {
                SDL_FreeSurface(surf); // this is if asymmetric zoom was necessary
            }
            surf = tmpImage;
        }
        int surfw = surf->w;
        int surfh = surf->h;
        SDL_Rect sdlsrc;
        sdlsrc.x = 0;
        sdlsrc.y = 0;
        sdlsrc.w = surfw;
        sdlsrc.h = surfh;
        SDL_Rect sdldest;
        sdldest.x = centerX - surfw / 2;
        sdldest.y = centerY - surfh / 2;
        sdldest.w = surfw / 2;
        sdldest.h = surfh / 2;
        SDL_BlitSurface(surf, &sdlsrc, screen, &sdldest);
        if (surf != sdl_image->surf) {
            SDL_FreeSurface(surf); // this is if asymmetric zoom was necessary
        }
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
