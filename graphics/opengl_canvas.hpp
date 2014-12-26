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

    explicit OpenGLImage(const std::string &filename, NoLoad);
    explicit OpenGLImage(SDL_Surface *surf);
    explicit OpenGLImage(const std::string &filename);
    virtual ~OpenGLImage();

    void downloadAndLoad();
    void reload();
    static void reloadImage(const std::shared_ptr<Image> &image);
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
    std::vector<Image::BlitDescription> blits;
    std::shared_ptr<OpenGLImage> image;
    OpenGLCanvas *canvas;
    GLuint vbo;
    mutable bool uploaded;

    void uploadVertexArray() const;
public:
    OpenGLDisplayList(OpenGLCanvas *canvas,
            const std::vector<Image::BlitDescription> &blits);
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

    friend class OpenGLDisplayList;

public:
    OpenGLCanvas(int width, int height);
    ~OpenGLCanvas();

    void onContextLost();
    void reinitialize();

    virtual int width();
    virtual int height();

    virtual FontManager &fontManager() {
        return mFontManager;
    }

    OpenGLImage *loadImageFromSurface(SDL_Surface *surf);
    OpenGLImage *loadImage(const std::string &filename);

    virtual void drawSprite(Image *image,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                            float angle);

    virtual DisplayList *makeDisplayList(const std::shared_ptr<Image> &image,
                                         const std::vector<Image::BlitDescription> &draws,
                                         const Rect&bounds);

    virtual void attachDisplayList(DisplayList *dl, const std::shared_ptr<Image> &image);

    virtual void drawDisplayList(const DisplayList *dl, int x, int y);

    virtual void beginFrame();
    virtual void endFrame();

    virtual void clear();

    GLuint spriteVBO;
    GLint positionLocation;
    GLint texCoordLocation;
    GLint matrixLocation;
    GLint sampTextureLocation;
    GLuint sampTexture;
    int buf;
    GLuint program;
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
};

}

#endif
