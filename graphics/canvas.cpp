#include "canvas.hpp"
#include "color.hpp"
#ifdef USE_SDL2
#include "SDL2/SDL_version.h"
#include "SDL2/SDL.h"
#else
#include "SDL/SDL_version.h"
#include "SDL/SDL.h"
#endif
namespace Polarity {
/*
void Canvas::drawLine(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b, float alpha) {
    SDL_Color color = {};
    color.r = r;
    color.g = g;
    color.b = b;
    drawLine(x0, y0, x1, y1, color, alpha);
    }*/
void Canvas::drawLine(int x0, int y0, int x1, int y1, const Color &color) {
    SDL_Color sdl_color = {};
    sdl_color.r = color.r;
    sdl_color.g = color.g;
    sdl_color.b = color.b;
#ifdef USE_SDL2
    sdl_color.a = color.a;
#else
    sdl_color.unused = color.a;
#endif
    drawLine(x0, y0, x1, y1, sdl_color, color.a / 255.0f);
}
}
