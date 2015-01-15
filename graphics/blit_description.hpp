#ifndef POLARITY_BLIT_DESCRIPTION_HPP_
#define POLARITY_BLIT_DESCRIPTION_HPP_
#include "util/shared.hpp"
#include "rect.hpp"

namespace Polarity {
    struct POLARITYGFX_EXPORT BlitDescription {
        Rect src;
        float centerX;
        float centerY;
        float scaleX;
        float scaleY;
        float angle;
    };
}
#endif
