#ifndef POLARITY_GRAPHICS_IMAGE_HPP__
#define POLARITY_GRAPHICS_IMAGE_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "rect.hpp"

namespace Polarity {

class Canvas;

class Image {
protected:
    friend class Animation;

    Image(const Image&x) = delete;
    Image& operator=(const Image&x) = delete;
public:
    Image() {}

    virtual ~Image() {}

    static std::shared_ptr<Image> get(Canvas *canvas, const std::string &filename);

    virtual void draw(Canvas *screen, int x, int y);
    virtual void drawSubimage(Canvas *screen, const Rect& src, int x, int y);
    virtual int width() = 0;
    virtual int height() = 0;
};

}

#endif
