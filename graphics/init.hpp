#ifndef _GRAPHICS_INIT_HPP_
#define _GRAPHICS_INIT_HPP_
namespace Polarity {
class Canvas;


int initGraphicsSystem(); // returns -1 on error
Canvas * makeGraphicsCanvas(const char * renderer_type, int initial_width, int initial_height);
void destroyGraphicsSystem();
}

#endif
