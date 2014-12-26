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
#include <SDL/SDL_timer.h>
#include <SDL/SDL_ttf.h>
#endif
#include "graphics/sdl_canvas.hpp"
#include "graphics/opengl_canvas.hpp"
#include "audio/audio.hpp"
#include "main/main.hpp"
#include "world/world.hpp"
#include "physics/behavior.hpp"
using namespace std;

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
using std::mutex;
namespace {
mutex mainThreadCallbackMutex;
}
#define LOCK_MAIN_THREAD_CALLBACK_MUTEX() std::unique_lock<mutex> local_lock(mainThreadCallbackMutex)
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

#ifdef EMSCRIPTEN
static void asyncFileLoadOnLoad(unsigned handle, void*ctx, void *data, size_t size){
    auto cb = reinterpret_cast<std::function<void(const char * data, int size)>*>(ctx);
    (*cb)(reinterpret_cast<const char*>(data), (size_t)size);
    delete cb;
}
static void asyncFileLoadOnError(unsigned handle, void*ctx, int, const char*){
    auto cb = reinterpret_cast<std::function<void(const char * data, int size)>*>(ctx);
    (*cb)(nullptr, -1);
    delete cb;
}

// Compatibility APIs.
static void asyncFileLoadOnLoad(void*ctx, void *data, unsigned *size){
    asyncFileLoadOnLoad(0, ctx, data, reinterpret_cast<size_t>(size));
}
static void asyncFileLoadOnLoad(void*ctx, void *data, unsigned size){
    asyncFileLoadOnLoad(0, ctx, data, size);
}
static void asyncFileLoadOnError(void*ctx, int size, const char*data){
    asyncFileLoadOnError(0, ctx, size, data);
}

void asyncFileLoad(const std::string &fileName,
                   const std::function<void(const char * data, int size)>&callback) {
    auto cb = new std::function<void(const char * data, int size)>(callback);
    emscripten_async_wget2_data(fileName.c_str(), "GET", "", cb, true, (em_async_wget2_data_onload_func)&asyncFileLoadOnLoad, &asyncFileLoadOnError, 0);
}
void platformExitProgram() {
}
#else
namespace {
mutex workerWorkMutex;
std::condition_variable workerWorkCondition;
}
std::deque<std::function<void()> >work;
std::vector<std::thread> workers;
void platformExitProgram() {
    for (size_t i=0;i<workers.size();++i) {
        std::unique_lock<mutex> workLock(workerWorkMutex);
        work.emplace_back(function<void()>());
	workerWorkCondition.notify_all();
    }
    for (size_t i=0;i<workers.size();++i) {
        workers[i].join();
    }
}

void worker() {
    while (true) {
        std::function<void()> f;
        {
            std::unique_lock<mutex> workLock(workerWorkMutex);
            while (work.empty()) {
                workerWorkCondition.wait(workLock);
            }
            f = std::move(work.front());
            work.pop_front();
        }
        if (f) {
            f();
        } else {
            return;
        }
    }
}
//FIXME: USE PTHREAD so that the disk load and the async processing happen on a worker thread
void asyncFileLoadHelper(const std::string &fileName,
                   const std::function<void(const char * data, int size)>&callback) {
    FILE * fp = fopen(fileName.c_str(), "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *data = (char*)malloc(size);
        fread(data, 1, size, fp);
        fclose(fp);
        callback(data, size);
        free(data);
    }else {
        callback(NULL, -1);
    }
}
void asyncFileLoadX(const std::string &fileName,
                   const std::function<void(const char * data, int size)>&callback) {
  asyncFileLoadHelper(fileName, callback);
}
void asyncFileLoad(const std::string &fileName,
                   const std::function<void(const char * data, int size)>&callback) {
    std::unique_lock<mutex> workLock(workerWorkMutex);
    if (workers.empty()) {
        for (int i=0; i < 3; ++i) {
            workers.emplace_back(&worker);
        }
    }
    work.emplace_back(std::bind(&asyncFileLoadHelper, fileName, std::move(callback)));
    workerWorkCondition.notify_all();
}
#endif

void exitProgram() {
    platformExitProgram();
    Polarity::world.reset();
    Polarity::screen.reset();
    {
        LOCK_MAIN_THREAD_CALLBACK_MUTEX();
        functionsToCallOnMainThread.clear();
    }
}
void mainThreadCallback(const std::function<void()>&&function) {
    LOCK_MAIN_THREAD_CALLBACK_MUTEX();
    functionsToCallOnMainThread.push_back(std::move(function));
}




#ifdef EMSCRIPTEN
void emLoopIter() {
    if (!loopIter(screen.get())) {
        exitProgram();
        SDL_Quit();
    }
}

void mainloop() {
    emscripten_set_main_loop(&emLoopIter, 0, 1);
}

#else
void mainloop() {
    atexit(SDL_Quit);
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
    std::weak_ptr<int>test0;
    if (!test0.lock()) {
        cerr << "OK2"<<std::endl;
    }
    {
        int * itest = nullptr;
        itest = new int(4);
        std::shared_ptr<int> test(itest);
        std::weak_ptr<int> test1(test);
        test0 = test1;
        if (test1.lock() && test0.lock()) {
            cerr << *test0.lock() << "OK1"<<std::endl;
        }
    }
    if (!test0.lock()) {
        cerr << "OK0"<<std::endl;
    }
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
#if SDL_MAJOR_VERSION < 2
    SDL_EnableUNICODE(SDL_ENABLE);
#endif

    //Polarity::screen.reset(new Polarity::SDLCanvas(CANVAS_WIDTH, CANVAS_HEIGHT));
    Polarity::screen.reset(new Polarity::OpenGLCanvas(CANVAS_WIDTH, CANVAS_HEIGHT));

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
    Polarity::screen.reset();
    TTF_Quit();
    std::cerr<<"Game over, man"<<std::endl;
    return 0;
}
