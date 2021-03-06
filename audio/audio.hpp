/**
 *  audio.hpp
 *  
 *  Author: Ilya Veygman <iveygman@gmail.com>
 *
 *  Contains some audio utilties
 */
#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <tuple>
#include <vector>
#include <memory>
#ifdef USE_SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_audio.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_audio.h>
#endif
#ifdef EMSCRIPTEN
#include <sys/stat.h>
#include <emscripten.h>
#endif

namespace Polarity {
enum AudioFileError {
    OK = 0,
    FILE_NOT_LOADED = 1,
    CANT_PLAY = 2,
    OTHER_ERROR = -1,
    NO_MORE_CHANNELS = 3,
    NO_SUCH_CHANNEL = 4
};

enum CurrentAudioState {
    ERROR = -1,
    UNKNOWN = -2,
    STOPPED = 0,
    PAUSED = 1,
    PLAYING = 2,
    LOADED = 3
};

class AudioChannelPlayer {
    static constexpr int AUDIO_RATE = 44100; // FIXME: Does this matter?
public:

    AudioChannelPlayer(int channels) :
            fNumChans(channels),
            fAllocatedChans(0) {
        if (Mix_OpenAudio(AUDIO_RATE, AUDIO_S16, 2, 4096)) {
            std::cerr << "Failed to init Audio" << std::endl;
        }
        fNumAvailableChans = Mix_AllocateChannels(channels);
    }
    ~AudioChannelPlayer() {
        Mix_AllocateChannels(0);
        for (std::map<std::string, Mix_Chunk*>::iterator i = fChunks.begin(), ie = fChunks.end(); i != ie; ++i) {
            Mix_FreeChunk(i->second);
            i->second = nullptr;
        }
        Mix_CloseAudio();
    }
    static Polarity::AudioFileError addChannel(std::shared_ptr<AudioChannelPlayer> thus, std::string id, std::string filePath, int num) {
        if (thus->fAllocatedChans >= thus->fNumChans) {
            std::cerr << "All channels allocated, can't allocate another one" << std::endl;
            return Polarity::AudioFileError::NO_MORE_CHANNELS;
        }

        thus->fChunks[id] = nullptr;
        thus->fChannelNames[id] = num;
        thus->fChannelPaths[id] = filePath;
        thus->fChannelStates[id] = Polarity::CurrentAudioState::LOADED;
        thus->fChannelLoops[id] = 1;
        thus->fChannelVolumes[id] = 0;
        thus->fAllocatedChans++;
        queueLoad(thus, id, filePath);
        return Polarity::AudioFileError::OK;
    }
#ifdef EMSCRIPTEN
    struct UserData{
        std::weak_ptr<AudioChannelPlayer> aplayer;
        std::string id;
        std::string filePath;
    };

    static void oncomplete(unsigned handle, void*ud, const char* fileName) {
        std::cerr << "audone: " << reinterpret_cast<UserData*>(ud)->filePath << " "<<fileName<< std::endl;
        std::unique_ptr<UserData> userData(reinterpret_cast<UserData*>(ud));
        std::cerr << "loaded audio: " << userData->filePath << std::endl;
        queueLoadHelper(userData->aplayer, userData->id, userData->filePath);
    }
    static void onprogress(unsigned handle, void*userData, int code) {
        std::cerr << "progress audio: " << reinterpret_cast<UserData*>(userData)->filePath << " " << code << std::endl;
    }
    static void onerror(unsigned, void*ud, int code) {
        std::unique_ptr<UserData> userData(reinterpret_cast<UserData*>(ud));

        std::cerr << "x failed to load " << reinterpret_cast<UserData*>(ud)->filePath << std::endl;
    }

    // Compatibility APIs.
    static void oncomplete(void*ud, const char* fileName) {
        oncomplete(0, ud, fileName);
    }

    static void onprogress(void*userData, int code) {
        onprogress(0, userData, code);
    }

    static void onerror(void*ud, int code) {
        onerror(0, ud, code);
    }

    static void queueLoad(const std::weak_ptr<AudioChannelPlayer>&aplayer,
                          std::string id, std::string filePath) {
        std::cerr<< "preparing to load "<<filePath<<std::endl;
        std::string fullPath = filePath;
        std::string::size_type where_slash = fullPath.find_first_of("\\/");
        while (where_slash != std::string::npos) {
            std::string subdir = fullPath.substr(0,where_slash);
            if (!subdir.empty()) {
                mkdir(subdir.c_str(), 0777);
            }
            where_slash = fullPath.find_first_of("\\/", where_slash + 1);
        }
        fullPath = "/" + filePath;
        emscripten_async_wget2(filePath.c_str(), fullPath.c_str(), "GET", "",
                               new UserData{aplayer, id, filePath},
                               &oncomplete, &onerror, &onprogress);
    }
#else
    static void queueLoad(const std::weak_ptr<AudioChannelPlayer>&aplayer,
                          std::string id, std::string filePath) {
        queueLoadHelper(aplayer, id, filePath);
    }
#endif
    static void queueLoadHelper(const std::weak_ptr<AudioChannelPlayer>&aplayer, std::string id, std::string filePath) {
        Mix_Chunk *chunk = Mix_LoadWAV( filePath.c_str() );
        if (chunk == nullptr) {
            std::cerr << "Couldn't load " << filePath << ", got error: " << Mix_GetError() << std::endl;
        } else {
            std::shared_ptr<AudioChannelPlayer> thus(aplayer.lock());
            if (thus) {
                if (thus->channelExists(id)) {
                    thus->fChunks[id] = chunk;
                    thus->setChannelVolume(id, thus->fChannelVolumes[id]);
                    if (thus->fChannelStates[id] == Polarity::CurrentAudioState::PLAYING) {
                        thus->playChannel(id, thus->fChannelLoops[id], true);
                    } else {
                        thus->stopChannel(id);
                    }
                }
            }
        }
    }

    bool channelLoaded(std::string id) {
        auto where= fChunks.find(id);
        return where != fChunks.end() && where->second != nullptr;
    }

    bool channelExists(std::string id) {
        return fChunks.find(id) != fChunks.end() &&
                fChannelNames.find(id) != fChannelNames.end() &&
                fChannelPaths.find(id) != fChannelPaths.end();
    }

    // scale is 0 to 1 and maps to 0 to 128
    Polarity::AudioFileError setChannelVolume(std::string id, double scale) {
        if (channelExists(id)) {
            fChannelVolumes[id] = scale;
            if (channelLoaded(id)) {
                Mix_Volume(fChannelNames[id], scale * MIX_MAX_VOLUME);
            }
            return Polarity::AudioFileError::OK;
        } else {
            return Polarity::AudioFileError::NO_SUCH_CHANNEL;
        }
    }

    Polarity::AudioFileError playChannel(std::string id, int loops = 1, bool force=false) {
        if (!channelExists(id)) {
            std::cerr << "Channel " << id << " doesn't exist" << std::endl;
            return Polarity::AudioFileError::NO_SUCH_CHANNEL;
        }
        if (channelLoaded(id) && (force ||
                                  fChannelStates[id] != Polarity::CurrentAudioState::PLAYING)) {
            if (Mix_Playing(fChannelNames[id]) == 0) {
                if( Mix_PlayChannel(fChannelNames[id], fChunks[id], loops) == -1) {
                    std::cerr << "Mix_PlayChannel failed: " << Mix_GetError() << std::endl;
                    return Polarity::AudioFileError::CANT_PLAY;
                }
            } else if (Mix_Paused(fChannelNames[id]) || fChannelStates[id] == Polarity::CurrentAudioState::PAUSED) {
                Mix_Resume(fChannelNames[id]);
            }
        }
        fChannelStates[id] = Polarity::CurrentAudioState::PLAYING;
        fChannelLoops[id] = loops;
        return Polarity::AudioFileError::OK;
    }

    Polarity::AudioFileError stopChannel(std::string id) {
        if (!channelExists(id)) {
            std::cerr << "Channel " << id << " doesn't exist" << std::endl;
            return Polarity::AudioFileError::NO_SUCH_CHANNEL;
        }
        if (channelLoaded(id)) {
            Mix_HaltChannel(fChannelNames[id]);
        }
        fChannelLoops[id] = 1;
        fChannelStates[id] = Polarity::CurrentAudioState::STOPPED;
        return Polarity::AudioFileError::OK;
    }
private:

    int fNumChans;

    std::map<std::string, Mix_Chunk*> fChunks;
    std::map<std::string, int> fChannelNames;
    std::map<std::string, std::string> fChannelPaths;
    std::map<std::string, Polarity::CurrentAudioState> fChannelStates;
    std::map<std::string, int> fChannelLoops;
    std::map<std::string, double> fChannelVolumes;
    int fAllocatedChans;
    int fNumAvailableChans;
};
}



