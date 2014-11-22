#include <cstring>
#include "graphics/image.hpp"
#include "graphics/sdl_canvas.hpp"
#include <unordered_map>
#include <png.h>
#include <assert.h>

namespace Polarity {

std::unordered_map<std::string, std::weak_ptr<Image>> imagesCache;

void Image::forEachImage(std::function<void(const std::shared_ptr<Image>&)> func) {
    for (auto val : imagesCache) {
        std::shared_ptr<Image> img = val.second.lock();
        if (img) {
            func(img);
        }
    }
}

std::shared_ptr<Image> Image::getValidImage(const std::string &filename) {
    auto where = imagesCache.find(filename);
    if (where != imagesCache.end()) {
        return where->second.lock();
    }
    return std::shared_ptr<Image>();
}

std::shared_ptr<Image> Image::get(Canvas *canvas, const std::string& filename) {
    auto it = imagesCache.find(filename);
    if (it != imagesCache.end()) {
        std::shared_ptr<Image> imPtr = it->second.lock();
        if (imPtr) {
            return imPtr;
        }
    }
    std::shared_ptr<Image> imPtr(canvas->loadImage(filename));
    imagesCache[filename] = imPtr;
    return imPtr;
}

// angle unrestricted
void Image::drawSprite(Canvas *screen,
                       float centerX, float centerY, float scaleX, float scaleY, float angle) {
    screen->drawSprite(this, centerX, centerY, scaleX, scaleY, angle);
}

void Image::draw(Canvas *screen, int x, int y, float angle) {//angle should be multiple of PI/2
    drawSprite(screen, x + width() / 2., y + height() / 2., width(), height(), angle);
}

namespace {
struct PngReadBuffer {
    const char * data;
    unsigned int size;
    unsigned int offset;
    bool error;
};
}

static void readPngCallback(png_structp userData, png_byte* destination, png_size_t size) {
    PngReadBuffer* handle = reinterpret_cast<PngReadBuffer*>(png_get_io_ptr(userData));
    if (handle->offset + size > handle->size) {
        size = handle->size - handle->offset;
        handle->error = true;
    }
    std::memcpy(destination, handle->data + handle->offset, size);
    handle->offset += size;
}
namespace {
void postFailure(Image *img,
                 const std::string &filename,
                 const std::function<void(Image*,
                                          const std::string&,
                                          const std::shared_ptr<Image::DecodedImage>&)>&callback) {
    callback(img, filename,
             std::shared_ptr<Image::DecodedImage>(new Image::DecodedImage(0,0,Image::DecodedImage::L)));
}
void readPngImage(png_structp pngState, png_infop infoPtr, Image::DecodedImage *png) {
    const png_size_t rowSize = png_get_rowbytes(pngState, infoPtr);
    int height = png->height;
    if (!height) {
        return;
    }
    const int dataLength = rowSize * height;
    assert((unsigned int)dataLength == png->data.size());

    std::vector<png_byte*> rowPtrs(height);

    for (int i = 0; i < height; i++) {
        rowPtrs[i] = &png->data[i * rowSize];
    }

    png_read_image(pngState, &rowPtrs[0]);
}
void configurePngHeader(png_structp pngState, png_infop infoPtr, bool addAlpha,
                        Image::DecodedImage*png) {
    int format, depth;
    png_uint_32 w = 0;
    png_uint_32 h = 0;
    png_read_info(pngState, infoPtr);
    png_get_IHDR(pngState, infoPtr, &w, &h, &depth, &format, nullptr, nullptr, nullptr);
    png->width = w;
    png->height = h;
    if (png_get_valid(pngState, infoPtr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(pngState); // Up 0/1 alpha channel to 8 bits
    }
    if (format == PNG_COLOR_TYPE_GRAY && depth < 8) {
        png_set_expand_gray_1_2_4_to_8(pngState); // make sure depth is 8 bits
    }
    // Convert paletted images, if needed.
    if (format == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(pngState); // palette unsupported
    }
    if (addAlpha && (format == PNG_COLOR_TYPE_PALETTE || format == PNG_COLOR_TYPE_RGB)) {
        png_set_add_alpha(pngState, 0xFF, PNG_FILLER_AFTER);
    }
    if (depth < 8) {
        png_set_packing(pngState); // align to 8 bit
    } else if (depth == 16) {
#if PNG_LIBPNG_VER_RELEASE > 46
        png_set_scale_16(pngState);
#endif
    }
    png_read_update_info(pngState, infoPtr);

    // Read the new color type after updates have been made.
    format = png_get_color_type(pngState, infoPtr);
    switch (format) {
    case PNG_COLOR_TYPE_GRAY:
        png->format = Image::DecodedImage::L;
        break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        png->format = Image::DecodedImage::LA;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        png->format = Image::DecodedImage::RGBA;
        break;
    default:
        png->format = Image::DecodedImage::RGB;
    }
    png->data.resize(png->height * png_get_rowbytes(pngState, infoPtr));
}
}
void Image::parseAndLoad(Image *img, const std::string &filename,
                         const std::function<void(Image*,
                                                  const std::string&,
                                                  const std::shared_ptr<DecodedImage>&)>&callback,
                         const char *pngData, int pngSize) {

    if (pngSize < 8 || !png_check_sig(reinterpret_cast<png_const_bytep>(pngData), 8)) {
        postFailure(img, filename, callback);
    }

    png_structp pngState = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    png_infop infoPtr = png_create_info_struct(pngState);
    PngReadBuffer pngDataHandle = {pngData, (unsigned int)pngSize, 0, false};
    png_set_read_fn(pngState, &pngDataHandle, &readPngCallback);

    if (setjmp(png_jmpbuf(pngState))) {
        postFailure(img, filename, callback);
        return;
    }

    DecodedImage *png = new DecodedImage(0, 0, DecodedImage::L);
    configurePngHeader(pngState, infoPtr, true, png);
    readPngImage(pngState, infoPtr, png);

    png_read_end(pngState, infoPtr);
    png_destroy_read_struct(&pngState, &infoPtr, nullptr);
    Polarity::mainThreadCallback(std::bind(callback, img, filename,
                                           std::shared_ptr<DecodedImage>(png)));
}
}
