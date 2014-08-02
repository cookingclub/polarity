#ifndef POLARITY_GRAPHICS_GRAPHICS_HPP__
#define POLARITY_GRAPHICS_GRAPHICS_HPP__

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
    SDL_Surface *loadImage(const std::string &filename);
}

#endif
