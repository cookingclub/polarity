#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "graphics/graphics.hpp"

namespace Polarity {

AudioFile *audioTest;
AudioChannelPlayer *audioPlayer;
bool buzzing = false;       // TODO: bring this into a game state! you should buzz if you have the appropriate powerup
bool woowooing = false;

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
    if (audioPlayer->addChannel("woowoo", "assets/audio/woowoo.mp3", 3) != AudioFileError::OK) {
        std::cerr << "Couldn't load woowoo track" << std::endl;
    } else {
        audioPlayer->setChannelVolume("woowoo", 1.0);
    }
    if (audioPlayer->addChannel("step-stone", "assets/audio/step_stone.wav", 4) != AudioFileError::OK) {
        std::cerr << "Couldn't load step_stone audio" << std::endl;
    } else {
        audioPlayer->setChannelVolume("step-stone", 0.15);
    }
    if (audioPlayer->addChannel("land-stone", "assets/audio/land_stone_weak.wav", 5) != AudioFileError::OK) {
        std::cerr << "Couldn't load land_stone audio" << std::endl;
    } else {
        audioPlayer->setChannelVolume("land-stone", 0.15);
    }
}

bool loopIter(SDL_Surface *screen) {
    SDL_Event event;
    std::vector<int> keyUps;
    SDL_FillRect(screen, NULL, 0xffffffff);
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
                } else if (event.key.keysym.sym == SDLK_v) {
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
                    if (!woowooing) {
                        audioPlayer->playChannel("woowoo", -1);
                    } else {
                        audioPlayer->stopChannel("woowoo");
                    }
                    woowooing = !woowooing;
                } else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_a) {
                    audioPlayer->playChannel("step-stone", -1);
                } else if (event.key.keysym.sym == SDLK_s) {
                    audioPlayer->playChannel("land-stone", 1);
                }
            } else {
                keyUps.push_back(event.key.keysym.sym);
                audioPlayer->stopChannel("step-stone");
                audioPlayer->stopChannel("land-stone");
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
