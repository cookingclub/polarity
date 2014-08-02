#include "audio/audio.hpp"
#include "main/main.hpp"
#include "graphics/graphics.hpp"

namespace Polarity {

SDL_Surface *test_image;
AudioFile *audioTest;

bool loaded = false;

void loadAssets() {
    test_image = loadImage("assets/helloworld.png");
    audioTest = new AudioFile("assets/bathed_in_the_light.mp3", 1);
    if (audioTest->validateMusicLoaded()) {
        std::cerr << "Couldn't load audio" << std::endl;
    } else {
        std::cerr << "Audio is loaded: " << audioTest->getState() << std::endl;
    }
}

bool loopIter(SDL_Surface *screen) {
    SDL_BlitSurface(test_image, NULL, screen, NULL );
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            std::cout << "Playing!" << std::endl;
            audioTest->startAudioPlayback();
        } else if (event.key.keysym.sym == SDLK_ESCAPE) {
            std::cout << "Stopping!" << std::endl;
            audioTest->stopAudioPlayback();            
        } else if (event.key.keysym.sym == SDLK_p) {
            std::cout << "Pausing!" << std::endl;
            audioTest->pauseAudioPlayback();            
        }
    }
    return true;
}
}
