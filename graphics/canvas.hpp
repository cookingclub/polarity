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

    virtual void drawImage(Image *image, const Rect &src, const Rect &dst) = 0;
    virtual void swapBuffers() = 0;
};

}

#endif
