#ifndef _WORLD_SOUNDS_HPP_
#define _WORLD_SOUNDS_HPP_
#include <memory>
namespace Polarity {
class AudioChannelPlayer;
std::shared_ptr<AudioChannelPlayer> loadAudioChannels();
}
#endif
