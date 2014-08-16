#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"

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
                                ChannelSpec("jump", "assets/audio/jump.wav", 0.25)
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

bool loopIter(Canvas *screen) {
    SDL_Event event;
    std::vector<int> keyUps;
    // SDL_FillRect(screen, NULL, 0xffffffff);
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            vector<bool> lastKeyState = world->getKeyState();
            if (event.type == SDL_KEYDOWN) {
                world->keyEvent(event.key.keysym.sym, true);
            } else {
                keyUps.push_back(event.key.keysym.sym);
            }
            world->findKeysJustPressed(lastKeyState);
        }
    }
    world->tick();
    completeAllPendingCallbacksFromMainThread();
    world->draw(screen);
    // all key up have to happen after key downs so we get a full tick of downs
    for (auto &key : keyUps) {
      world->keyEvent(key, false);
    }
    return true;
}
}
