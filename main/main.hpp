#ifndef POLARITY_MAIN_MAIN_HPP__
#define POLARITY_MAIN_MAIN_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>

namespace Polarity {
    class Canvas;
    void completeAllPendingCallbacksFromMainThread();
    void mainThreadCallback(const std::function<void()>&&);
    void asyncFileLoad(const std::string &fname,
                       const std::function<void(const char *data, int size)>&callback);
    bool loopIter(Canvas *screen);
    void exitProgram();
}

#endif
