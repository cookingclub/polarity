#ifndef POLARITY_GRAPHICS_SDL_CANVAS_HPP__
#define POLARITY_GRAPHICS_SDL_CANVAS_HPP__
#ifdef USE_SDL2
#include "SDL2/SDL_version.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#else
#include "SDL/SDL_version.h"
#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"
#endif
#include "image.hpp"
#include "canvas.hpp"
#include "display_list.hpp"
#include "main/main.hpp"
#include "font_manager.hpp"

namespace Polarity {

class SDLImage : public Image {
    friend class SDLCanvas;
    static void loadImageSDL(Image *super,
                             const std::string &filename,
                             const std::shared_ptr<DecodedImage> &image);
    void downloadAndLoad();
public:
    explicit SDLImage(SDL_Surface* surf);
    SDLImage(const std::string&filename);
    virtual ~SDLImage();

    virtual int width() { return w; }
    virtual int height() { return h; }
    virtual void enableAlphaBlend();
    virtual void disableAlphaBlend();
private:
    int w;
    int h;
    SDL_Surface *surf;
#if SDL_MAJOR_VERSION >= 2
    // either surf or texture will be null
    SDL_Texture *texture;
#else
    void *dummy; // to keep same struct size
#endif
};

class SDLContext {
public:
    SDLContext(const SDLContext&) = delete;
    SDLContext& operator=(const SDLContext&) = delete;
    SDLContext();
    ~SDLContext();
#if SDL_MAJOR_VERSION >= 2
    SDL_Window *window;
    SDL_Renderer *renderer;
#else
    void *dummyA;
    void *dummyB;
#endif
};

class SDLCanvas : public Canvas {
public:
#if SDL_MAJOR_VERSION >= 2
    typedef SDL_Texture SDLRenderTargetType;
#else
    typedef SDL_Surface SDLRenderTargetType;
#endif
    SDLCanvas(
              SDLRenderTargetType*surf,
              int width,
              int height,
              SDLCanvas *parent);
    SDLCanvas(int width, int height);
    ~SDLCanvas();
    virtual int width() { return w; }
    virtual int height() { return h; }

    virtual FontManager &fontManager();

private:
    static SDL_Rect rectToSDL(const Rect& rect);
    static SDL_Rect rectToRotatedSDL(const Rect& rect, SDL_Surface *src, SDL_Surface *dst);

public:

    SDLImage *loadImageFromSurface(SDL_Surface *surf);
    SDLImage *loadImage(const std::string &filename);
    static bool similar(float a, float b, float tol);
    class SDLDisplayList : public DisplayList {
    protected:
        std::weak_ptr<Image> image;
        std::vector<Image::BlitDescription> dl;
    public:
        SDLDisplayList(SDLCanvas *canvas,
                       const std::shared_ptr<Image> &image,
                       const std::vector<Image::BlitDescription> &draws);
        void attach(const std::shared_ptr<Image>&newImage);
        void draw(Canvas *canvas, int x, int y) const;
        ~SDLDisplayList();
    };
    class SDLImageCacheDisplayList : public SDLDisplayList {
        Rect bounds;
        std::unique_ptr<SDLCanvas> cache;
        static SDLCanvas * makeBlankDrawableSurface(int w, int h, SDLCanvas *parent);
    public:
        SDLImageCacheDisplayList(SDLCanvas *canvas,
                                  const std::shared_ptr<Image> &image,
                                  const std::vector<Image::BlitDescription> &lst,
                                 const Rect &bound);
        ~SDLImageCacheDisplayList();
        void loadImage();
        void reloadImage();
        void attach(const std::shared_ptr<Image> &newImage);
        void draw(Canvas *canvas, int x, int y) const;
    };
    virtual DisplayList *makeDisplayList(const std::shared_ptr<Image> &image,
                                         const std::vector<Image::BlitDescription> &draws,
                                         const Rect&bounds);
    virtual void attachDisplayList(DisplayList *dl, const std::shared_ptr<Image> &image);
    virtual void drawDisplayList(const DisplayList *dl, int x, int y);

    virtual void drawSprite(Image *image,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                            float angle, float alpha /* from 0 - 1*/);
#if SDL_MAJOR_VERSION < 2
    void setupAlphaBlit(SDL_Surface * surf, float alpha, SDL_Rect sdldest);
    void doAlphaBlit(SDL_Surface *, float alpha, SDL_Rect sdldest);
#endif
    void drawSpriteSrc(Image *image,
                            const Rect &src,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                           float angle, float alpha /* from 0 - 1 */);
    virtual void drawLine(int x0, int y0, int x1, int y1,
                          const SDL_Color& color, float alpha);
    virtual void resize(int w, int h);
    virtual void beginFrame();
    virtual void endFrame();
#if SDL_MAJOR_VERSION >= 2
    SDL_Texture* screen;
    void * temp_buffer;
#else
    SDL_Surface* screen;
    SDL_Surface* temp_buffer; // for alpha compositing
#endif
    int w, h;
    std::shared_ptr<SDLContext> context;

    FontManager mFontManager;
};

}

#endif
