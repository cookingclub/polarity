#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "physics/behavior.hpp"
using namespace std;

const int AUDIO_RATE = 44100; // FIXME: Does this matter?
const int CANVAS_WIDTH = 1280;
const int CANVAS_HEIGHT = 720;


static SDL_Surface *screen;

namespace Polarity {

std::shared_ptr<AudioChannelPlayer> audioPlayer;
extern void loadAssets();

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
    Polarity::loadAssets();
    Polarity::World::init();
/*
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 40.0f);
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(2.0f, 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    Polarity::world->addObject(new Polarity::KeyboardBehavior(), bodyDef, fixtureDef);

    b2BodyDef bodyDef2;
    bodyDef2.type = b2_staticBody;
    bodyDef2.position.Set(0.0f, 10.0f);
    Polarity::world->addObject(new Polarity::KeyboardBehavior(), bodyDef2, fixtureDef);
*/
    Polarity::mainloop();
}
