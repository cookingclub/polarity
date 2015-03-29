#include "graphics/font_renderer.hpp"
#include "graphics/canvas.hpp"
#include "graphics/color.hpp"
#ifdef USE_SDL2
#include <SDL2/SDL_ttf.h>
#else
#include <SDL/SDL_ttf.h>
#endif

#ifdef EMSCRIPTEN
#define TTF_RenderUTF8_Blended TTF_RenderText_Solid
#endif

#include <iostream>

#ifdef EMSCRIPTEN
static std::string fontPath(const std::string &fontName) {
    return fontName;
}
#else
static const char *PREPEND_PATH = "assets/fonts/";
static const char *TTF_EXT = ".ttf";

static std::string fontPath(const std::string &fontName) {
    return PREPEND_PATH + fontName + TTF_EXT;
}
#endif
namespace Polarity {

FontRenderer::FontRenderer(const std::string &fontName, int ptSize) {
    std::string fontFilename = fontPath(fontName);
    font = TTF_OpenFont(fontFilename.c_str(), ptSize);
    if (!font) {
        std::cerr << "Failed to open font " << fontName << " size "
                  << ptSize << ": " << TTF_GetError() << std::endl;
    }
}

FontRenderer::~FontRenderer() {
    TTF_CloseFont(font);
}

Rect FontRenderer::textSize(const std::string&message) {
    if (isLoaded()) {
        int w = 0, h = 0;
        TTF_SizeUTF8(font, message.c_str(), &w, &h);
        return Rect(0, 0, w, h);
    }
    return Rect(0,0,0,0);
}

Image *FontRenderer::render(Canvas *canvas, const Color &color,
                            const std::string &message) {
    SDL_Color sdl_color;
    sdl_color.r = color.r;
    sdl_color.g = color.g;
    sdl_color.b = color.b;
#ifdef USE_SDL2
    sdl_color.a = color.a;
#else
    sdl_color.unused = color.a;
#endif

    if (!isLoaded()) {
        return nullptr;
    }
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, message.c_str(), sdl_color);
    return canvas->loadImageFromSurface(surf);
}

}
