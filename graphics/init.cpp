#include <assert.h>
#include "init.hpp"
#include "canvas.hpp"
#include "opengl_canvas.hpp"
#include "sdl_canvas.hpp"
#include "util/async_io_task.hpp"
#if SDL_MAJOR_VERSION >= 2
#include <SDL2/SDL_ttf.h>
#else
#include <SDL/SDL_ttf.h>
#endif

namespace Polarity {
std::shared_ptr<AsyncIOTask> createAsyncIoTask(){
    std::shared_ptr<Polarity::AsyncIOTask> localAsyncIOTask(new Polarity::AsyncIOTask);
    return localAsyncIOTask;
}
static bool graphicsInitialized = false;
int initGraphicsSystem() {
    int retval = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO |
                          SDL_INIT_TIMER);
    if (retval) {
        std::cerr << "Failed to init SDL\n";
        return -1;
    }
    if (TTF_Init()) {
        std::cerr << "Failed to init TTF\n";
        return -1;
    }
#if SDL_MAJOR_VERSION < 2
    SDL_EnableUNICODE(SDL_ENABLE);
#endif
    graphicsInitialized = true;
    return retval;

}
Canvas* makeGraphicsCanvas(const std::shared_ptr<AsyncIOTask>& asyncIOTask,
                           const char *renderer_type, int initial_width, int initial_height) {
    assert(graphicsInitialized);
    if (strstr(renderer_type, "SDL") || strstr(renderer_type, "sdl")) {
        return new Polarity::SDLCanvas(asyncIOTask, initial_width, initial_height);
    }
    return new Polarity::OpenGLCanvas(asyncIOTask, initial_width, initial_height);
}
void destroyGraphicsSystem() {
    assert(graphicsInitialized);
    SDL_AudioQuit();
#ifndef EMSCRIPTEN
    TTF_Quit();
    SDL_Quit();
#endif
    graphicsInitialized = false;
}
}
