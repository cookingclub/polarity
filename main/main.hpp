#ifndef POLARITY_MAIN_MAIN_HPP__
#define POLARITY_MAIN_MAIN_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>

namespace Polarity {
    class AsyncIOTask;
    class Canvas;
    AsyncIOTask& getAsyncIOTask();
    bool loopIter(Canvas *screen);
}

#endif
