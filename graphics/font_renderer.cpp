#include "graphics/font_renderer.hpp"
#include "graphics/canvas.hpp"

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

Image *FontRenderer::render(Canvas *canvas, const SDL_Color &color,
                            const std::string &message) {
    if (!isLoaded()) {
        return nullptr;
    }
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, message.c_str(), color);
    return canvas->loadImageFromSurface(surf);
}

}
