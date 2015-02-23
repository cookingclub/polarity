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
#include "main/game.hpp"
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
SDL_Event *event;
bool loopIter(Canvas *screen) {
    std::vector<int> keyUps;
    // Maintain a strong reference to world so we can handle world changes.
    std::shared_ptr<World> currentWorldRef (Game::getSingleton().pinWorld());
    // SDL_FillRect(screen, NULL, 0xffffffff);
    while (screen->getNextEvent(event)) {
        if (!Game::getSingleton().injectInput(event)) {
            return false;
        }
    }
    Game::getSingleton().performTick();
    return true;
}
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
    if (exited == false && !loopIter(Polarity::screen.get())) {
        Polarity::Game::getSingleton().stopGameAndCleanupGraphicsAndEvents();
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
        if (!loopIter(Polarity::screen.get())) {
            break;
        }
        Uint32 newTime = SDL_GetTicks();
        Uint32 curDelay = newTime - time;
        if (curDelay < 1000/60) {
            SDL_Delay(1000/60 - curDelay);
        }
        time = SDL_GetTicks();
    }
}
#endif


int main(int argc, char**argv) {
    const char *renderer_type = DEFAULT_RENDERER;
    if (argc > 1) {
        renderer_type = argv[1];
    }
    Polarity::initGraphicsSystem();
    std::shared_ptr<Polarity::AsyncIOTask> localAsyncIOTask(new Polarity::AsyncIOTask);
    Polarity::screen.reset(Polarity::makeGraphicsCanvas(localAsyncIOTask, renderer_type, CANVAS_WIDTH, CANVAS_HEIGHT));
    Polarity::event = Polarity::screen->makeBlankEventUnion();

    srand(time(NULL));
    Polarity::Game::getSingleton().startGame(Polarity::screen, "assets/levels/level2.tmx");
    mainloop();
    Polarity::screen->destroyEventUnion(Polarity::event);
    Polarity::event = NULL;
    Polarity::screen.reset();
    Polarity::Game::getSingleton().stopGameAndCleanupGraphicsAndEvents();
    std::cerr<<"Thank you for playing polarity"<<std::endl;
    return 0;
}
