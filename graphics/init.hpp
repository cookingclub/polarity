#ifndef _GRAPHICS_INIT_HPP_
#define _GRAPHICS_INIT_HPP_
#include <memory>
#include "util/shared.hpp"
namespace Polarity {
class AsyncIOTask;
class Canvas;
POLARITYGFX_FUNCTION_EXPORT std::shared_ptr<AsyncIOTask> createAsyncIoTask();
POLARITYGFX_FUNCTION_EXPORT int initGraphicsSystem(); // returns -1 on error
POLARITYGFX_FUNCTION_EXPORT Canvas * makeGraphicsCanvas(const std::shared_ptr<AsyncIOTask>& asyncIOTask, const char * renderer_type, int initial_width, int initial_height);
POLARITYGFX_FUNCTION_EXPORT void destroyGraphicsSystem();
}

#endif
