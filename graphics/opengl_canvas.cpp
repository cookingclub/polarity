#include <set>
#include "opengl_canvas.hpp"
#include "display_list.hpp"
#include "main/main.hpp"
#include "graphics/matrix4x4.hpp"


#ifdef EMSCRIPTEN

namespace {
    std::set<Polarity::OpenGLCanvas*> allCanvases;
}

extern "C"
void onContextLost() {
    std::cerr << "Context lost" << std::endl;
    for (Polarity::OpenGLCanvas *canvas : allCanvases) {
        canvas->onContextLost();
    }
}

extern "C"
void onContextRestored() {
    std::cerr << "Context restored" << std::endl;
    for (Polarity::OpenGLCanvas *canvas : allCanvases) {
        canvas->reinitialize();
    }
}
#endif

namespace Polarity {
OpenGLDisplayList::OpenGLDisplayList(
        OpenGLCanvas *canvas,
        const std::vector<Image::BlitDescription> &blits)
    : blits(blits), canvas(canvas), vbo(-1), uploaded(false) {
    reinitialize();
    canvas->displayLists.insert(this);
}

OpenGLDisplayList::~OpenGLDisplayList() {
    auto it = canvas->displayLists.find(this);
    canvas->displayLists.erase(it);
}

void OpenGLDisplayList::reinitialize() {
    glGenBuffers(1, &vbo);
    uploaded = false;
}

void OpenGLDisplayList::uploadVertexArray() const {
    if (!image->isLoaded()) {
        std::cerr << "not loaded: " << image->sourceUrl() << std::endl;
        return;
    }
    if (image->width() <= 0 || image->height() <= 0) {
        std::cerr << "invalid width! " << image->sourceUrl() << std::endl;
        return;
    }
    GLfloat data[blits.size() * 6 * 5];
    GLfloat *thisTri = data;
    for (const auto &blit : blits) {
        Rect texcoords = Rect(
            (float)blit.src.left() / image->width(),
            (float)blit.src.top() / image->height(),
            (float)blit.src.width() / image->width(),
            (float)blit.src.height() / image->height());

        GLMatrix4x4 mat = GLMatrix4x4::translation(blit.centerX, blit.centerY, 0);
        mat *= GLMatrix4x4::rotationZ(blit.angle);
        mat *= GLMatrix4x4::scalar(0.5 * blit.scaleX, 0.5 * blit.scaleY, 1);

        Rect dst (
            -0.5,
            -0.5,
            1,
            1);
        GLVec4 vecs[6] = {
            mat * GLVec4(-1, 1, 0, 1),
            mat * GLVec4(1, 1, 0, 1),
            mat * GLVec4(-1, -1, 0, 1),
            mat * GLVec4(1, -1, 0, 1),
            mat * GLVec4(-1, -1, 0, 1),
            mat * GLVec4(1, 1, 0, 1)
        };
        GLfloat uv[12] = {
            texcoords.left(), texcoords.bottom(),
            texcoords.right(), texcoords.bottom(),
            texcoords.left(), texcoords.top(),
            texcoords.right(), texcoords.top(),
            texcoords.left(), texcoords.top(),
            texcoords.right(), texcoords.bottom()
        };
        for (int j = 0; j < 6; j++) {
            std::copy(vecs[j].values(), vecs[j].values() + 3, thisTri);
            thisTri[3] = uv[j * 2];
            thisTri[4] = uv[j * 2 + 1];
            thisTri += 5;
        };
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    uploaded = true;
}

void OpenGLDisplayList::draw(Canvas *canvas, int x, int y) const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (!uploaded) {
        uploadVertexArray();
    }
    if (!uploaded) {
        return;
    }
    OpenGLCanvas *ogl_canvas = static_cast<OpenGLCanvas*> (canvas);
    glUseProgram(ogl_canvas->program);
    glBindTexture(GL_TEXTURE_2D, image->texture());
    glActiveTexture(GL_TEXTURE0 + ogl_canvas->sampTexture);

    GLMatrix4x4 screen_coords_adjustment = {
        2.f / (GLfloat)ogl_canvas->width(), 0, 0, 0,
        0, -2.f / (GLfloat)ogl_canvas->height(), 0, 0,
        0, 0, 1, 0,
        -1 + 2 * x / (GLfloat)ogl_canvas->width(), 1 - 2 * y / (GLfloat)ogl_canvas->height(), 0, 1
    };

    GLMatrix4x4 mat = screen_coords_adjustment;
    //mat *= GLMatrix4x4::translation(centerX, centerY, 0);
    //mat *= GLMatrix4x4::rotationZ(angle);
    //mat *= GLMatrix4x4::scalar(scaleX, scaleY, 1);

    glUniformMatrix4fv(ogl_canvas->matrixLocation, 1, GL_FALSE, mat.values());
    float color[4] = {1,1,1,1};
    glUniform4fv(ogl_canvas->colorLocation, 1, color);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    GLfloat *offset = 0;
    glVertexAttribPointer(ogl_canvas->positionLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset));
    glVertexAttribPointer(ogl_canvas->texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset + 3));
    glEnableVertexAttribArray(ogl_canvas->positionLocation);
    glEnableVertexAttribArray(ogl_canvas->texCoordLocation);
    glDrawArrays(GL_TRIANGLES, 0, blits.size() * 6);
}

void OpenGLDisplayList::attach(const std::shared_ptr<Image> &newImage) {
    image = std::static_pointer_cast<OpenGLImage>(newImage);
    uploaded = false;
}

static GLuint genTexture(bool use_nearest=false) {
    GLuint texture = 0;
    // Have OpenGL generate a texture object handle for us
    glGenTextures( 1, &texture );

    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, texture );

    // Set the texture's stretching properties
    if (use_nearest) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    } else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    return texture;
}

OpenGLImage::OpenGLImage(const std::string &filename, NoLoad)
    : Image(filename), tex(0), w(0), h(0) {
}

OpenGLImage::OpenGLImage(const std::string &filename)
    : Image(filename), tex(genTexture()), w(0), h(0) {
    if (!filename.empty()) {
        downloadAndLoad();
    }
}

OpenGLImage::OpenGLImage(SDL_Surface *surface)
    : Image(std::string()), tex(genTexture(true)), w(surface->w), h(surface->h) {
    int texture_format;
    bool colorNoAlpha = (surface->format->BitsPerPixel == 24 && surface->format->BytesPerPixel==3);
    bool colorAlpha = (surface->format->BitsPerPixel == 32 && surface->format->BytesPerPixel==4);
    uint32_t v255shift24 = 255;
    v255shift24 <<= 24;
    if (colorNoAlpha && surface->format->Rmask == 255 && surface->format->Gmask == (255 << 8) && surface->format->Bmask == (255 << 16)) {
        texture_format = GL_RGB;
#ifndef USE_GLES
    } else if (colorNoAlpha && surface->format->Bmask == 255 && surface->format->Gmask == (255 << 8) && surface->format->Rmask == (255 << 16)) {
        texture_format = GL_BGR;
    } else if (colorAlpha && surface->format->Bmask == 255 && surface->format->Gmask == (255 << 8) && surface->format->Rmask == (255 << 16)  && surface->format->Amask == v255shift24) {
        texture_format = GL_BGRA;
#endif
    } else if (colorAlpha && surface->format->Rmask == 255 && surface->format->Gmask == (255 << 8) && surface->format->Bmask == (255 << 16)  && surface->format->Amask == v255shift24) {
        texture_format = GL_RGBA;
    } else {
#ifdef USE_SDL2
        SDL_Surface *newSurf = SDL_ConvertSurfaceFormat(
                surface, SDL_PIXELFORMAT_ABGR8888, 0);
#else
        SDL_Surface *newSurf = SDL_CreateRGBSurface(
                SDL_SWSURFACE, surface->w, surface->h, 32,
                255, (255 << 8), (255 << 16), v255shift24);
        SDL_Rect r;
        r.x = 0;
        r.y = 0;
        r.w = surface->w;
        r.h = surface->h;
        if ( (surface->flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
            surface->flags &= ~SDL_SRCALPHA;
        }
        SDL_LowerBlit(surface, &r, newSurf, &r);
#endif
        SDL_FreeSurface(surface);
        surface = newSurf;
        texture_format = GL_RGBA;
        colorNoAlpha = false;
        colorAlpha = true;
    }

    if (!SDL_LockSurface(surface)) {
        glTexImage2D( GL_TEXTURE_2D, 0, colorAlpha ? GL_RGBA : GL_RGB, w, h, 0,
                      texture_format, GL_UNSIGNED_BYTE, surface->pixels);
        stage = COMPLETE;
#ifndef EMSCRIPTEN
        SDL_UnlockSurface(surface);
#endif
    }
    SDL_FreeSurface(surface);
}

OpenGLImage::~OpenGLImage() {
    if (tex != 0) {
        glDeleteTextures(1, &tex);
    }
}

void OpenGLImage::loadImageOpenGL(Image *super,
                             const std::string &filename,
                             const std::shared_ptr<DecodedImage> &image) {
    std::shared_ptr<Image>superShared = Image::getValidImage(filename);
    if (superShared.get() == super) {
        OpenGLImage *thus = static_cast<OpenGLImage*>(superShared.get());
        if (image->width == 0 || image->height == 0) {
            std::cerr << "Failed to load image: " << filename << std::endl;
            thus->stage = FAILED;
        } else {
            int texture_format = 0;
            switch (image->format) {
            case DecodedImage::L:
                texture_format = GL_LUMINANCE;
                break;
            case DecodedImage::LA:
                texture_format = GL_LUMINANCE_ALPHA;
                break;
            case DecodedImage::RGB:
                texture_format = GL_RGB;
                break;
            case DecodedImage::RGBA:
                texture_format = GL_RGBA;
                break;
            }
            glBindTexture( GL_TEXTURE_2D, thus->tex );

            // Edit the texture object's image data using the information SDL_Surface gives us
            glTexImage2D( GL_TEXTURE_2D, 0, texture_format, image->width, image->height, 0,
                          texture_format, GL_UNSIGNED_BYTE, &image->data[0]);
            thus->w = image->width;
            thus->h = image->height;
            thus->stage = COMPLETE;
        }
    }
}

void OpenGLImage::downloadAndLoad() {
    Image * super = this;
    Polarity::asyncFileLoad(filename,
                            std::bind(Image::parseAndLoad,
                                      super,
                                      filename,
                                      &OpenGLImage::loadImageOpenGL,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
}

void OpenGLImage::reload() {
    stage = LOADING;
    tex = genTexture();
    downloadAndLoad();
}

void OpenGLImage::reloadImage(const std::shared_ptr<Image> &image) {
    static_cast<OpenGLImage*>(image.get())->reload();
}

void OpenGLCanvas::createSpriteRectArray() {
    Rect texcoords = Rect(0, 0, 1, 1);
    Rect dst = Rect(-0.5, -0.5, 1, 1);
    const GLfloat data[] = {
        dst.left(), dst.bottom(), 0, texcoords.left(), texcoords.bottom(),
        dst.right(), dst.bottom(), 0, texcoords.right(), texcoords.bottom(),
        dst.left(), dst.top(), 0, texcoords.left(), texcoords.top(),
        dst.right(), dst.top(), 0, texcoords.right(), texcoords.top(),
        dst.left(), dst.top(), 0, texcoords.left(), texcoords.top(),
        dst.right(), dst.bottom(), 0, texcoords.right(), texcoords.bottom()
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    GLfloat *offset = 0;
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset));
    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset + 3));
    glEnableVertexAttribArray(positionLocation);
    glEnableVertexAttribArray(texCoordLocation);
}


GLuint OpenGLCanvas::compileShader(const GLchar *src, const char *name, int type) {
    GLuint shader = glCreateShader(type);

    GLint shaderlen = strlen(src);
    glShaderSource(shader, 1, &src, &shaderlen);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    GLint len = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len) {
        GLchar log[len + 1];
        glGetShaderInfoLog(shader, len, &len, log);
        std::cerr << name << " shader produced errors:\n" << log << std::endl;
    }
    if (success == GL_FALSE) {
        std::cerr << "Failed to compile " << name << " shader!" << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}
   
void OpenGLCanvas::deleteProgram(GLuint program) {
    glDeleteProgram(program);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}

void OpenGLCanvas::createRectProgram() {

    constexpr const GLchar VERTEX_SHADER[] = "\n\
precision mediump float;\n\
attribute vec3 a_position;\n\
attribute vec2 a_texcoord;\n\
varying vec2 v_texcoord;\n\
uniform mat4 u_matrix;\n\
\n\
void main() {\n\
    v_texcoord = a_texcoord;\n\
    gl_Position = u_matrix * vec4(a_position, 1.);\n\
}\n\
";

    constexpr const GLchar FRAGMENT_SHADER[] = "\n\
precision mediump float;\n\
varying vec2 v_texcoord;\n\
uniform vec4 u_color;\n\
uniform sampler2D samp_texture;\n\
\n\
void main() {\n\
    gl_FragColor = u_color * texture2D(samp_texture, v_texcoord);\n\
    /*gl_FragColor = vec4(abs(v_texcoord), 0.1, 1.0);*/\n\
/* + 15.0*texture2D(samp_texture, v_texcoord) + vec4(0,0,0,1);*/\n\
}\n\
";
    vertexShader = compileShader(VERTEX_SHADER, "vertex",
                                        GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        return;
    }
    fragmentShader = compileShader(FRAGMENT_SHADER, "fragment",
                                          GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    positionLocation = glGetAttribLocation(program, "a_position");
    texCoordLocation = glGetAttribLocation(program, "a_texcoord");
    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    GLint len = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if (len) {
        GLchar log[len + 1];
        glGetProgramInfoLog(program, len, &len, log);
        std::cerr << "program produced errors:\n" << log << std::endl;
    }
    if (success == GL_FALSE) {
        std::cerr << "Failed to link program!" << std::endl;
        deleteProgram(program);
        program = 0;
        return;
    }
    glUseProgram(program);
    matrixLocation = glGetUniformLocation(program, "u_matrix");
    colorLocation = glGetUniformLocation(program, "u_color");
    sampTextureLocation = glGetUniformLocation(program, "samp_texture");

    glGenBuffers(1, &spriteVBO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);

    createSpriteRectArray();

    sampTexture = 0;
    glUniform1i(sampTextureLocation, 0);
}

void OpenGLCanvas::createWhiteTexture() {
    whiteTexture = genTexture(true);
    uint8_t pixel[4] = {255, 255, 255, 255};
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, pixel);
}

OpenGLCanvas::OpenGLCanvas(int width, int height)
        : w(width), h(height), mFontManager(FONT_CACHE_SIZE, TEXT_CACHE_SIZE) {
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
#if SDL_MAJOR_VERSION >= 2
    window = SDL_CreateWindow("Polarity",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              width, height,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    renderer = SDL_GL_CreateContext(window);
    SDL_GL_GetDrawableSize(window, &w, &h);
#else
    screen = SDL_SetVideoMode(
        width, height, 0,
        SDL_HWSURFACE | SDL_RESIZABLE | SDL_OPENGL );
#endif
    lostContext = false;
    reinitialize();
#ifdef EMSCRIPTEN
    allCanvases.insert(this);
#endif
}

OpenGLCanvas::~OpenGLCanvas() {
#ifdef EMSCRIPTEN
    auto it = allCanvases.find(this);
    if (it != allCanvases.end()) {
        allCanvases.erase(it);
    }
#endif
#if SDL_MAJOR_VERISON >= 2
    SDL_GL_DeleteContext(context);
#endif
}

void OpenGLCanvas::onContextLost() {
    lostContext = true;
    fontManager().clearTextCache();
}

void OpenGLCanvas::reinitialize() {
    lostContext = false;
    createRectProgram();
    createWhiteTexture();
    glViewport(0, 0, width(), height());
    Image::forEachImage(OpenGLImage::reloadImage);
    for (OpenGLDisplayList *dl : displayLists) {
        dl->reinitialize();
    }
    fontManager().clearTextCache();
}

int OpenGLCanvas::width() {
    return w;
}

int OpenGLCanvas::height() {
    return h;
}

OpenGLImage *OpenGLCanvas::loadImageFromSurface(SDL_Surface *surf) {
    if (lostContext) {
        return new OpenGLImage(std::string(), OpenGLImage::NoLoad());
    }
    OpenGLImage *retval = new OpenGLImage(surf);
    return retval;
}

OpenGLImage *OpenGLCanvas::loadImage(const std::string &filename) {
    if (lostContext) {
        return new OpenGLImage(filename, OpenGLImage::NoLoad());
    }
    OpenGLImage *retval = new OpenGLImage(filename);
    return retval;
}

DisplayList *OpenGLCanvas::makeDisplayList(const std::shared_ptr<Image> &image,
                                           const std::vector<Image::BlitDescription> &draws,
                                           const Rect&bounds) {
    OpenGLDisplayList *list = new OpenGLDisplayList(this, draws);
    list->attach(image);
    return list;
}

void OpenGLCanvas::attachDisplayList(DisplayList *dl, const std::shared_ptr<Image> &image) {
    dl->attach(image);
}

void OpenGLCanvas::drawDisplayList(const DisplayList *dl, int x, int y) {
    dl->draw(this, x, y);
}

namespace {

GLMatrix4x4 getScreenCoordAdjustment(int width, int height) {
    GLMatrix4x4 screen_coords_adjustment = {
        2.f / (GLfloat)width, 0, 0, 0,
        0, -2.f / (GLfloat)height, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1
    };
    return screen_coords_adjustment;
}

}

void OpenGLCanvas::drawSprite(Image *image,
                              float centerX, float centerY,
                              float scaleX, float scaleY,
                              float angle, float alpha) {
    if (!image->isLoaded()) {
        return;
    }
    OpenGLImage* ogl_image = static_cast<OpenGLImage*>(image);

    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, ogl_image->texture());
    glActiveTexture(GL_TEXTURE0 + sampTexture);


    GLMatrix4x4 mat = getScreenCoordAdjustment(width(), height());
    mat *= GLMatrix4x4::translation(centerX, centerY, 0);
    mat *= GLMatrix4x4::rotationZ(angle);
    mat *= GLMatrix4x4::scalar(scaleX, scaleY, 1);
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, mat.values());
    float color[4] = {1, 1, 1, alpha};
    glUniform4fv(colorLocation, 1, color);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    GLfloat *offset = 0;
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset));
    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset + 3));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void OpenGLCanvas::drawLine(int x0, int y0, int x1, int y1,
                         const SDL_Color& color,
                         float alpha) {
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glActiveTexture(GL_TEXTURE0 + sampTexture);
    GLMatrix4x4 mat = getScreenCoordAdjustment(width(), height());
    mat *= GLMatrix4x4::translation((x0 + x1) / 2.0f, (y0 + y1) / 2.0f, 0);
    mat *= GLMatrix4x4::scalar(x1 - x0, y1 - y0, 1);
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, mat.values());
    float c[4] = {color.r/ 255.0f, color.g / 255.0f, color.b / 255.0f, alpha};
    glUniform4fv(colorLocation, 1, c);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    GLfloat *offset = 0;
    // skip the next vertex by saying pixel size is 10!!
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset + 5));
    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset + 8));
    glDrawArrays(GL_LINES, 0, 2);
}


void OpenGLCanvas::clear() {
    glFlush();
    //glClearColor(0.1,0.1,0.1,1);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    glFlush();
}

void OpenGLCanvas::resize(int new_width, int new_height) {
#if SDL_MAJOR_VERSION < 2
    SDL_FreeSurface(screen);
    screen = SDL_SetVideoMode(
        new_width, new_height, 0,
        SDL_HWSURFACE | SDL_RESIZABLE | SDL_OPENGL );
#endif
}

void OpenGLCanvas::beginFrame() {
#if SDL_MAJOR_VERSION >= 2
    int screenw = 0, screenh =0;
    SDL_GL_GetDrawableSize(window, &screenw, &screenh);
    if (w != screenw || h != screenh) {
        w = screenw;
        h = screenh;
        glViewport(0, 0, w, h);
    }
#else
    if (w != screen->w || h != screen->h) {
        w = screen->w;
        h = screen->h;
        glViewport(0, 0, w, h);
    }
#endif
}
void OpenGLCanvas::endFrame() {
    glFlush();
#if SDL_MAJOR_VERSION >= 2
    SDL_GL_SwapWindow(window);
#else
    SDL_GL_SwapBuffers();
#endif
}

}

