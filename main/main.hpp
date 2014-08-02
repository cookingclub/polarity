#ifndef POLARITY_MAIN_MAIN_HPP__
#define POLARITY_MAIN_MAIN_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>

#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"

namespace Polarity {
    bool loopIter(SDL_Surface *screen);
}

#endif
