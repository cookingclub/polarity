/**
 *  audio.hpp
 *  
 *  Author: Ilya Veygman <iveygman@gmail.com>
 *
 *  Contains some audio utilties
 */

#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_audio.h>

using namespace std;

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

// good for playing ambient tracks on a loop
// recommend using one of the channel-mixed things for music though
class AudioFile {
public:

    AudioFile() :
        fPath(""),
        fMusic(nullptr),
        fNumLoops(1),
        fState(UNKNOWN) {
    }

    AudioFile(string path, int nLoop = 1) :
        fPath(path),
        fMusic(Mix_LoadMUS( path.c_str() )),
        fNumLoops(nLoop),
        fState(CurrentAudioState::UNKNOWN) {
    }
    ~AudioFile() {
        Mix_FreeMusic( fMusic );
    }

    AudioFileError validateMusicLoaded() {
        if (fMusic == nullptr) {
            fState = CurrentAudioState::ERROR;
            return AudioFileError::FILE_NOT_LOADED;
        }
        fState = LOADED;
        return AudioFileError::OK;
    }

    AudioFileError startAudioPlayback() {
        // are we already playing? if not, start
        if ( fState != CurrentAudioState::PLAYING ) {
            int err = Mix_PlayMusic(this->fMusic, fNumLoops);
            if (err == -1) {
                cerr << "Can't play back audio file for some reason... are you use the player is loaded?\n";
                fState = ERROR;
                return AudioFileError::CANT_PLAY;
            }
        } else if ( Mix_PausedMusic() == 1 || fState == CurrentAudioState::PAUSED) {
            // if paused, resume it
            Mix_ResumeMusic();
        }
        fState = CurrentAudioState::PLAYING;
        return AudioFileError::OK;
    }

    void stopAudioPlayback() {
        if (fState == CurrentAudioState::PLAYING || fState == CurrentAudioState::PAUSED) {
            fState = CurrentAudioState::STOPPED;
            Mix_HaltMusic();
        }
    }

    void pauseAudioPlayback() {
        if (fState == CurrentAudioState::PLAYING) {
            Mix_PauseMusic();
            fState = CurrentAudioState::PAUSED;
        }
    }

    void resumeAudioPlayback() {
        if (fState == CurrentAudioState::PAUSED) {
            Mix_ResumeMusic();
            fState = CurrentAudioState::PLAYING;
        }
    }

    CurrentAudioState getState() const {
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
    CurrentAudioState fState;
};

class AudioChannelPlayer {
public:

    AudioChannelPlayer(int channels) :
        fNumChans(channels),
        fAllocatedChans(0) {
        fNumAvailableChans = Mix_AllocateChannels(channels);
    }

    AudioFileError addChannel(string id, string filePath, int num) {
        if (fAllocatedChans >= fNumChans) {
            cerr << "All channels allocated, can't allocate another one" << endl;
            return AudioFileError::NO_MORE_CHANNELS;
        }
        Mix_Chunk *chunk = Mix_LoadWAV( filePath.c_str() );
        if (chunk == nullptr) {
            cerr << "Couldn't load " << filePath << ", got error: " << Mix_GetError() << endl;
            return AudioFileError::FILE_NOT_LOADED;
        } 

        fChunks[id] = chunk;
        fChannelNames[id] = num;
        fChannelPaths[id] = filePath;
        fChannelStates[id] = CurrentAudioState::LOADED;
        fChannelVolumes[id] = 0;
        setChannelVolume(id, 0);
        fAllocatedChans++;
        return AudioFileError::OK;
    }


    bool channelExists(string id) {
        return  fChunks.find(id) != fChunks.end() &&
                fChannelNames.find(id) != fChannelNames.end() &&
                fChannelPaths.find(id) != fChannelPaths.end();
    }

    // scale is 0 to 1 and maps to 0 to 128
    AudioFileError setChannelVolume(string id, double scale) {
        if (channelExists(id)) {
            fChannelVolumes[id] = Mix_Volume(fChannelNames[id], scale * MIX_MAX_VOLUME);
            return AudioFileError::OK;
        } else {
            return AudioFileError::NO_SUCH_CHANNEL;
        }
    }

    AudioFileError playChannel(string id) {
        if (!channelExists(id)) {
            cerr << "Channel " << id << " doesn't exist" << endl;
            return AudioFileError::NO_SUCH_CHANNEL;
        }
        if (fChannelStates[id] != CurrentAudioState::PLAYING) {
            if (Mix_Playing(fChannelNames[id]) == 0) {
                if( Mix_PlayChannel(fChannelNames[id], fChunks[id], 1) == -1) {
                    cerr << "Mix_PlayChannel failed: " << Mix_GetError() << endl;
                    return AudioFileError::CANT_PLAY;
                }
            } else if (Mix_Paused(fChannelNames[id]) || fChannelStates[id] == CurrentAudioState::PAUSED) {
                Mix_Resume(fChannelNames[id]);
            }
        }
        fChannelStates[id] = CurrentAudioState::PLAYING;
        return AudioFileError::OK;
    }

    AudioFileError stopChannel(string id) {
        if (!channelExists(id)) {
            cerr << "Channel " << id << " doesn't exist" << endl;
            return AudioFileError::NO_SUCH_CHANNEL;
        }

        Mix_HaltChannel(fChannelNames[id]);
        fChannelStates[id] = CurrentAudioState::STOPPED;
        return AudioFileError::OK;
    }

    AudioFileError fadeOutChannel(string id, int durationMilliseconds) {
        if (!channelExists(id)) {
            cerr << "Channel " << id << " doesn't exist" << endl;
            return AudioFileError::NO_SUCH_CHANNEL;
        }

        Mix_FadeOutChannel(fChannelNames[id], durationMilliseconds);
        fChannelStates[id] = CurrentAudioState::STOPPED;
        return AudioFileError::OK;
    }

    AudioFileError fadeInChannel(string id, int durationMilliseconds) {
        if (!channelExists(id)) {
            cerr << "Channel " << id << " doesn't exist" << endl;
            return AudioFileError::NO_SUCH_CHANNEL;
        }
        if (fChannelStates[id] != CurrentAudioState::PLAYING) {
            if( Mix_FadeInChannel(fChannelNames[id], fChunks[id], 1, durationMilliseconds) == -1) {
                cerr << "Mix_PlayChannel failed: " << Mix_GetError() << endl;
                return AudioFileError::CANT_PLAY;
            }
        }
        fChannelStates[id] = CurrentAudioState::PLAYING;
        return AudioFileError::OK;
    }
private:

    int fNumChans;

    map<string, Mix_Chunk*> fChunks;
    map<string, int> fChannelNames;
    map<string, string> fChannelPaths;
    map<string, CurrentAudioState> fChannelStates;
    map<string, int> fChannelVolumes;
    int fAllocatedChans;
    int fNumAvailableChans;
};