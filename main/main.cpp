#include <mutex>
#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif
#include <mutex>
#include <thread>
#include <deque>
#include <condition_variable>
#ifdef USE_SDL2
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#else
#include <SDL/SDL_video.h>
#include <SDL/SDL_timer.h>
#include <SDL/SDL_ttf.h>
#endif
#include "graphics/init.hpp"
#include "world/sounds.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "physics/behavior.hpp"
#include "util/async_io_task.hpp"

using namespace std;
#ifndef DEFAULT_RENDERER
#define DEFAULT_RENDERER "SDL"
#endif
const int CANVAS_WIDTH = 1280;
const int CANVAS_HEIGHT = 720;


namespace Polarity {

static std::shared_ptr<Canvas> screen;
std::shared_ptr<AsyncIOTask> asyncIOTask;
std::shared_ptr<AudioChannelPlayer> audioPlayer;
std::shared_ptr<PlayerState> playerState (new PlayerState());
std::shared_ptr<GameState> gameState;


AsyncIOTask& getAsyncIOTask() {
    return *asyncIOTask;
}
void exitProgram() {
    Polarity::asyncIOTask->quiesce();
    Polarity::world.reset();
    Polarity::screen.reset();
    Polarity::asyncIOTask->quiesce();
    Polarity::asyncIOTask.reset();
    Polarity::destroyGraphicsSystem();
}

#ifdef EMSCRIPTEN
extern "C" {
    void windowResize(int x, int y) {
        if (Polarity::screen) {
            Polarity::screen->resize(x, y);
        }
    }
}

bool exited = false;
void emLoopIter() {
    if (exited == false && !loopIter(screen.get())) {
        exitProgram();
        exited = true;
    }
}

void mainloop() {
    emscripten_set_main_loop(&emLoopIter, 0, 1);
}

#else
void mainloop() {
    Uint32 time = SDL_GetTicks();
    while (true) {
        if (!loopIter(screen.get())) {
            break;
        }
        Uint32 newTime = SDL_GetTicks();
        Uint32 curDelay = newTime - time;
        if (curDelay < 1000/60) {
            SDL_Delay(1000/60 - curDelay);
        }
        time = SDL_GetTicks();
    }
    exitProgram();
}
#endif
}

int main(int argc, char**argv) {
    const char *renderer_type = DEFAULT_RENDERER;
    if (argc > 1) {
        renderer_type = argv[1];
    }
    Polarity::initGraphicsSystem();
    std::shared_ptr<Polarity::AsyncIOTask> localAsyncIOTask(new Polarity::AsyncIOTask);
    Polarity::asyncIOTask = localAsyncIOTask;
    Polarity::screen.reset(Polarity::makeGraphicsCanvas(localAsyncIOTask, renderer_type, CANVAS_WIDTH, CANVAS_HEIGHT));

    srand(time(NULL));
    Polarity::audioPlayer = Polarity::loadAudioChannels();
    Polarity::World::init(Polarity::screen, Polarity::audioPlayer, Polarity::playerState, Polarity::gameState);
    Polarity::mainloop();
    Polarity::screen.reset();
    std::cerr<<"Thank you for playing polarity"<<std::endl;
    return 0;
}
