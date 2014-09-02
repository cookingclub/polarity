#include "opengl_canvas.hpp"
#include "display_list.hpp"
#include "main/main.hpp"
#include "graphics/matrix4x4.hpp"

namespace Polarity {

OpenGLDisplayList::OpenGLDisplayList(
    const std::vector<Image::BlitDescription> &blits)
    : blits(blits), uploaded(false) {

    glGenBuffers(1, &vbo);
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
    int i = 0;
    for (const auto &blit : blits) {
        Rect texcoords = Rect(
            (float)blit.src.left() / image->width(),
            (float)blit.src.top() / image->height(),
            (float)blit.src.width() / image->width(),
            (float)blit.src.height() / image->height());
        Rect dst (
            blit.centerX - (float)blit.scaleX / 2,
            blit.centerY - (float)blit.scaleY / 2,
            blit.scaleX,
            blit.scaleY);
        GLfloat thisRect[6 * 5] = {
            dst.left(), dst.bottom(), 0, texcoords.left(), texcoords.bottom(),
            dst.right(), dst.bottom(), 0,  texcoords.right(), texcoords.bottom(),
            dst.left(), dst.top(), 0, texcoords.left(), texcoords.top(),
            dst.right(), dst.top(), 0, texcoords.right(), texcoords.top(),
            dst.left(), dst.top(), 0, texcoords.left(), texcoords.top(),
            dst.right(), dst.bottom(), 0, texcoords.right(), texcoords.bottom()
        };
        std::copy(thisRect, thisRect + 6 * 5, data + i);
        i += 6 * 5;
    }
/* = ;*/
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

OpenGLImage::OpenGLImage(const std::string &filename)
    : Image(filename), tex(0), w(0), h(0) {
    if (!filename.empty()) {
        downloadAndLoad();
    }
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
            int nOfColors = 0;
            int texture_format = 0;
            switch (image->format) {
            case DecodedImage::L:
                nOfColors = 1;
                texture_format = GL_LUMINANCE;
                break;
            case DecodedImage::LA:
                nOfColors = 2;
                texture_format = GL_LUMINANCE_ALPHA;
                break;
            case DecodedImage::RGB:
                nOfColors = 3;
                texture_format = GL_RGB;
                break;
            case DecodedImage::RGBA:
                nOfColors = 4;
                texture_format = GL_RGBA;
                break;
            }
            // get the number of channels in the SDL surface
            GLuint texture = 0;
            // Have OpenGL generate a texture object handle for us
            glGenTextures( 1, &texture );

            // Bind the texture object
            glBindTexture( GL_TEXTURE_2D, texture );

            // Set the texture's stretching properties
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

            // Edit the texture object's image data using the information SDL_Surface gives us
            glTexImage2D( GL_TEXTURE_2D, 0, texture_format, image->width, image->height, 0,
                          texture_format, GL_UNSIGNED_BYTE, &image->data[0]);
            thus->tex = texture;
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
    GLuint shaders[2];
    GLsizei count = 0;
    glGetAttachedShaders(program, 2, &count, shaders);
    glDeleteProgram(program);
    for (GLsizei i = 0; i < count; i++) {
        glDeleteShader(shaders[i]);
    }
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
uniform sampler2D samp_texture;\n\
\n\
void main() {\n\
    gl_FragColor = texture2D(samp_texture, v_texcoord);\n\
    /*gl_FragColor = vec4(abs(v_texcoord), 0.1, 1.0);*/\n\
/* + 15.0*texture2D(samp_texture, v_texcoord) + vec4(0,0,0,1);*/\n\
}\n\
";
    GLuint vertexShader = compileShader(VERTEX_SHADER, "vertex",
                                        GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        return;
    }
    GLuint fragmentShader = compileShader(FRAGMENT_SHADER, "fragment",
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
    sampTextureLocation = glGetUniformLocation(program, "samp_texture");

    glGenBuffers(1, &spriteVBO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);

    createSpriteRectArray();

    sampTexture = 0;
    glUniform1i(sampTextureLocation, 0);
}

OpenGLCanvas::OpenGLCanvas(int width, int height) {
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    screen = SDL_SetVideoMode(
        width, height, 0,
        SDL_HWSURFACE | SDL_RESIZABLE | SDL_OPENGL );
    createRectProgram();
    glViewport(0, 0, width, height);
}

int OpenGLCanvas::width() {
    return screen->w;
}

int OpenGLCanvas::height() {
    return screen->h;
}

OpenGLImage *OpenGLCanvas::loadImage(const std::string &filename) {
    OpenGLImage *retval = new OpenGLImage(filename);
    return retval;
}

DisplayList *OpenGLCanvas::makeDisplayList(const std::shared_ptr<Image> &image,
                                           const std::vector<Image::BlitDescription> &draws,
                                           const Rect&bounds) {
    OpenGLDisplayList *list = new OpenGLDisplayList(draws);
    list->attach(image);
    return list;
}

void OpenGLCanvas::attachDisplayList(DisplayList *dl, const std::shared_ptr<Image> &image) {
    dl->attach(image);
}

void OpenGLCanvas::drawDisplayList(const DisplayList *dl, int x, int y) {
    dl->draw(this, x, y);
}

void OpenGLCanvas::drawSprite(Image *image,
                              float centerX, float centerY,
                              float scaleX, float scaleY,
                              float angle) {
    if (!image->isLoaded()) {
        return;
    }
    OpenGLImage* ogl_image = static_cast<OpenGLImage*>(image);

    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, ogl_image->texture());
    glActiveTexture(GL_TEXTURE0 + sampTexture);

    GLMatrix4x4 screen_coords_adjustment = {
        2.f / (GLfloat)width(), 0, 0, 0,
        0, -2.f / (GLfloat)height(), 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1
    };

    GLMatrix4x4 mat = screen_coords_adjustment;
    mat *= GLMatrix4x4::translation(centerX, centerY, 0);
    mat *= GLMatrix4x4::rotationZ(angle);
    mat *= GLMatrix4x4::scalar(scaleX, scaleY, 1);

    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, mat.values());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    GLfloat *offset = 0;
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset));
    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(offset + 3));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void OpenGLCanvas::clear() {
    glFlush();
    //glClearColor(0.1,0.1,0.1,1);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    glFlush();
}

void OpenGLCanvas::swapBuffers() {
    glFlush();
    SDL_GL_SwapBuffers();
}

}
