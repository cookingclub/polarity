#include "util/async_io_task.hpp"
#include "image.hpp"
#include "canvas.hpp"
#include "display_list.hpp"
#include "sdl_canvas.hpp"
#include "font_manager.hpp"
#if SDL_MAJOR_VERSION < 2
#include "SDL/SDL_gfxPrimitives.h"
#include "SDL/SDL_rotozoom.h"
#endif

namespace Polarity {

void SDLImage::loadImageSDL(Image *super,
                            const std::string &filename,
                            const std::shared_ptr<DecodedImage> &image) {
    std::shared_ptr<Image>superShared = Image::getValidImage(filename);
    if (superShared.get() == super) {
        SDLImage *thus = static_cast<SDLImage*>(superShared.get());
        if (image->width == 0 || image->height == 0) {
            std::cerr << "Failed to load image: " << filename << std::endl;
            thus->stage = FAILED;
        } else {
            unsigned char rmask[4] = {0xff,0x0,0x0,0x0};
            unsigned char gmask[4] = {0x0,0xff,0x0,0x0};
            unsigned char bmask[4] = {0x0,0x0,0xff,0x0};
            unsigned char amask[4] = {0x0,0x0,0x0,0xff};
            Uint32 mask[4] = {0, 0, 0, 0};
            memcpy(&mask[0], rmask, 4);
            if (image->format == DecodedImage::LA || image->format == DecodedImage::L) {
                memcpy(&mask[1], rmask, 4);
                memcpy(&mask[2], rmask, 4);
            } else {
                memcpy(&mask[1], gmask, 4);
                memcpy(&mask[2], bmask, 4);
            }
            if (image->format == DecodedImage::LA || image->format == DecodedImage::RGBA) {
                if (image->format == DecodedImage::LA) {
                    memcpy(&mask[3], gmask, 4);
                } else {
                    memcpy(&mask[3], amask, 4);
                }
            }
            SDL_Surface *tmpImage = SDL_CreateRGBSurface(0, //FLAGS
                                                             image->width,
                                                              image->height,
                                                         image->pixelSize() * 8,
                                                         mask[0], mask[1], mask[2], mask[3]);
            if (SDL_LockSurface(tmpImage) == 0) {
                if (tmpImage->pixels) {
                    unsigned char * pixels = static_cast<unsigned char*>(tmpImage->pixels);
                    unsigned int pixelSize = image->pixelSize();
                    unsigned int height = image->height;
                    unsigned int pitch = pixelSize * image->width;
                    unsigned int dpitch = tmpImage->pitch;
                    for (unsigned int i = 0; i < height; ++i) {
                        memcpy(pixels + dpitch * i,
                               &image->data[pitch * i],
                               pitch);
                    }
                }
                SDL_UnlockSurface(tmpImage);
            } else {
                std::cerr << "Unable to lock surface " << filename << std::endl;
            }
            thus->w = image->width;
            thus->h = image->height;
            //tmpImage = IMG_Load(filename.c_str());
            if (!tmpImage) {
                thus->stage = FAILED;
            } else {
                //thus->surf = SDL_ConvertSurface(tmpImage, canvas->surface->pixels, 0);
#if SDL_MAJOR_VERSION >= 2
                thus->surf = tmpImage;
#else
                thus->surf = SDL_DisplayFormatAlpha(tmpImage);
                SDL_FreeSurface(tmpImage);
#endif
                thus->stage = COMPLETE;
            }
        }
    }
}
void SDLImage::downloadAndLoad(const std::shared_ptr<AsyncIOTask>& asyncIOTask){
    Image * super = this;
    asyncIOTask->asyncFileLoad(filename,
                               std::bind(Image::parseAndLoad,
                                         super,
                                         filename,
                                         asyncIOTask,
                                         &SDLImage::loadImageSDL,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
}

SDLImage::SDLImage(SDL_Surface* surf) :
        Image(std::string()),
        w(surf->w),
        h(surf->h),
        surf(surf)
#if SDL_MAJOR_VERSION >= 2
        ,texture(nullptr)
#endif
{
        this->stage = COMPLETE;
}
SDLImage::SDLImage(const std::shared_ptr<AsyncIOTask>& asyncIOTask,
                   const std::string&filename) : Image(filename), w(0), h(0) {
    surf = nullptr;
#if SDL_MAJOR_VERSION >= 2
    texture = nullptr;
#endif
    if (!filename.empty()) {
        downloadAndLoad(asyncIOTask);
    }
}
SDLImage::~SDLImage() {
    if (surf != nullptr) {
        SDL_FreeSurface(surf);
    }
#if SDL_MAJOR_VERSION >= 2
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
#endif
}

void SDLImage::enableAlphaBlend() {
    if (surf != nullptr) {
#ifdef SDL_SRCALPHA
        SDL_SetAlpha(surf, SDL_SRCALPHA, 0xff);
#else
        SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_BLEND);
#endif
    }
}
void SDLImage::disableAlphaBlend() {
    if (surf != nullptr) {
#ifdef SDL_SRCALPHA
        SDL_SetAlpha(surf, 0, 0xff);//surf->format->alpha
#else
        SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_NONE);
#endif
    }
}


SDLContext::SDLContext()
#if SDL_MAJOR_VERSION >= 2
   : window(nullptr), renderer(nullptr)
#endif
{

}
SDLContext::~SDLContext() {
#if SDL_MAJOR_VERSION >= 2
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
#endif
}

SDLCanvas::SDLCanvas(SDLRenderTargetType*surf,
                     int width,
                     int height,
                     SDLCanvas *parent)
    : screen(surf), context(parent->context),
     mFontManager(FONT_CACHE_SIZE, TEXT_CACHE_SIZE), mAsyncIOTask(parent->mAsyncIOTask) {
    w = width;
    h = height;
    temp_buffer = nullptr;
}
SDLCanvas::SDLCanvas(const std::shared_ptr<AsyncIOTask>&asyncIOTask, int width, int height)
            : w(width), h(height), context(new SDLContext()),
              mFontManager(FONT_CACHE_SIZE, TEXT_CACHE_SIZE), mAsyncIOTask(asyncIOTask) {
    temp_buffer = nullptr;
#if SDL_MAJOR_VERSION >= 2
    context->window = SDL_CreateWindow("Polarity",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       width, height,
                                       SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_RESIZABLE);
    SDL_SetWindowGrab(context->window, SDL_FALSE);
    context->renderer = SDL_CreateRenderer(context->window, -1, 0);
    screen = nullptr;
#else
    screen = SDL_SetVideoMode(width,
                              height,
                              0,
                              SDL_HWSURFACE | SDL_RESIZABLE);
#endif
}
SDLCanvas::~SDLCanvas() {
#if SDL_MAJOR_VERSION >= 2
    if (screen != nullptr) {
        SDL_DestroyTexture(screen);
    }
#else
    if (temp_buffer) {
        SDL_FreeSurface(temp_buffer);
    }
    SDL_FreeSurface(screen);
#endif
}
FontManager &SDLCanvas::fontManager() {
    return mFontManager;
}
const std::shared_ptr<AsyncIOTask> &SDLCanvas::asyncIOTask() const {
    return mAsyncIOTask;
}

SDL_Rect SDLCanvas::rectToSDL(const Rect& rect) {
    SDL_Rect out;
    out.x = rect.x;
    out.y = rect.y;
    out.w = rect.w;
    out.h = rect.h;
    return out;
}
SDL_Rect SDLCanvas::rectToRotatedSDL(const Rect& rect, SDL_Surface *src, SDL_Surface *dst) {
    SDL_Rect out;
    float halfWidth = rect.w * 0.5;
    float halfHeight = rect.h * 0.5;
    float centerX = rect.x + halfWidth;
    float centerY = rect.y + halfHeight;
    float oldHalfWidth = src->w * 0.5;
    float oldHalfHeight = src->h * 0.5;
    float newHalfWidth = dst->w * 0.5;
    float newHalfHeight = dst->h * 0.5;
    halfWidth *= newHalfWidth / oldHalfWidth;
    halfHeight *= newHalfHeight / oldHalfHeight;
    out.x = centerX - halfWidth;
    out.y = centerY - halfHeight;
    out.w = halfWidth * 2.0;
    out.h = halfHeight * 2.0;
    return out;
}

SDLImage *SDLCanvas::loadImageFromSurface(SDL_Surface *surf) {
    return new SDLImage(surf);
}
SDLImage *SDLCanvas::loadImage(const std::string &filename) {
    SDLImage *retval = new SDLImage(mAsyncIOTask, filename);
    return retval;
}
bool SDLCanvas::similar(float a, float b, float tol) {
    a -= b;
    if (a < 0) a = -a;
    return a < tol;
}
SDLCanvas::SDLDisplayList::SDLDisplayList(SDLCanvas *canvas,
                                          const std::shared_ptr<Image> &image,
                                          const std::vector<BlitDescription> &draws)
    : image(image), dl(draws) {
}
void SDLCanvas::SDLDisplayList::attach(const std::shared_ptr<Image>&newImage) {
    image = newImage;
}
void SDLCanvas::SDLDisplayList::draw(Canvas *canvas, int x, int y) const {
    std::shared_ptr<Image> img (image.lock());
    if (img) {
        for (auto blit : dl) {
            static_cast<SDLCanvas*>(canvas)->drawSpriteSrc(
                img.get(), blit.src, blit.centerX + x, blit.centerY + y,
                blit.scaleX, blit.scaleY, blit.angle, 1.0);
        }
    }
}
SDLCanvas::SDLDisplayList::~SDLDisplayList(){
}

SDLCanvas * SDLCanvas::SDLImageCacheDisplayList::makeBlankDrawableSurface(int w,
                                                                          int h,
                                                                          SDLCanvas *parent) {
    unsigned char rmask[4] = {0xff,0x0,0x0,0x0};
    unsigned char gmask[4] = {0x0,0xff,0x0,0x0};
    unsigned char bmask[4] = {0x0,0x0,0xff,0x0};
    unsigned char amask[4] = {0x0,0x0,0x0,0xff};
    Uint32 mask[4] = {0, 0, 0, 0};
    memcpy(&mask[0], rmask, 4);
    memcpy(&mask[1], gmask, 4);
    memcpy(&mask[2], bmask, 4);
    memcpy(&mask[3], amask, 4);
#if SDL_MAJOR_VERSION >= 2
    SDLCanvas * retval = new SDLCanvas(SDL_CreateTexture(parent->context->renderer,
                                                         SDL_PIXELFORMAT_ABGR8888,
                                                         SDL_TEXTUREACCESS_TARGET,
                                                         w,
                                                         h),
                                       w,
                                       h,
                                       parent);
#else
    SDL_Surface *tmpImage = SDL_CreateRGBSurface(0, //FLAGS
                                                 w,
                                                 h,
                                                 32,
                                                 mask[0],
                                                 mask[1],
                                                 mask[2],
                                                 mask[3]);
    SDLCanvas * retval = new SDLCanvas(SDL_DisplayFormatAlpha(tmpImage), w, h, parent);
    SDL_FreeSurface(tmpImage);
#endif
    return retval;
}
SDLCanvas::SDLImageCacheDisplayList::SDLImageCacheDisplayList(SDLCanvas *canvas,
                                                              const std::shared_ptr<Image> &image,
                                                              const std::vector<BlitDescription> &lst,
                                                              const Rect &bound)
    : SDLDisplayList(canvas, image, lst),
     bounds(bound),
     cache(makeBlankDrawableSurface(bound.w, bound.h, canvas)) {
    loadImage();
}
SDLCanvas::SDLImageCacheDisplayList::~SDLImageCacheDisplayList(){
    cache.reset();
    std::cerr<<"Destructing icdl"<<std::endl;
}
void SDLCanvas::SDLImageCacheDisplayList::loadImage() {
    std::shared_ptr<Image> img(image.lock());
    // this lets us copy the alpha channel directly
    static_cast<SDLImage*>(img.get())->disableAlphaBlend();
    if (img) {
        for(const BlitDescription &blit : dl) {
            cache.get()->drawSpriteSrc(
                img.get(), blit.src,
                blit.centerX - bounds.x,
                blit.centerY - bounds.y,
                blit.scaleX, blit.scaleY, blit.angle, 1.0);
        }
    }
    static_cast<SDLImage*>(img.get())->enableAlphaBlend();
}
void SDLCanvas::SDLImageCacheDisplayList::reloadImage() {
#if SDL_MAJOR_VERSION >= 2
    SDL_SetRenderTarget(cache->context->renderer, cache->screen);
    SDL_RenderClear(cache->context->renderer);
    SDL_SetRenderTarget(cache->context->renderer, cache->screen);
#else
    SDL_Rect full;
    full.x = 0;
    full.y = 0;
    full.w = cache->width();
    full.h = cache->height();
    SDL_FillRect(cache->screen, &full, 0);
#endif
    loadImage();
}
void SDLCanvas::SDLImageCacheDisplayList::attach(const std::shared_ptr<Image> &newImage) {
    SDLDisplayList::attach(newImage);
    reloadImage();
}
void SDLCanvas::SDLImageCacheDisplayList::draw(Canvas *canvas, int x, int y) const {
    SDL_Rect src;
    SDL_Rect dest;
    src.x = 0;
    src.y = 0;
    src.w = cache->width();
    src.h = cache->height();
    dest.x = static_cast<int>(x + bounds.x);
    dest.y = static_cast<int>(y + bounds.y);
    dest.w = src.w;
    dest.h = src.h;
#if SDL_MAJOR_VERSION >= 2
    SDL_RenderCopy(static_cast<SDLCanvas*>(canvas)->context->renderer,
                   cache->screen, &src, &dest);
#else
    SDL_BlitSurface(cache->screen, &src, static_cast<SDLCanvas*>(canvas)->screen, &dest);
#endif
}

DisplayList *SDLCanvas::makeDisplayList(const std::shared_ptr<Image> &image,
                                        const std::vector<BlitDescription> &draws,
                                        const Rect&bounds) {
    if (false && bounds.w && bounds.h) {
        return new SDLImageCacheDisplayList(this, image, draws, bounds);
    } else {
        return new SDLDisplayList(this, image, draws);
    }
}
void SDLCanvas::attachDisplayList(DisplayList *dl, const std::shared_ptr<Image> &image) {
    dl->attach(image);
}
void SDLCanvas::drawDisplayList(const DisplayList *dl, int x, int y){
    dl->draw(this, x,y);
}

void SDLCanvas::drawSprite(Image *image,
                           float centerX, float centerY,
                           float scaleX, float scaleY,
                           float angle, float alpha /* from 0 - 1*/) {
    //SDLImage* sdl_image = static_cast<SDLImage*>(image);
    drawSpriteSrc(image, Rect(0, 0, image->width(), image->height()),
                  centerX, centerY, scaleX, scaleY, angle, alpha);
}

#if SDL_MAJOR_VERSION < 2
void SDLCanvas::setupAlphaBlit(SDL_Surface * surf, float alpha, SDL_Rect sdldest) {
#ifndef EMSCRIPTEN
    if (alpha != 1.0) {
        if (temp_buffer == nullptr) {
            temp_buffer = SDL_CreateRGBSurface(0, screen->w, screen->h, screen->format->BitsPerPixel,
                                               screen->format->Rmask, screen->format->Gmask, screen->format->Bmask,
                                               0);
        }
        SDL_BlitSurface(screen, &sdldest, temp_buffer, &sdldest);
    }
#else
    // EMSCRIPTEN disobeys the spec here and multiplies the per-pixel alpha with the constant per-surface alpha
    // We use this to save 3x in rendering time
    SDL_SetAlpha(surf, SDL_SRCALPHA, (Uint8)(255 * alpha));
#endif
}
void SDLCanvas::doAlphaBlit(SDL_Surface *, float alpha, SDL_Rect sdldest) {
#ifndef EMSCRIPTEN
    if (alpha != 1.0) {
        SDL_SetAlpha(temp_buffer, SDL_SRCALPHA, (Uint8)(255 - 255 * alpha));
        SDL_BlitSurface(temp_buffer, &sdldest, screen, &sdldest);
    }
#endif
}
#endif
void SDLCanvas::drawSpriteSrc(Image *image,
                              const Rect &src,
                              float centerX, float centerY,
                              float scaleX, float scaleY,
                              float angle, float alpha /* from 0 - 1 */) {
    if (!image->isLoaded()) {
        return;
    }
    SDLImage* sdl_image = static_cast<SDLImage*>(image);
    if (similar(angle, 0, 3.1415926536 / 180.0)) {
        angle = 0; // if something is almost straight up: just optimize
    }
#if SDL_MAJOR_VERSION >= 2
    if (sdl_image->surf) {
        sdl_image->texture = SDL_CreateTextureFromSurface(context->renderer, sdl_image->surf);
        SDL_FreeSurface(sdl_image->surf);
        sdl_image->surf = nullptr;
    }
    SDL_SetTextureAlphaMod(sdl_image->texture, (Uint8)(alpha * 255));
    if (!sdl_image->texture) {
        return;
    }
    if (screen) {
        SDL_SetRenderTarget(context->renderer, screen);
    }
    SDL_Rect sdlsrc(SDLCanvas::rectToSDL(src));
    SDL_Rect sdldest;
    sdldest.x = centerX - scaleX / 2;
    sdldest.y = centerY - scaleY / 2;
    sdldest.w = sdlsrc.w;
    sdldest.h = sdlsrc.h;
    if (angle == 0) {
        SDL_RenderCopy(context->renderer, sdl_image->texture, &sdlsrc, &sdldest);
    } else {
        SDL_RenderCopyEx(context->renderer, sdl_image->texture, &sdlsrc, &sdldest,
                         -angle * 180 / M_PI, NULL, SDL_FLIP_NONE);
    }
    if (screen) {
        SDL_SetRenderTarget(context->renderer, NULL);
    }
#else
    SDL_Surface * surf = sdl_image->surf;
    if (!surf) {
        return;
    }
    if (angle == 0 && scaleX == src.w && scaleY == src.h) {
        SDL_Rect sdlsrc (SDLCanvas::rectToSDL(src));
        SDL_Rect sdldest;
        sdldest.x = centerX - scaleX / 2;
        sdldest.y = centerY - scaleY / 2;
        sdldest.w = sdlsrc.w;
        sdldest.h = sdlsrc.h;
        setupAlphaBlit(surf, alpha, sdldest);
        SDL_BlitSurface(surf, &sdlsrc, screen, &sdldest);
        doAlphaBlit(surf, alpha, sdldest);
        return;
    }
    if (src.x != 0 || src.y != 0 || src.w != surf->w || src.h != surf->h) {
        SDL_Surface *tmpImage = SDL_CreateRGBSurface(0, //FLAGS
                                                     src.w,
                                                     src.h,
                                                     surf->format->BytesPerPixel * 8,
                                                     surf->format->Rmask,
                                                     surf->format->Gmask,
                                                     surf->format->Bmask,
                                                     surf->format->Amask);
        SDL_Rect sdlsrc, sdldest;
        sdlsrc.x = src.x;
        sdlsrc.y = src.y;
        sdlsrc.w = src.w;
        sdlsrc.h = src.h;
        sdldest.x = 0;
        sdldest.y = 0;
        sdldest.w = src.w;
        sdldest.h = src.h;
        SDL_BlitSurface(surf, &sdlsrc, tmpImage, &sdldest);
        surf = tmpImage;
    }
    float zoom = scaleX != surf->w ? scaleX / surf->w : 1.0;
    if ((angle == 0 && zoom != 1.0) || !SDLCanvas::similar(scaleX * surf->h, scaleY * surf->w,
                                                           0.25f * (surf->h + surf->w))) {
        SDL_Surface *tmpImage = nullptr;
        // std::cerr<< "Zoom " <<scaleX / surf->w<<","<< scaleY / surf->h << std::endl;
        tmpImage = zoomSurface(surf, scaleX / surf->w, scaleY / surf->h, true);
        if (surf != sdl_image->surf) {
            SDL_FreeSurface(surf); // this is if asymmetric zoom was necessary
        }
        surf = tmpImage;
        zoom = 1.0;
    }
    if (angle != 0 || zoom != 1.0) {
        // std::cerr<< "Zoom " <<zoom<<", rot "<< angle << std::endl;
        SDL_Surface * tmpImage = rotozoomSurface(surf, angle * (180./ 3.1415926536), zoom, 1);
        if (surf != sdl_image->surf) {
            SDL_FreeSurface(surf); // this is if asymmetric zoom was necessary
        }
        surf = tmpImage;
    }
    int surfw = surf->w;
    int surfh = surf->h;
    SDL_Rect sdlsrc;
    sdlsrc.x = 0;
    sdlsrc.y = 0;
    sdlsrc.w = surfw;
    sdlsrc.h = surfh;
    SDL_Rect sdldest;
    sdldest.x = centerX - surfw / 2;
    sdldest.y = centerY - surfh / 2;
    sdldest.w = surfw;
    sdldest.h = surfh;
    setupAlphaBlit(surf, alpha, sdldest);
    SDL_BlitSurface(surf, &sdlsrc, screen, &sdldest);
    doAlphaBlit(surf, alpha, sdldest);
    if (surf != sdl_image->surf) {
        SDL_FreeSurface(surf); // this is if asymmetric zoom was necessary
    }
#endif
}

void SDLCanvas::drawLine(int x0, int y0, int x1, int y1,
                         const SDL_Color& color,
                         float alpha) {
#if SDL_MAJOR_VERSION >= 2
    if (screen) {
        SDL_SetRenderTarget(context->renderer, screen);
    }
    SDL_SetRenderDrawColor(context->renderer, color.r, color.g, color.b, (Uint8)(255 * alpha));
    SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderDrawLine(context->renderer, x0, y0, x1, y1);
#else
    lineRGBA(screen, x0, y0, x1, y1, color.r, color.g, color.b, (Uint8)(255 * alpha));
#endif
}
void SDLCanvas::resize(int new_width, int new_height) {
#if SDL_MAJOR_VERSION < 2
    SDL_FreeSurface(screen);
    screen = SDL_SetVideoMode(
        new_width, new_height, 0,
        SDL_HWSURFACE | SDL_RESIZABLE );
#endif
}

void SDLCanvas::beginFrame() {
    mAsyncIOTask->callPendingCallbacksFromMainThread();
#if SDL_MAJOR_VERSION >= 2
    if (screen == nullptr) {
        SDL_GetWindowSize(context->window,
                          &w, &h);
    } else {
        int access; Uint32 format;
        SDL_QueryTexture(screen,
                         &format, &access, &w, &h);
        
    }
#else
    w = screen->w;
    h = screen->h;
#endif
}
void SDLCanvas::endFrame() {
#if SDL_MAJOR_VERSION >= 2
    if (screen == nullptr) {
        SDL_RenderPresent(context->renderer);
    }
#else
# ifndef EMSCRIPTEN
    SDL_Flip(screen);
# endif
#endif
    mAsyncIOTask->callPendingCallbacksFromMainThread();
}
bool SDLCanvas::getNextEvent(SDL_Event *out_event) {
    return 0 != SDL_PollEvent(out_event);
}
SDL_Event* SDLCanvas::makeBlankEventUnion()const {
    SDL_Event * retval = new SDL_Event();
    memset(retval, 0, sizeof(SDL_Event));
    return retval;
}
void SDLCanvas::destroyEventUnion(SDL_Event*e)const {
    delete e;
}

}

