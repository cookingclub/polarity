#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
#ifndef EMSCRIPTEN
#include <thread>
#endif
#include "util/shared.hpp"
namespace Polarity {
class POLARITYGFX_EXPORT AsyncIOTask{
public:
    AsyncIOTask();
    // loads the file into ram and calls back on a different thread
    void asyncFileLoad(const std::string &fileName,
                       const std::function<void(const char * data, int size)>&callback);
    void mainThreadCallback(const std::function<void()>&&function);
    void callPendingCallbacksFromMainThread();
    /// Terminate and join all workers
    void quiesce();
    ~AsyncIOTask();
private:
    std::vector<std::function<void()> > functionsToCallOnMainThread;

#ifndef EMSCRIPTEN
    std::deque<std::function<void()> >mWork;
    std::vector<std::thread> mWorkers;
    std::mutex mWorkerWorkMutex;
    std::mutex mMainThreadCallbackMutex;
    std::condition_variable mWorkerWorkCondition;
    void worker();
#endif
};

}

