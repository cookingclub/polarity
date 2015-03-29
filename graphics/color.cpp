#include "color.hpp"
#ifdef USE_SDL2
#include "SDL2/SDL_version.h"
#include "SDL2/SDL.h"
#else
#include "SDL/SDL_version.h"
#include "SDL/SDL.h"
#endif

namespace Polarity {
Color::Color(const SDL_Color& c){
    r = c.r;
    g = c.g;
    b = c.b;
#ifdef USE_SDL2
    a = c.a;
#else
    a = c.unused;
#endif
}
}
