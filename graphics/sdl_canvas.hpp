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
#include "display_list.hpp"
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
                    //thus->surf = SDL_ConvertSurface(tmpImage, canvas->surface->pixels, 0);
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

    virtual int width() { return surf ? surf->w : 0; }
    virtual int height() { return surf ? surf->h : 0; }
    virtual void enableAlphaBlend() {
        if (surf != nullptr) {
#ifdef SDL_SRCALPHA
            SDL_SetAlpha(surf, SDL_SRCALPHA, 0xff);
#else
            SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_BLEND);
#endif
        }
    }
    virtual void disableAlphaBlend() {
        if (surf != nullptr) {
#ifdef SDL_SRCALPHA
            SDL_SetAlpha(surf, 0, 0xff);//surf->format->alpha
#else
            SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_NONE);
#endif
        }
    }
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
    class SDLDisplayList : public DisplayList {
    protected:
        std::weak_ptr<Image> image;
        std::vector<Image::BlitDescription> dl;
    public:
        SDLDisplayList(SDLCanvas *canvas,
                       const std::shared_ptr<Image> &image,
                       const std::vector<Image::BlitDescription> &draws):image(image), dl(draws) {
        }
        void attach(const std::shared_ptr<Image>&newImage) {
            image = newImage;
        }
        void draw(Canvas *canvas, int x, int y) const {
            std::shared_ptr<Image> img (image.lock());
            if (img) {
                for (auto blit : dl) {
                    static_cast<SDLCanvas*>(canvas)->drawSpriteSrc(
                        img.get(), blit.src, blit.centerX + x, blit.centerY + y,
                        blit.scaleX, blit.scaleY, blit.angle);
                }
            }
        }
    };
    class SDLImageCacheDisplayList : public SDLDisplayList {
        Rect bounds;
        std::unique_ptr<SDLCanvas> cache;
        static SDLCanvas * makeBlankDrawableSurface(int w, int h) {
            unsigned char rmask[4] = {0xff,0x0,0x0,0x0};
            unsigned char gmask[4] = {0x0,0xff,0x0,0x0};
            unsigned char bmask[4] = {0x0,0x0,0xff,0x0};
            unsigned char amask[4] = {0x0,0x0,0x0,0xff};
            Uint32 mask[4] = {0, 0, 0, 0};
            memcpy(&mask[0], rmask, 4);
            memcpy(&mask[1], gmask, 4);
            memcpy(&mask[2], bmask, 4);
            memcpy(&mask[3], amask, 4);
            SDL_Surface *tmpImage = SDL_CreateRGBSurface(0, //FLAGS
                                 w,
                                 h,
                                 32,
                                 mask[0],
                                 mask[1],
                                 mask[2],
                                 mask[3]);
            SDLCanvas * retval = new SDLCanvas(SDL_DisplayFormatAlpha(tmpImage));
            SDL_FreeSurface(tmpImage);
            return retval;
        }
    public:
        SDLImageCacheDisplayList(SDLCanvas *canvas,
                                  const std::shared_ptr<Image> &image,
                                  const std::vector<Image::BlitDescription> &lst,
                                  const Rect &bound)
            : SDLDisplayList(canvas, image, lst),
              bounds(bound),
              cache(makeBlankDrawableSurface(bound.w, bound.h)) {
            loadImage();
        }
        void loadImage() {
            std::shared_ptr<Image> img(image.lock());
            // this lets us copy the alpha channel directly
            static_cast<SDLImage*>(img.get())->disableAlphaBlend();
            if (img) {
                for(Image::BlitDescription blit : dl) {
                    cache.get()->drawSpriteSrc(
                        img.get(), blit.src,
                        blit.centerX - bounds.x,
                        blit.centerY - bounds.y,
                        blit.scaleX, blit.scaleY, blit.angle);
                }
            }
            static_cast<SDLImage*>(img.get())->enableAlphaBlend();
        }
        void reloadImage() {
            SDL_Rect full;
            full.x = 0;
            full.y = 0;
            full.w = cache->screen->w;
            full.h = cache->screen->h;
            SDL_FillRect(cache->screen, &full, 0);
            loadImage();
        }
        void attach(const std::shared_ptr<Image> &newImage) {
            SDLDisplayList::attach(newImage);
            reloadImage();
        }
        void draw(Canvas *canvas, int x, int y) const {
            SDL_Rect src;
            SDL_Rect dest;
            src.x = 0;
            src.y = 0;
            src.w = cache->screen->w;
            src.h = cache->screen->h;
            dest.x = static_cast<int>(x + bounds.x);
            dest.y = static_cast<int>(y + bounds.y);
            dest.w = src.w;
            dest.h = src.h;
            SDL_BlitSurface(cache->screen, &src, static_cast<SDLCanvas*>(canvas)->screen, &dest);
        }
    };
    virtual DisplayList *makeDisplayList(const std::shared_ptr<Image> &image,
                                         const std::vector<Image::BlitDescription> &draws,
                                         const Rect&bounds) {
        if (bounds.w && bounds.h) {
            return new SDLImageCacheDisplayList(this, image, draws, bounds);
        } else {
            return new SDLDisplayList(this, image, draws);
        }
    }
    virtual void attachDisplayList(DisplayList *dl, const std::shared_ptr<Image> &image) {
        dl->attach(image);
    }
    virtual void drawDisplayList(const DisplayList *dl, int x, int y){
        dl->draw(this, x,y);
    }

    virtual void drawSprite(Image *image,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                            float angle) {
        SDLImage* sdl_image = static_cast<SDLImage*>(image);
        SDL_Surface * surf = sdl_image->surf;
        if (!surf) {
            return;
        }
        drawSpriteSrc(image, Rect(0, 0, surf->w, surf->h),
                      centerX, centerY, scaleX, scaleY, angle);
    }
    void drawSpriteSrc(Image *image,
                            const Rect &src,
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
        if (angle == 0 && scaleX == src.w && scaleY == src.h) {
            SDL_Rect sdlsrc (rectToSDL(src));
            SDL_Rect sdldest;
            sdldest.x = centerX - scaleX / 2;
            sdldest.y = centerY - scaleY / 2;
            sdldest.w = sdlsrc.w;
            sdldest.h = sdlsrc.h;
            SDL_BlitSurface(surf, &sdlsrc, screen, &sdldest);
            return;
        }
        if (src.x != 0 || src.y != 0 || src.w != surf->w || src.h != surf->h) {
            SDL_Surface *tmpImage = SDL_CreateRGBSurface(0, //FLAGS
                                                         src.w,
                                                         src.h,
                                                         surf->format->BytesPerPixel * 8,
                                                         surf->format->Rmask,
                                                         surf->format->Gmask,
                                                         surf->format->Bmask,
                                                         surf->format->Amask);
            SDL_Rect sdlsrc, sdldest;
            sdlsrc.x = src.x;
            sdlsrc.y = src.y;
            sdlsrc.w = src.w;
            sdlsrc.h = src.h;
            sdldest.x = 0;
            sdldest.y = 0;
            sdldest.w = src.w;
            sdldest.h = src.h;
            SDL_BlitSurface(surf, &sdlsrc, tmpImage, &sdldest);
            surf = tmpImage;
        }
        float zoom = scaleX != surf->w ? scaleX / surf->w : 1.0;
        if ((angle == 0 && zoom != 1.0) || !SDLCanvas::similar(scaleX * surf->h, scaleY * surf->w,
                                                               0.25f * (surf->h + surf->w))) {
            SDL_Surface *tmpImage = nullptr;
#if 0
            // for some reason SDL doesn't support stretchBlit but does support zoom
            tmpImage = SDL_CreateRGBSurface(0, //FLAGS
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
#else
            // std::cerr<< "Zoom " <<scaleX / surf->w<<","<< scaleY / surf->h << std::endl;
            tmpImage = zoomSurface(surf, scaleX / surf->w, scaleY / surf->h, true);
#endif
            if (surf != sdl_image->surf) {
                SDL_FreeSurface(surf); // this is if asymmetric zoom was necessary
            }
            surf = tmpImage;
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
    virtual void swapBuffers() {
#ifndef EMSCRIPTEN
        SDL_Flip(screen);
#endif
    }

    SDL_Surface* screen;
};

}

#endif
