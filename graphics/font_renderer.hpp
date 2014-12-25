#ifndef POLARITY_GRAPHICS_FONT_RENDERER_HPP__
#define POLARITY_GRAPHICS_FONT_RENDERER_HPP__

#include "graphics/canvas.hpp"

#include "util/lru_map.hpp"

#include <iostream>

#ifdef USE_SDL2
#include <SDL2/SDL_ttf.h>
#else
#include <SDL/SDL_ttf.h>
#endif

#ifdef EMSCRIPTEN
#define TTF_RenderUTF8_Blended TTF_RenderText_Solid
#endif

namespace Polarity {

class FontRenderer {
public:

    FontRenderer(const std::string &fontName, int ptSize) {
        font = TTF_OpenFont(fontName.c_str(), ptSize);
        if (!font) {
            std::cerr << "Failed to open font " << fontName << " size "
                      << ptSize << ": " << TTF_GetError() << std::endl;
        }
    }

    ~FontRenderer() {
        TTF_CloseFont(font);
    }

    bool isLoaded() {
        return (font != nullptr);
    }

    Image *render(Canvas *canvas, SDL_Color color, const std::string &message) {
        if (!isLoaded()) {
            return nullptr;
        }
        SDL_Surface *surf = TTF_RenderUTF8_Blended(font, message.c_str(), color);
        return canvas->loadImageFromSurface(surf);
    }

private:

    TTF_Font *font;

};
}

#endif
