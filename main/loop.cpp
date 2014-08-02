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
    audioTest = new AudioFile("assets/audio/bathed_in_the_light.mp3", 1);
    if (audioTest->validateMusicLoaded()) {
        std::cerr << "Couldn't load audio" << std::endl;
    } else {
        std::cerr << "Audio is loaded: " << audioTest->getState() << std::endl;
    }

    audioPlayer = new AudioChannelPlayer(3);
    if (audioPlayer->addChannel("forward", "assets/audio/builder.mp3", 1) != AudioFileError::OK) {
        std::cerr << "Couldn't load forward track" << std::endl;
    }
    if (audioPlayer->addChannel("reverse", "assets/audio/builder_rev.mp3", 2) != AudioFileError::OK) {
        std::cerr << "Couldn't load reverse track" << std::endl;
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
        } else if (event.key.keysym.sym == SDLK_1) {
            audioPlayer->stopChannel("forward");
            audioPlayer->stopChannel("reverse");
            audioPlayer->setChannelVolume("forward", 1.0);
            audioPlayer->setChannelVolume("reverse", 0.0);
            audioPlayer->playChannel("forward");
            audioPlayer->playChannel("reverse");
        } else if (event.key.keysym.sym == SDLK_9) {
            audioPlayer->stopChannel("forward");
            audioPlayer->stopChannel("reverse");
            audioPlayer->setChannelVolume("forward", 0.0);
            audioPlayer->setChannelVolume("reverse", 1.0);
            audioPlayer->playChannel("forward");
            audioPlayer->playChannel("reverse");
        } else if (event.key.keysym.sym == SDLK_5) {
            audioPlayer->stopChannel("forward");
            audioPlayer->stopChannel("reverse");
            audioPlayer->setChannelVolume("forward", 0.5);
            audioPlayer->setChannelVolume("reverse", 0.5);
            audioPlayer->playChannel("forward");
            audioPlayer->playChannel("reverse");
        } else if (event.key.keysym.sym == SDLK_0) {
            audioPlayer->stopChannel("forward");
            audioPlayer->stopChannel("reverse");
        }
    }
    world->tick();
    return true;
}
}
