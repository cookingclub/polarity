#ifndef _GRAPHICS_INIT_HPP_
#define _GRAPHICS_INIT_HPP_
#include <memory>
namespace Polarity {
class AsyncIOTask;
class Canvas;


int initGraphicsSystem(); // returns -1 on error
Canvas * makeGraphicsCanvas(const std::shared_ptr<AsyncIOTask>& asyncIOTask,
                            const char * renderer_type, int initial_width, int initial_height);
void destroyGraphicsSystem();
}

#endif
