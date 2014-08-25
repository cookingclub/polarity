#ifndef POLARITY_GRAPHICS_CANVAS_HPP__
#define POLARITY_GRAPHICS_CANVAS_HPP__

#include "image.hpp"

namespace Polarity {
class Canvas {
    Canvas& operator= (const Canvas&) = delete;
    Canvas(const Canvas&) = delete;
public:
    Canvas() {}

    virtual int width() = 0;
    virtual int height() = 0;

    virtual ~Canvas() {}

    virtual Image *loadImage(const std::string &filename) = 0;
    virtual void drawSprite(Image *image,
                            const Rect & src,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                            float angle) = 0;
    virtual DisplayList *makeDisplayList(const std::shared_ptr<Image> &image,
                                         const std::vector<Image::BlitDescription> &draws,
                                         const Rect& bounds) = 0;
    virtual void attachDisplayList(DisplayList *, const std::shared_ptr<Image> &image) = 0;
    virtual void drawDisplayList(const DisplayList *, int x, int y) = 0;
    virtual void swapBuffers() = 0;
};

}

#endif
