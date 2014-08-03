#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "graphics/graphics.hpp"

namespace Polarity {

AudioFile *audioTest;

bool loaded = false;

void loadAssets() {
    audioTest = new AudioFile("assets/bathed_in_the_light.mp3", 1);
    if (audioTest->validateMusicLoaded()) {
        std::cerr << "Couldn't load audio" << std::endl;
    } else {
        std::cerr << "Audio is loaded: " << audioTest->getState() << std::endl;
    }
}

bool loopIter(SDL_Surface *screen) {
    SDL_Event event;
    std::vector<int> keyUps;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        if (event.type == SDL_KEYDOWN) {
	  world->keyEvent(event.key.keysym.sym, true);
	} else {
	  keyUps.push_back(event.key.keysym.sym);
	}
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
    }
    world->tick();
    // all key up have to happen after key downs so we get a full tick of downs
    for (auto &key : keyUps) {
      world->keyEvent(key, false);
    }
    return true;
}
}
