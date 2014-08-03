#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "graphics/graphics.hpp"

namespace Polarity {

SDL_Surface *test_image;
AudioFile *audioTest;
AudioChannelPlayer *audioPlayer;

bool loaded = false;

void loadAssets() {
    test_image = loadImage("assets/helloworld.png");
//     audioTest = new AudioFile("assets/audio/bathed_in_the_light.mp3", 1);
//     if (audioTest->validateMusicLoaded()) {
//         std::cerr << "Couldn't load audio" << std::endl;
//     } else {
//         std::cerr << "Audio is loaded: " << audioTest->getState() << std::endl;
//     }

    audioPlayer = new AudioChannelPlayer(16);
    if (audioPlayer->addChannel("white", "assets/audio/frozen_star.mp3", 0) != AudioFileError::OK) {
        std::cerr << "Couldn't load white track" << std::endl;
    }
    if (audioPlayer->addChannel("black", "assets/audio/lightless_dawn.mp3", 1) != AudioFileError::OK) {
        std::cerr << "Couldn't load black track" << std::endl;
    }
}

bool loopIter(SDL_Surface *screen) {
    SDL_BlitSurface(test_image, NULL, screen, NULL );
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        world->keyEvent(event.key.keysym.sym, event.type == SDL_KEYDOWN);
        if (event.key.keysym.sym == SDLK_SPACE) {
            audioPlayer->playChannel("white");
            audioPlayer->playChannel("black");
        } else if (event.key.keysym.sym == SDLK_ESCAPE) {
            audioPlayer->stopChannel("white");
            audioPlayer->stopChannel("black");
        } else if (event.key.keysym.sym == SDLK_w) {
            audioPlayer->setChannelVolume("white", 1.0);
            audioPlayer->setChannelVolume("black", 0.0);
        } else if (event.key.keysym.sym == SDLK_b) {
            audioPlayer->setChannelVolume("white", 0.0);
            audioPlayer->setChannelVolume("black", 1.0);
        }
      }
    }
    world->tick();
    return true;
}
}
