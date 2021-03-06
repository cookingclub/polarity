#ifndef POLARITY_GRAPHICS_FONT_RENDERER_HPP__
#define POLARITY_GRAPHICS_FONT_RENDERER_HPP__

#include <string>
#include "util/shared.hpp"
#include "rect.hpp"
typedef struct _TTF_Font TTF_Font;
struct SDL_Color;

namespace Polarity {
struct Color;
class Image;
class Canvas;

class POLARITYGFX_EXPORT FontRenderer {
public:

    FontRenderer(const std::string &fontName, int ptSize);

    ~FontRenderer();

    bool isLoaded() {
        return (font != nullptr);
    }

    Image *render(Canvas *canvas, const Color &color,
                  const std::string &message);
    Rect textSize(const std::string &message);
    
private:

    TTF_Font *font;

};
}

#endif
