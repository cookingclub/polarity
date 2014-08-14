#ifndef POLARITY_GRAPHICS_ANIMATION_HPP__
#define POLARITY_GRAPHICS_ANIMATION_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "image.hpp"
#include "canvas.hpp"

#include "tmxparser.h"

namespace Polarity {

class Animation : public Image{
    Animation()=delete;
    Animation(const Animation&x) = delete;
    Animation &operator=(const Animation&x) = delete;
    explicit Animation(const std::string &single_frame);
    explicit Animation(const std::string &pattern, const std::string &ext, int numFrames);
    std::vector<std::shared_ptr<Image> > images;
    size_t getFrame();
public:
    void restart();
    void start();
    void pause();
    void setLoop(bool loop) { this->loop = loop; }
    ~Animation();
    long long lastTime;
    bool loop;
    bool running;
    size_t frame;
    float frameTime;
    static std::shared_ptr<Animation> get(const std::string &filename);

    void draw(Canvas* canvas, int x, int y);
    void draw(Canvas* canvas, const Rect& src, int x, int y);
};

}

#endif
