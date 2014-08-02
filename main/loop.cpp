#include "main/main.hpp"
#include "graphics/graphics.hpp"

namespace Polarity {

SDL_Surface *test_image;

bool loaded = false;

void loadAssets() {
    test_image = loadImage("assets/helloworld.png");
}

bool loopIter(SDL_Surface *screen) {
    if (!loaded) {
        loadAssets();
        loaded = true;
    }
    SDL_BlitSurface(test_image, NULL, screen, NULL );
    return true;
}
}
