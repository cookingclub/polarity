#include "graphics/graphics.hpp"
#include <unordered_map>

namespace Polarity {
SDL_Surface *loadImage(const std::string &filename) {
    SDL_Surface *image, *tmpImage;
    tmpImage = IMG_Load(filename.c_str());
    if (!tmpImage) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    image = SDL_DisplayFormatAlpha(tmpImage);
    SDL_FreeSurface(tmpImage);
    return image;
}

std::unordered_map<std::string, std::weak_ptr<Image>> imagesCache;

Image::Image(const std::string& filename) {
    surf = loadImage(filename);
}

std::shared_ptr<Image> Image::get(const std::string& filename) {
    auto it = imagesCache.find(filename);
    if (it != imagesCache.end()) {
        std::shared_ptr<Image> imPtr = it->second.lock();
        if (imPtr) {
            return imPtr;
        }
    }
    std::shared_ptr<Image> imPtr(new Image(filename));
    imagesCache[filename] = imPtr;
    return imPtr;
}

Image::~Image() {
    SDL_FreeSurface(surf);
}

void Image::draw(SDL_Surface *screen, SDL_Rect *src, int x, int y) {
    SDL_Rect dst;
    dst.w = surf->w;
    dst.h = surf->h;
    dst.x = x;
    dst.y = y;
    SDL_BlitSurface(surf, src, screen, &dst);
}

void Image::draw(SDL_Surface *screen, int x, int y) {
    SDL_Rect src;
    src.w = surf->w;
    src.h = surf->h;
    src.x = 0;
    src.y = 0;
    draw(screen, &src, x, y);
}




Animation::Animation(const std::string& filename, const std::string &ext, int numFrames) {
    frame = 0;
    lastTime = SDL_GetTicks();
    running = true;
    frameTime = 1.0/60.0;
    if (numFrames > 999) { // just so we don't get weird behavior
      numFrames = 999;
      std::cerr << "Truncating animation to 999 frames" << std::endl;
    }
    char numberTarget[64]={0};
    char * mdata = strdup(filename.c_str());
    for (int i = 0; i < numFrames; ++i) {
      sprintf(numberTarget, "%d.", i);
      std::string mdata = filename + numberTarget + ext;
      images.emplace_back(new Image(mdata));
    }
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
      std::cerr << "Animation filename of wrong pattern: no dash " << filename <<std::endl;
    }
    return std::shared_ptr<Animation>();
}

Animation::~Animation() {
    SDL_FreeSurface(surf);
}

void Animation::start() {
    running = true;
}

void Animation::pause() {
    running = false;
}

size_t Animation::getFrame() {
  long long curTime = SDL_GetTicks();
  size_t ticksPerFrame = frameTime * 1000;
  size_t deltaTicks = curTime - lastTime;
  size_t deltaFrames = deltaTicks / ticksPerFrame;
  lastTime += deltaFrames * ticksPerFrame;
  frame += deltaFrames;
  if (!images.empty()) {
    frame %= images.size();
  }
  return frame;
}

void Animation::draw(SDL_Surface *screen, SDL_Rect *src, int x, int y) {
  size_t frame = getFrame();
  if (!images.empty()) {
    images[frame]->draw(screen, src, x, y);
  }else {
    std::cerr << "Drawing animation empty" << std::endl;
  }
}

void Animation::draw(SDL_Surface *screen, int x, int y) {
  size_t frame = getFrame();
  if (!images.empty()) {
    images[frame]->draw(screen, x, y);
  } else {
    std::cerr << "Drawing animation empty" << std::endl;
  }
}

}
