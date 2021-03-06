#include "graphics/image.hpp"
#include "graphics/animation.hpp"
#include <unordered_map>

#ifdef USE_SDL2
#include "SDL2/SDL.h" // SDL_GetTicks()
#else
#include "SDL/SDL.h" // SDL_GetTicks()
#endif

namespace Polarity {

Animation::Animation(Canvas *canvas, const std::string& filename, const std::string &ext, int numFrames) : Image(filename) {
    frame = 0;
    lastTime = SDL_GetTicks();
    running = true;
    loop = true;
    frameTime = 1.0/16.0;
    if (numFrames > 999) { // just so we don't get weird behavior
      numFrames = 999;
      std::cerr << "Truncating animation to 999 frames" << std::endl;
    }
    char numberTarget[64]={0};
    for (int i = 0; i < numFrames; ++i) {
      if (numFrames > 1000) {
          sprintf(numberTarget, "%d.", i);
      } else if (numFrames > 100) {
          sprintf(numberTarget, "%03d.", i);
      }else if (numFrames > 10) {
          sprintf(numberTarget, "%02d.", i);
      } else {
          sprintf(numberTarget, "%02d.", i);
      }
      std::string mdata = filename + numberTarget + ext;
      images.push_back(Image::get(canvas, mdata));
    }
    if (numFrames == 0) {
      images.push_back(Image::get(canvas, filename));
    }
}

std::shared_ptr<Animation> Animation::get(Canvas *canvas, const std::string& filename) {
  std::string::size_type extwhere = filename.find_last_of(".");
  std::string ext = filename.substr(extwhere + 1);
  std::string basename = filename.substr(0, extwhere);
    std::string::size_type where = basename.find_last_of("-");
    if (where !=std::string::npos) {
       const char * data (basename.c_str());
       const char * numFramesStr = data + where + 1;
       int numFrames = 1;
       sscanf(numFramesStr, "%d", &numFrames);
       return std::shared_ptr<Animation>(new Animation(canvas,
                               basename.substr(0,where + 1),
                               ext,
                               numFrames));
    }else {
        return std::shared_ptr<Animation>(new Animation(canvas, basename,
                                                       std::string(),
                                                       0));
    }
}

Animation::~Animation() {
}

void Animation::restart() {
    frame = 0;
    start();
}

void Animation::start() {
    running = true;
    lastTime = SDL_GetTicks();
}

void Animation::pause() {
    running = false;
}

size_t Animation::getFrame() {
    if (images.empty()) {
        std::cerr << "No frames in surface: shouldn't happen." << std::endl;
        return 0;
    }
    long long curTime = SDL_GetTicks();
    size_t ticksPerFrame = frameTime * 1000;
    size_t deltaTicks = curTime - lastTime;
    size_t deltaFrames = deltaTicks / ticksPerFrame;
    lastTime += deltaFrames * ticksPerFrame;
    if (!running) {
        return frame;
    }
    size_t newFrame = frame + deltaFrames;
    if (newFrame >= images.size() && !loop) {
        newFrame = 0;
        running = false;
    } else {
        frame = newFrame % images.size();
    }
    return frame;
}
void Animation::drawSprite(Canvas *screen,
                           float centerX, float centerY,
                           float scaleX, float scaleY, float angle, float alpha) {
    size_t frame = getFrame();
    if (!images.empty()) {
        images[frame]->drawSprite(screen, centerX, centerY, scaleX, scaleY, angle, alpha);
    }else {
        std::cerr << "No frames in surface: shouldn't happen." << std::endl;
    }
}

int Animation::width() {
    if (images.empty()) {
        return 0;
    }
    return images[frame]->width();
}

int Animation::height() {
    if (images.empty()) {
        return 0;
    }
    return images[frame]->height();
}

}
