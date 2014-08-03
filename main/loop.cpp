#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "graphics/graphics.hpp"

namespace Polarity {

AudioFile *audioTest;
extern std::shared_ptr<AudioChannelPlayer> audioPlayer;
// bool buzzing = false;       // TODO: bring this into a game state! you should buzz if you have the appropriate powerup
// bool woowooing = false;

bool loaded = false;

const int NUM_AUDIO_CHANNELS = 64;
typedef std::tuple<string, string, double> ChannelSpec;
static ChannelSpec specs[] = {  ChannelSpec("white-music", "assets/audio/frozen_star.mp3", 0.0),
                                ChannelSpec("black-music", "assets/audio/lightless_dawn.mp3", 0.0),
                                ChannelSpec("buzz", "assets/audio/buzz.mp3", 1.0),
                                ChannelSpec("woowoo", "assets/audio/woowoo.mp3", 1.0),
                                ChannelSpec("step-stone", "assets/audio/step_stone.wav", 0.15),
                                ChannelSpec("land-soft", "assets/audio/land_stone_weak.wav", 0.85),
                                ChannelSpec("grunt", "assets/audio/jump_grunt.wav", 0.25)
                            };

AudioFileError createAudioChannel(std::shared_ptr<AudioChannelPlayer> audioPlayer, string id, string filepath, int num, double initVolume = 0.0) {
    AudioFileError err = audioPlayer->addChannel(id, filepath, num);
    if (err != AudioFileError::OK) {
        cerr << "Couldn't load track for '" << id << "'\n";
    } else {
        audioPlayer->setChannelVolume(id, initVolume);
    }
    return err;
}

void loadAudioChannels() {
    audioPlayer = make_shared<AudioChannelPlayer>(Polarity::NUM_AUDIO_CHANNELS);
    for (int i = 0; i < 7 && i < Polarity::NUM_AUDIO_CHANNELS; ++i) {
        double vol;
        string id;
        string path;
        tie(id, path, vol) = Polarity::specs[i];
        Polarity::createAudioChannel(audioPlayer, id, path, i, vol);
    }
}


void loadAssets() {
    loadAudioChannels();
}

bool loopIter(SDL_Surface *screen) {
    SDL_Event event;
    std::vector<int> keyUps;
    SDL_FillRect(screen, NULL, 0xffffffff);
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            if (event.type == SDL_KEYDOWN) {
                world->keyEvent(event.key.keysym.sym, true);
//                 if (event.key.keysym.sym == SDLK_SPACE) {
//                     audioPlayer->playChannel("white-music");
//                     audioPlayer->playChannel("black-music");
//                 } else if (event.key.keysym.sym == SDLK_ESCAPE) {
//                     audioPlayer->stopChannel("white-music");
//                     audioPlayer->stopChannel("black-music");
//                 } else if (event.key.keysym.sym == SDLK_v) {
//                     // switch to white track
//                     audioPlayer->setChannelVolume("white-music", 1.0);
//                     audioPlayer->setChannelVolume("black-music", 0.0);
//                 } else if (event.key.keysym.sym == SDLK_b) {
//                     // switch to black track
//                     audioPlayer->setChannelVolume("white-music", 0.0);
//                     audioPlayer->setChannelVolume("black-music", 1.0);
//                 } else if (event.key.keysym.sym == SDLK_1) {
//                     if (!buzzing) {
//                         audioPlayer->playChannel("buzz", -1);
//                     } else {
//                         audioPlayer->stopChannel("buzz");
//                     }
//                     buzzing = !buzzing;
//                 } else if (event.key.keysym.sym == SDLK_2) {
//                     if (!woowooing) {
//                         audioPlayer->playChannel("woowoo", -1);
//                     } else {
//                         audioPlayer->stopChannel("woowoo");
//                     }
//                     woowooing = !woowooing;
//                 } else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_a) {
//                     audioPlayer->playChannel("step-stone", -1);
//                 } else if (event.key.keysym.sym == SDLK_s) {
//                     audioPlayer->playChannel("land-soft", 1);
//                 } else if (event.key.keysym.sym == SDLK_w) {
//                     audioPlayer->playChannel("grunt", 1);
//                 }
            } else {
                keyUps.push_back(event.key.keysym.sym);
//                 audioPlayer->stopChannel("step-stone");
//                 audioPlayer->stopChannel("land-soft");
//                 audioPlayer->stopChannel("grunt");
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
