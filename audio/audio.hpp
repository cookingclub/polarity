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
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

using std::string;
using std::cerr;
using std::cout;

class AudioFile {
public:
    enum AudioFileError {
        OK = 0,
        FILE_NOT_LOADED = 1,
        CANT_PLAY = 2,
        OTHER_ERROR = -1
    };

    enum CurrentAudioState {
        ERROR = -1,
        UNKNOWN = -2,
        STOPPED = 0,
        PAUSED = 1,
        PLAYING = 2
    };

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
        return AudioFileError::OK;
    }

    AudioFileError startAudioPlayback() {
        // are we already playing? if not, start
        if ( Mix_PlayingMusic() == 0) {
            int err = Mix_PlayMusic(fMusic, fNumLoops);
            if (err == -1) {
                cerr << "Can't play back audio file for some reason... are you use the player is loaded?\n";
                fState = ERROR;
                return AudioFileError::CANT_PLAY;
            }
        } else if ( Mix_PausedMusic() == 1 ) {
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
private:
    string fPath;
    Mix_Music *fMusic;
    int fNumLoops;  // -1 indicates loop forever, 0 = don't play? why would i not want to play? it's so bad that I now have cancer
    CurrentAudioState fState;
};