#ifdef EMSCRIPTEN
#include "emscripten.h"
#endif

#include "graphics/sdl_canvas.hpp"
#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "physics/behavior.hpp"
using namespace std;

const int AUDIO_RATE = 44100; // FIXME: Does this matter?
const int CANVAS_WIDTH = 1280;
const int CANVAS_HEIGHT = 720;


namespace Polarity {

static std::shared_ptr<Canvas> screen;

std::shared_ptr<AudioChannelPlayer> audioPlayer;
std::shared_ptr<PlayerState> playerState (new PlayerState());
std::shared_ptr<GameState> gameState;

extern void loadAssets();
#ifdef EMSCRIPTEN
#define LOCK_MAIN_THREAD_CALLBACK_MUTEX()
#else
namespace {
std::mutex mainThreadCallbackMutex;
}
#define LOCK_MAIN_THREAD_CALLBACK_MUTEX() std::lock_guard(mainThreadCallbackMutex)
#endif
namespace {
std::vector<std::function<void()> > functionsToCallOnMainThread;
}
void completeAllPendingCallbacksFromMainThread(){
    std::vector<std::function<void()> > savedFunctionsToCallOnMainThread;
    {   // keep the function lock as short as possible
        LOCK_MAIN_THREAD_CALLBACK_MUTEX();
        functionsToCallOnMainThread.swap(savedFunctionsToCallOnMainThread);
    }
    for (auto &func : savedFunctionsToCallOnMainThread) {
        func();
    }
    savedFunctionsToCallOnMainThread.resize(0);
    {   // keep the function lock as short as possible
        LOCK_MAIN_THREAD_CALLBACK_MUTEX();
        functionsToCallOnMainThread.swap(savedFunctionsToCallOnMainThread);
    }
    for (auto &func : savedFunctionsToCallOnMainThread) {
        func();
    }
}

#if 0//def EMSCRIPTEN
static void asyncFileLoadOnLoad(void*ctx, void *data, unsigned*size){
    auto cb = static_cast<std::function<void(const char * data, int size)>*>(ctx);
    *cb(data, *size);
}
static void asyncFileLoadOnError(void*ctx, int, const char*){
    auto cb = static_cast<std::function<void(const char * data, int size)>*>(ctx);
    *cb(nullptr, -1);
}
static void asyncFileLoadOnProgress(void*, int, int){
}
void asyncFileLoad(const std::string fileName,
                   const std::function<void(const char * data, int size)>&callback) {
    auto cb = new std::function<void(const char * data, int size)>(callback);
    emscripten_async_wget2_data(fileName, "GET", "", cb, true, asyncFileLoadOnLoad, asyncFileLoadOnError, asyncFileLoadOnProgress);
}
#else

//FIXME: USE PTHREAD so that the disk load and the async processing happen on a worker thread
void asyncFileLoad(const std::string &fileName,
                   const std::function<void(const char * data, int size)>&callback) {
    FILE * fp = fopen(fileName.c_str(), "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *data = (char*)malloc(size);
        callback(data, size);
        free(data);
    }else {
        callback(NULL, -1);
    }
}
#endif
void mainThreadCallback(const std::function<void()>&&function) {
    LOCK_MAIN_THREAD_CALLBACK_MUTEX();
    functionsToCallOnMainThread.push_back(std::move(function));
}




#ifdef EMSCRIPTEN
void emLoopIter() {
    if (!loopIter(screen.get())) {
        SDL_Quit();
    }
    screen->swapBuffers();
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
        } else {
            screen->swapBuffers();
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

    Polarity::screen.reset(new Polarity::SDLCanvas(CANVAS_WIDTH, CANVAS_HEIGHT));

    // SDL_MapRGB(screen->format, 65, 65, 65);
    srand(time(NULL));
    Polarity::loadAssets();
    Polarity::World::init(Polarity::screen, Polarity::audioPlayer, Polarity::playerState, Polarity::gameState);
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
