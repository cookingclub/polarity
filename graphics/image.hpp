#ifndef POLARITY_GRAPHICS_IMAGE_HPP__
#define POLARITY_GRAPHICS_IMAGE_HPP__

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>
#include "rect.hpp"

namespace Polarity {

class Canvas;
class DisplayList;


class Image {
protected:
    friend class Animation;
    static std::shared_ptr<Image> getValidImage(const std::string&filename);
    Image(const Image&x) = delete;
    Image& operator=(const Image&x) = delete;
    std::string filename;
    enum LoadStage{
        FAILED = -1,
        COMPLETE = 0,
        LOADING = 1,
    };
    LoadStage stage;
public:
    class DecodedImage {
    public:
        enum Format{
            L=1,
            LA=2,
            RGB=3,
            RGBA=4
        };

        DecodedImage(int w, int h, Format f) {
            width = w;
            height = h;
            format = f;
        }
        DecodedImage(const DecodedImage&) = delete;
        DecodedImage& operator=(const DecodedImage&)const = delete;
        int pixelSize() const{
            return (int)format;
        }
        unsigned int width;
        unsigned int height;
        Format format;
        std::vector<unsigned char> data;
    };
    static void parseAndLoad(Image *img, const std::string &filename,
                             const std::function<void(Image*,
                                                      const std::string&,
                                                      const std::shared_ptr<DecodedImage>&)>&mainThreadCallback,
                             const char *data, int size);
    Image(const std::string &filename):filename(filename) { stage = LOADING;}
    void downloadAsync(const std::function<void(const char*data, int size)>&callback);

    virtual ~Image() {}

    static std::shared_ptr<Image> get(Canvas *canvas, const std::string &filename);
    void draw(Canvas *screen, int x, int y, float angle);
    struct BlitDescription {
        Rect src;
        float centerX;
        float centerY;
        float scaleX;
        float scaleY;
        float angle;
    };
    virtual void draw(Canvas *screen, const BlitDescription &desc, int x, int y) {
        drawSprite(screen,
                   desc.src, desc.centerX + x, desc.centerY + y,
                   desc.scaleX, desc.scaleY, desc.angle);
    }
    virtual void drawSprite(Canvas *screen, const Rect &src, float centerX, float centerY,
                            float scaleX, float scaleY, float angle);
    virtual void drawSubimage(Canvas *screen, const Rect& src, int x, int y, float angle);
    virtual int width() = 0;
    virtual int height() = 0;
    bool isLoaded() const{
        return stage == COMPLETE;
    }
    const std::string& sourceUrl() const{
        return filename;
    }
};

}

#endif
