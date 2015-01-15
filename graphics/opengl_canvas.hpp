#ifndef POLARITY_GRAPHICS_OPENGL_CANVAS_HPP__
#define POLARITY_GRAPHICS_OPENGL_CANVAS_HPP__

#ifdef USE_GLES
#include "GLES2/gl2.h"
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#ifdef USE_SDL2
#include "SDL2/SDL_version.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
#else
#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#endif
#include "display_list.hpp"
#include "image.hpp"
#include "canvas.hpp"
#include "font_manager.hpp"

#include <set>

namespace Polarity {

class OpenGLCanvas;

class OpenGLImage : public Image {
    friend class OpenGLCanvas;
public:
    class NoLoad {};

    OpenGLImage(const std::string &filename, NoLoad);
    explicit OpenGLImage(SDL_Surface *surf);
    OpenGLImage(const std::shared_ptr<AsyncIOTask>& asyncIOTask, const std::string &filename);
    virtual ~OpenGLImage();

    void downloadAndLoad(const std::shared_ptr<AsyncIOTask>&asyncIOTask);
    void reload(const std::shared_ptr<AsyncIOTask>&asyncIOTask);
    static void reloadImage(const std::shared_ptr<AsyncIOTask>&asyncIOTask, const std::shared_ptr<Image> &image);
    static void loadImageOpenGL(Image *super, const std::string &filename,
                         const std::shared_ptr<DecodedImage> &image);

    virtual int width() { return w; }
    virtual int height() { return h; }

    GLuint texture() { return tex; }

    virtual void enableAlphaBlend() {}
    virtual void disableAlphaBlend() {}

private:
    GLuint tex;
    int w;
    int h;
};

class OpenGLDisplayList : public DisplayList {
    std::vector<BlitDescription> blits;
    std::shared_ptr<OpenGLImage> image;
    OpenGLCanvas *canvas;
    GLuint vbo;
    mutable bool uploaded;

    void uploadVertexArray() const;
public:
    OpenGLDisplayList(OpenGLCanvas *canvas,
            const std::vector<BlitDescription> &blits);
    ~OpenGLDisplayList();

    void reinitialize();
    virtual void draw(Canvas *canvas, int x, int y) const;
    virtual void attach(const std::shared_ptr<Image> &newImage);
};

class OpenGLCanvas : public Canvas {

    void createSpriteRectArray();

    GLuint compileShader(const GLchar *src, const char *name, int type);
   
    void deleteProgram(GLuint program);

    void createRectProgram();

    void createWhiteTexture();

    friend class OpenGLDisplayList;

public:
    OpenGLCanvas(const std::shared_ptr<AsyncIOTask> &asyncIOTask, int width, int height);
    ~OpenGLCanvas();

    void onContextLost();
    void reinitialize();

    virtual int width();
    virtual int height();

    virtual FontManager &fontManager() {
        return mFontManager;
    }
    virtual const std::shared_ptr<AsyncIOTask> &asyncIOTask() const {
        return mAsyncIOTask;
    }

    OpenGLImage *loadImageFromSurface(SDL_Surface *surf);
    OpenGLImage *loadImage(const std::string &filename);

    virtual void drawSprite(Image *image,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                            float angle, float alpha);
    virtual void drawLine(int x0, int y0, int x1, int y1,
                          const SDL_Color& color, float alpha);
    virtual DisplayList *makeDisplayList(const std::shared_ptr<Image> &image,
                                         const std::vector<BlitDescription> &draws,
                                         const Rect&bounds);

    virtual void attachDisplayList(DisplayList *dl, const std::shared_ptr<Image> &image);

    virtual void drawDisplayList(const DisplayList *dl, int x, int y);

    virtual void resize(int w, int h);
    virtual void beginFrame();
    virtual void endFrame();
    virtual bool getNextEvent(SDL_Event* out_event);
    virtual void clear();

    GLuint spriteVBO;
    GLint positionLocation;
    GLint texCoordLocation;
    GLint matrixLocation;
    GLint colorLocation;
    GLint sampTextureLocation;
    GLuint sampTexture;
    int buf;
    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint whiteTexture;
    int w;
    int h;
    bool lostContext;
#if SDL_MAJOR_VERSION >= 2
    SDL_Window *window;
    SDL_GLContext renderer;
#else
    SDL_Surface* screen;
    void *dummy;
#endif
    std::set<OpenGLDisplayList*> displayLists;

    FontManager mFontManager;
    std::shared_ptr<AsyncIOTask> mAsyncIOTask;
};

}

#endif
