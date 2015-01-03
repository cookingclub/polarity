#include <string>
#include <functional>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#include "async_io_task.hpp"
using std::mutex;
using std::condition_variable;
using std::function;
namespace Polarity {
AsyncIOTask::AsyncIOTask() {

}

#ifdef EMSCRIPTEN
#define LOCK_MAIN_THREAD_CALLBACK_MUTEX()
#else
#define LOCK_MAIN_THREAD_CALLBACK_MUTEX() std::unique_lock<mutex> local_lock(mMainThreadCallbackMutex)
#endif



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


void AsyncIOTask::asyncFileLoad(const std::string &fileName,
                   const std::function<void(const char * data, int size)>&callback) {
    auto cb = new std::function<void(const char * data, int size)>(callback);
    const char* file_name_cstr = fileName.c_str();
    emscripten_async_wget2_data(file_name_cstr, "GET", "", cb, true, (em_async_wget2_data_onload_func)&asyncFileLoadOnLoad, &asyncFileLoadOnError, 0);
}
#else

void AsyncIOTask::worker() {
    while (true) {
        std::function<void()> f;
        {
            std::unique_lock<mutex> workLock(mWorkerWorkMutex);
            while (mWork.empty()) {
                mWorkerWorkCondition.wait(workLock);
            }
            f = std::move(mWork.front());
            mWork.pop_front();
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
void AsyncIOTask::asyncFileLoad(const std::string &fileName,
                   const std::function<void(const char * data, int size)>&callback) {
    std::unique_lock<mutex> workLock(mWorkerWorkMutex);
    if (mWorkers.empty()) {
        for (int i=0; i < 3; ++i) {
            mWorkers.emplace_back(std::bind(&AsyncIOTask::worker, this));
        }
    }
    mWork.emplace_back(std::bind(&asyncFileLoadHelper, fileName, std::move(callback)));
    mWorkerWorkCondition.notify_all();
}
#endif

void AsyncIOTask::mainThreadCallback(const std::function<void()>&&function) {
    LOCK_MAIN_THREAD_CALLBACK_MUTEX();
    functionsToCallOnMainThread.push_back(std::move(function));
}

void AsyncIOTask::callPendingCallbacksFromMainThread(){
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

void AsyncIOTask::quiesce() {
#ifndef EMSCRIPTEN
    for (size_t i=0;i<mWorkers.size();++i) {
        std::unique_lock<mutex> workLock(mWorkerWorkMutex);
        mWork.emplace_back(function<void()>());
        mWorkerWorkCondition.notify_all();
    }
    for (size_t i=0;i<mWorkers.size();++i) {
        mWorkers[i].join();
    }
    mWorkers.clear();
    {
        LOCK_MAIN_THREAD_CALLBACK_MUTEX();
        functionsToCallOnMainThread.clear();
    }
#endif
}
AsyncIOTask::~AsyncIOTask() {

}
}
