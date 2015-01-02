#include <audio/audio.hpp>
#include "sounds.hpp"
namespace Polarity {
using namespace std;
const int NUM_AUDIO_CHANNELS = 64;
typedef std::tuple<std::string, std::string, double> ChannelSpec;
static ChannelSpec specs[] = {  ChannelSpec("white-music", "assets/audio/frozen_star.mp3", 0.0),
                                ChannelSpec("black-music", "assets/audio/lightless_dawn.mp3", 0.0),
                                ChannelSpec("buzz", "assets/audio/buzz.mp3", 1.0),
                                ChannelSpec("woowoo", "assets/audio/woowoo.mp3", 1.0),
                                ChannelSpec("step-stone", "assets/audio/step_stone.wav", 0.15),
                                ChannelSpec("land-soft", "assets/audio/land_stone_weak.wav", 0.85),
                                ChannelSpec("jump", "assets/audio/jump.wav", 0.25)
                            };

AudioFileError createAudioChannel(std::shared_ptr<AudioChannelPlayer> audioPlayer, string id, string filepath, int num, double initVolume = 0.0) {
    AudioFileError err = audioPlayer->addChannel(audioPlayer, id, filepath, num);
    if (err != AudioFileError::OK) {
        cerr << "Couldn't load track for '" << id << "'\n";
    } else {
        audioPlayer->setChannelVolume(id, initVolume);
    }
    return err;
}

std::shared_ptr<AudioChannelPlayer> loadAudioChannels() {
    std::shared_ptr<AudioChannelPlayer> audioPlayer(std::make_shared<AudioChannelPlayer>(Polarity::NUM_AUDIO_CHANNELS));
    for (int i = 0; i < 7 && i < Polarity::NUM_AUDIO_CHANNELS; ++i) {
        double vol;
        std::string id;
        std::string path;
        tie(id, path, vol) = Polarity::specs[i];
        Polarity::createAudioChannel(audioPlayer, id, path, i, vol);
    }
    return audioPlayer;
}
}
