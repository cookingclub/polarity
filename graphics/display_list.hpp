#ifndef _POLARITY_GRAPHICS_DISPLAY_LIST_HPP_
#define _POLARITY_GRAPHICS_DISPLAY_LIST_HPP_

#include "image.hpp"

#include <memory>

namespace Polarity {
    class Canvas;
    class POLARITYGFX_EXPORT DisplayList {
    public:
        virtual ~DisplayList(){}
        virtual void draw(Canvas * cavas, int x, int y) const = 0;
        virtual void attach(const std::shared_ptr<Image>&newImage) = 0;
    };
}

#endif

