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
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_audio.h>

using namespace std;

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
}

// good for playing ambient tracks on a loop
// recommend using one of the channel-mixed things for music though
class AudioFile {
public:

    AudioFile() :
        fPath(""),
        fMusic(nullptr),
        fNumLoops(1),
        fState(Polarity::CurrentAudioState::UNKNOWN) {
    }

    AudioFile(string path, int nLoop = 1) :
        fPath(path),
        fMusic(Mix_LoadMUS( path.c_str() )),
        fNumLoops(nLoop),
        fState(Polarity::CurrentAudioState::UNKNOWN) {
    }
    ~AudioFile() {
        Mix_FreeMusic( fMusic );
    }

    Polarity::AudioFileError validateMusicLoaded() {
        if (fMusic == nullptr) {
            fState = Polarity::CurrentAudioState::ERROR;
            return Polarity::AudioFileError::FILE_NOT_LOADED;
        }
        fState = Polarity::CurrentAudioState::LOADED;
        return Polarity::AudioFileError::OK;
    }

    Polarity::AudioFileError startAudioPlayback() {
        // are we already playing? if not, start
        if ( fState != Polarity::CurrentAudioState::PLAYING ) {
            int err = Mix_PlayMusic(this->fMusic, fNumLoops);
            if (err == -1) {
                cerr << "Can't play back audio file for some reason... are you use the player is loaded?\n";
                fState = Polarity::CurrentAudioState::ERROR;
                return Polarity::AudioFileError::CANT_PLAY;
            }
        } else if ( Mix_PausedMusic() == 1 || fState == Polarity::CurrentAudioState::PAUSED) {
            // if paused, resume it
            Mix_ResumeMusic();
        }
        fState = Polarity::CurrentAudioState::PLAYING;
        return Polarity::AudioFileError::OK;
    }

    void stopAudioPlayback() {
        if (fState == Polarity::CurrentAudioState::PLAYING || fState == Polarity::CurrentAudioState::PAUSED) {
            fState = Polarity::CurrentAudioState::STOPPED;
            Mix_HaltMusic();
        }
    }

    void pauseAudioPlayback() {
        if (fState == Polarity::CurrentAudioState::PLAYING) {
            Mix_PauseMusic();
            fState = Polarity::CurrentAudioState::PAUSED;
        }
    }

    void resumeAudioPlayback() {
        if (fState == Polarity::CurrentAudioState::PAUSED) {
            Mix_ResumeMusic();
            fState = Polarity::CurrentAudioState::PLAYING;
        }
    }

    Polarity::CurrentAudioState getState() const {
        return fState;
    }

    Mix_Music* getMusic() const {
        return fMusic;
    }

    string getPath() const {
        return fPath;
    }
private:
    string fPath;
    Mix_Music *fMusic;
    int fNumLoops;  // -1 indicates loop forever, 0 = don't play? why would i not want to play? it's so bad that I now have cancer
    Polarity::CurrentAudioState fState;
};

class AudioChannelPlayer {
public:

    AudioChannelPlayer(int channels) :
        fNumChans(channels),
        fAllocatedChans(0) {
        fNumAvailableChans = Mix_AllocateChannels(channels);
    }

    Polarity::AudioFileError addChannel(string id, string filePath, int num) {
        if (fAllocatedChans >= fNumChans) {
            cerr << "All channels allocated, can't allocate another one" << endl;
            return Polarity::AudioFileError::NO_MORE_CHANNELS;
        }
        Mix_Chunk *chunk = Mix_LoadWAV( filePath.c_str() );
        if (chunk == nullptr) {
            cerr << "Couldn't load " << filePath << ", got error: " << Mix_GetError() << endl;
            return Polarity::AudioFileError::FILE_NOT_LOADED;
        } 

        fChunks[id] = chunk;
        fChannelNames[id] = num;
        fChannelPaths[id] = filePath;
        fChannelStates[id] = Polarity::CurrentAudioState::LOADED;
        fChannelVolumes[id] = 0;
        setChannelVolume(id, 0);
        fAllocatedChans++;
        return Polarity::AudioFileError::OK;
    }


    bool channelExists(string id) {
        return  fChunks.find(id) != fChunks.end() &&
                fChannelNames.find(id) != fChannelNames.end() &&
                fChannelPaths.find(id) != fChannelPaths.end();
    }

    // scale is 0 to 1 and maps to 0 to 128
    Polarity::AudioFileError setChannelVolume(string id, double scale) {
        if (channelExists(id)) {
            fChannelVolumes[id] = Mix_Volume(fChannelNames[id], scale * MIX_MAX_VOLUME);
            return Polarity::AudioFileError::OK;
        } else {
            return Polarity::AudioFileError::NO_SUCH_CHANNEL;
        }
    }

    Polarity::AudioFileError playChannel(string id, int loops = 1) {
        if (!channelExists(id)) {
            cerr << "Channel " << id << " doesn't exist" << endl;
            return Polarity::AudioFileError::NO_SUCH_CHANNEL;
        }
        if (fChannelStates[id] != Polarity::CurrentAudioState::PLAYING) {
            if (Mix_Playing(fChannelNames[id]) == 0) {
                if( Mix_PlayChannel(fChannelNames[id], fChunks[id], loops) == -1) {
                    cerr << "Mix_PlayChannel failed: " << Mix_GetError() << endl;
                    return Polarity::AudioFileError::CANT_PLAY;
                }
            } else if (Mix_Paused(fChannelNames[id]) || fChannelStates[id] == Polarity::CurrentAudioState::PAUSED) {
                Mix_Resume(fChannelNames[id]);
            }
        }
        fChannelStates[id] = Polarity::CurrentAudioState::PLAYING;
        return Polarity::AudioFileError::OK;
    }

    Polarity::AudioFileError stopChannel(string id) {
        if (!channelExists(id)) {
            cerr << "Channel " << id << " doesn't exist" << endl;
            return Polarity::AudioFileError::NO_SUCH_CHANNEL;
        }

        Mix_HaltChannel(fChannelNames[id]);
        fChannelStates[id] = Polarity::CurrentAudioState::STOPPED;
        return Polarity::AudioFileError::OK;
    }
private:

    int fNumChans;

    map<string, Mix_Chunk*> fChunks;
    map<string, int> fChannelNames;
    map<string, string> fChannelPaths;
    map<string, Polarity::CurrentAudioState> fChannelStates;
    map<string, int> fChannelVolumes;
    int fAllocatedChans;
    int fNumAvailableChans;
};
