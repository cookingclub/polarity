#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include <GL/gl.h>
namespace Polarity {


bool loopIter(Canvas *screen) {
    screen->beginFrame();
    screen->clear();
    SDL_Event event;
    std::vector<int> keyUps;
    // Maintain a strong reference to world so we can handle world changes.
    std::shared_ptr<World> currentWorldRef (world);
    // SDL_FillRect(screen, NULL, 0xffffffff);
    while (screen->getNextEvent(&event)) {
        if (
#if SDL_MAJOR_VERSION >= 2
            event.type == SDL_WINDOWEVENT_RESIZED
#else
            event.type == SDL_VIDEORESIZE
#endif
        ) {
#if SDL_MAJOR_VERSION >= 2
            fprintf(stderr, "TEST\n");//%d %d\n", event.resize.w, event.resize.h);
#else
            fprintf(stderr, "TEST %d %d\n", event.resize.w, event.resize.h);
            screen->resize(event.resize.w, event.resize.h);
#endif
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return false;
                }
                world->keyEvent(event.key.keysym.sym, true);
            } else {
                keyUps.push_back(event.key.keysym.sym);
            }
        }
    }
    world->tick();
    completeAllPendingCallbacksFromMainThread();
    world->draw(screen);
    // all key up have to happen after key downs so we get a full tick of downs
    for (auto &key : keyUps) {
      world->keyEvent(key, false);
    }
    screen->endFrame();
    return true;
}
}
