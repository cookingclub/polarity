#include "graphics/image.hpp"
#include "graphics/animation.hpp"
#include <unordered_map>

namespace Polarity {

Animation::Animation(const std::string& filename, const std::string &ext, int numFrames) {
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
    char * mdata = strdup(filename.c_str());
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
      images.push_back(Image::get(mdata));
    }
    if (numFrames == 0) {
      images.push_back(Image::get(filename));
    }
    surf = images[0]->surf;
}

std::shared_ptr<Animation> Animation::get(const std::string& filename) {
  std::string::size_type extwhere = filename.find_last_of(".");
  std::string ext = filename.substr(extwhere + 1);
  std::string basename = filename.substr(0, extwhere);
    std::string::size_type where = basename.find_last_of("-");
    if (where !=std::string::npos) {
       const char * data (basename.c_str());
       const char * numFramesStr = data + where + 1;
       int numFrames = 1;
       sscanf(numFramesStr, "%d", &numFrames);
       return std::shared_ptr<Animation>(new Animation(basename.substr(0,where + 1),
                               ext,
                               numFrames));
    }else {
       return std::shared_ptr<Animation>(new Animation(basename,
                                                       std::string(),
                                                       0));
    }
}

Animation::~Animation() {
    // do not double free the surface;
    surf = nullptr;
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

void Animation::draw(SDL_Surface *screen, const Rect &src, int x, int y) {
    size_t frame = getFrame();
    if (!images.empty()) {
        surf = images[frame]->surf;
        images[frame]->draw(screen, src, x, y);
    }else {
        std::cerr << "No frames in surface: shouldn't happen." << std::endl;
    }
}

void Animation::draw(SDL_Surface *screen, int x, int y) {
    size_t frame = getFrame();
    if (!images.empty()) {
        surf = images[frame]->surf;
        images[frame]->draw(screen, x, y);
    } else {
        std::cerr << "No frames in surface: shouldn't happen." << std::endl;
    }
}

}
