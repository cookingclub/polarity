#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "graphics/graphics.hpp"

namespace Polarity {

AudioFile *audioTest;
AudioChannelPlayer *audioPlayer;
bool buzzing = false;       // TODO: bring this into a game state! you should buzz if you have the appropriate powerup

bool loaded = false;

void loadAssets() {
    audioPlayer = new AudioChannelPlayer(32);
    if (audioPlayer->addChannel("white", "assets/audio/frozen_star.mp3", 0) != AudioFileError::OK) {
        std::cerr << "Couldn't load white track" << std::endl;
    }
    if (audioPlayer->addChannel("black", "assets/audio/lightless_dawn.mp3", 1) != AudioFileError::OK) {
        std::cerr << "Couldn't load black track" << std::endl;
    }
    if (audioPlayer->addChannel("buzz", "assets/audio/buzz.mp3", 2) != AudioFileError::OK) {
        std::cerr << "Couldn't load buzz track" << std::endl;
    } else {
        audioPlayer->setChannelVolume("buzz", 1.0);
    }
}

bool loopIter(SDL_Surface *screen) {
    SDL_Event event;
    std::vector<int> keyUps;
    static std::shared_ptr<Animation> anim = Animation::get("/assets/graphics/walking-15.png");
    anim->draw(screen, 500, 0);
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            if (event.type == SDL_KEYDOWN) {
                world->keyEvent(event.key.keysym.sym, true);
                if (event.key.keysym.sym == SDLK_SPACE) {
                    audioPlayer->playChannel("white");
                    audioPlayer->playChannel("black");
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    audioPlayer->stopChannel("white");
                    audioPlayer->stopChannel("black");
                } else if (event.key.keysym.sym == SDLK_w) {
                    // switch to white track
                    audioPlayer->setChannelVolume("white", 1.0);
                    audioPlayer->setChannelVolume("black", 0.0);
                } else if (event.key.keysym.sym == SDLK_b) {
                    // switch to black track
                    audioPlayer->setChannelVolume("white", 0.0);
                    audioPlayer->setChannelVolume("black", 1.0);
                } else if (event.key.keysym.sym == SDLK_1) {
                    if (!buzzing) {
                        audioPlayer->playChannel("buzz", -1);
                    } else {
                        audioPlayer->stopChannel("buzz");
                    }
                    buzzing = !buzzing;
                } else if (event.key.keysym.sym == SDLK_2) {
                    
                }            
            } else {
                keyUps.push_back(event.key.keysym.sym);
            }
        }
    }
    world->tick();
    world->draw(screen);
    // all key up have to happen after key downs so we get a full tick of downs
    for (auto &key : keyUps) {
      world->keyEvent(key, false);
    }
    return true;
}
}
