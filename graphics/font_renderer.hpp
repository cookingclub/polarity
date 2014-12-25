#ifndef POLARITY_GRAPHICS_FONT_RENDERER_HPP__
#define POLARITY_GRAPHICS_FONT_RENDERER_HPP__

#include "graphics/canvas.hpp"

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

    FontRenderer(const std::string &name, int ptSize) {
        font = TTF_OpenFont(name.c_str(), ptSize);
        if (!font) {
            std::cerr << "Failed to open font " << name << " size "
                      << ptSize << ": " << TTF_GetError() << std::endl;
        }
    }

    ~FontRenderer() {
        TTF_CloseFont(font);
    }

    bool isLoaded() {
        return (font != nullptr);
    }

    void draw(Canvas *canvas, Rect rect, SDL_Color color,
              const std::string &message) {
        if (!isLoaded()) {
            return;
        }
        SDL_Surface *surf = TTF_RenderUTF8_Blended(font, message.c_str(), color);
        std::shared_ptr<Image> image (canvas->loadImageFromSurface(surf));
        canvas->drawSprite(image.get(),
                           rect.left() + image->width() / 2,
                           rect.top() - image->height() / 2,
                           image->width(), image->height(), 0);
    }

private:

    TTF_Font *font;

    // std::unordered_map<std::string, std::shared_ptr<Image> > textCache;

};
}

#endif
