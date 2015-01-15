#ifndef POLARITY_GRAPHICS_CANVAS_HPP__
#define POLARITY_GRAPHICS_CANVAS_HPP__
#include "util/shared.hpp"
#include "image.hpp"
struct SDL_Surface;
struct SDL_Color;
union SDL_Event;


namespace Polarity {

struct BlitDescription;
class FontManager;
class AsyncIOTask;
class Rect;

class POLARITYGFX_EXPORT Canvas {
    Canvas& operator= (const Canvas&) = delete;
    Canvas(const Canvas&) = delete;
protected:
    enum {
#ifdef EMSCRIPTEN
        FONT_CACHE_SIZE=4096,
#else
        FONT_CACHE_SIZE=16,
#endif
        TEXT_CACHE_SIZE=4096
    };
public:
    Canvas() {}

    virtual int width() = 0;
    virtual int height() = 0;

    virtual ~Canvas() {}

    virtual FontManager &fontManager() = 0;
    virtual const std::shared_ptr<AsyncIOTask> &asyncIOTask() const = 0;

    // Manages lifetime of the surface from here on.
    virtual Image *loadImageFromSurface(SDL_Surface *surf) = 0;
    virtual Image *loadImage(const std::string &filename) = 0;
    virtual void drawSprite(Image *image,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                            float angle, float alpha) = 0;
    virtual void drawLine(int x0, int y0, int x1, int y1, const SDL_Color& color, float alpha) = 0;
    virtual DisplayList *makeDisplayList(const std::shared_ptr<Image> &image,
                                         const std::vector<BlitDescription> &draws,
                                         const Rect& bounds) = 0;
    virtual void attachDisplayList(DisplayList *, const std::shared_ptr<Image> &image) = 0;
    virtual void drawDisplayList(const DisplayList *, int x, int y) = 0;
    virtual void resize(int w, int h) = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear() {}
    virtual bool getNextEvent(SDL_Event* out_event) = 0;
};

}

#endif
