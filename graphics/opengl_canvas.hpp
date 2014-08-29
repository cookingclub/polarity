#ifndef POLARITY_GRAPHICS_OPENGL_CANVAS_HPP__
#define POLARITY_GRAPHICS_OPENGL_CANVAS_HPP__

#include "GLES2/gl2.h"

#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "display_list.hpp"
#include "image.hpp"
#include "canvas.hpp"

namespace Polarity {

class OpenGLDisplayList : public DisplayList {
    std::vector<Image::BlitDescription> blits;
    std::shared_ptr<Image> image;
public:
    OpenGLDisplayList(const std::vector<Image::BlitDescription> &blits);
    virtual void draw(Canvas *canvas, int x, int y) const;
    virtual void attach(const std::shared_ptr<Image> &newImage);
};

class OpenGLImage : public Image {
    friend class OpenGLCanvas;
public:
    explicit OpenGLImage(const std::string &filename);
    virtual ~OpenGLImage();

    void downloadAndLoad();
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

class OpenGLCanvas : public Canvas {

    void createRectArray(OpenGLImage *img, const Rect &src, const Rect &dst);

    GLuint compileShader(const GLchar *src, const char *name, int type);
   
    void deleteProgram(GLuint program);

    void createRectProgram();

    void drawSpriteSrc(Image *image, const Rect &src,
                            float centerX, float centerY,
                            float scaleX, float scaleY,
                            float angle);

    friend class OpenGLDisplayList;

public:
    OpenGLCanvas(int width, int height);

    virtual int width();
    virtual int height();

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

    virtual void swapBuffers();

    virtual void clear();

    GLuint vbo;
    GLint positionLocation;
    GLint texCoordLocation;
    GLint matrixLocation;
    GLint sampTextureLocation;
    GLuint sampTexture;
    int buf;
    GLuint program;
    SDL_Surface* screen;
};

}

#endif
