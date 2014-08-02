#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

#include "audio/audio.hpp"
#include "main/main.hpp"

using namespace std;

const int AUDIO_RATE = 44100; // FIXME: Does this matter?
const int CANVAS_WIDTH = 800;
const int CANVAS_HEIGHT = 600;


static SDL_Surface *screen;
static AudioFile audioTest;

namespace Polarity {

#ifdef EMSCRIPTEN
void emLoopIter() {
    if (!loopIter(screen)) {
        SDL_Quit();
    }
}

void mainloop() {
    emscripten_set_main_loop(&emLoopIter, 0, 1);
}

#else
void mainloop() {
    atexit(SDL_Quit);
    while (true) {
        if (!loopIter(screen)) {
            break;
        }
    }
}
#endif
}

int main() {
    int retval = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO |
                          SDL_INIT_TIMER);
    if (retval) {
        cerr << "Failed to init SDL" << endl;
        return 1;
    }
    if (TTF_Init()) {
        cerr << "Failed to init TTF" << endl;
        return 1;
    }
    SDL_EnableUNICODE(SDL_ENABLE);
    if (Mix_OpenAudio(AUDIO_RATE, AUDIO_S16, 2, 4096)) {
        cerr << "Failed to init Audio" << endl;
        return 1;
    }
    screen = SDL_SetVideoMode(
            CANVAS_WIDTH, CANVAS_HEIGHT, 0,
            SDL_HWSURFACE | SDL_RESIZABLE);

    SDL_MapRGB(screen->format, 65, 65, 65);
    srand(time(NULL));

    audioTest = AudioFile("assets/bathed_in_the_light.mp3");
    if (audioTest.validateMusicLoaded()) {
        cerr << "Couldn't load audio" << endl;
    }

    Polarity::mainloop();
}


