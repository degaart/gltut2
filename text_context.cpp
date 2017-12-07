#include "context.h"
#include <utility>

#include <cmath>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include "shader.h"
#include "image.h"
#include "system.h"
#include "stb_truetype.h"
#include "text_context.h"

class text_context_impl : public context {
    private:
        unsigned int text_vao;
        std::shared_ptr<Shader> text_program;
        unsigned int text_tex;
        std::vector<stbtt_bakedchar> charData;
        unsigned int text_vbo;
        float font_size;
    public:
        text_context_impl();
        void draw(float ticks);
        void drawText(const std::string& text, float x, float y);
};

#define TEX_SIZE   128

text_context_impl::text_context_impl()
{
    glGenVertexArrays(1, &this->text_vao);
    glBindVertexArray(this->text_vao);

    this->text_program = std::make_shared<Shader>(fmt::format("{}/text_shader.vs", resDir),
                                                 fmt::format("{}/text_shader.fs", resDir));
    text_program->use();
    text_program->setInt("texture1", 0);

    static const float text_vertices[] = {
        -1.0f,  1.0f,   0.0f,   0.0f,   1.0f,
        1.0f,   -1.0f,  0.0f,   1.0f,   0.0f,
        -1.0f,  -1.0f,  0.0f,   0.0f,   0.0f,

        -1.0f,  1.0f,   0.0f,   0.0f,   1.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,
        1.0f,   -1.0f,  0.0f,   1.0f,   0.0f,
    };
    glGenBuffers(1, &this->text_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->text_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(text_vertices), text_vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          BUFFER_OBJECT(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          BUFFER_OBJECT(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    auto filename = fmt::format("{}/dos.ttf", resDir);
    auto fontData = sys::readfile(filename);
    std::vector<unsigned char> pixels(TEX_SIZE * TEX_SIZE);

    font_size = 16;
    this->charData = std::vector<stbtt_bakedchar>(96); // 127 - ' ' + 1 = 96
    int ret = stbtt_BakeFontBitmap(fontData.data(), 0,
                                   font_size,
                                   pixels.data(), TEX_SIZE, TEX_SIZE,
                                   ' ', 96,
                                   charData.data());

    glGenTextures(1, &this->text_tex);
    glBindTexture(GL_TEXTURE_2D, this->text_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,                     /* mipmap level */
                 GL_RED,               /* format to store into */
                 TEX_SIZE, TEX_SIZE,
                 0,                     /* unused legacy stuff */
                 GL_RED,                /* input format */
                 GL_UNSIGNED_BYTE,      /* input datatype */
                 pixels.data());

    //glDisable(GL_CULL_FACE);
    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LEQUAL);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

struct Point {
    float x, y;
};

void text_context_impl::draw(float ticks)
{
    drawText("gltut", 0, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void text_context_impl::drawText(const std::string& text, float x, float y)
{
    this->text_program->use();
    glBindVertexArray(this->text_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->text_tex);
    this->text_program->setInt("texture1", 0);
    glBindBuffer(GL_ARRAY_BUFFER, this->text_vbo);

    Point cursor;
    cursor.x = 0;
    cursor.y = y + font_size;

    Point offset;
    offset.x = 0;
    offset.y = 0;

    Point scale;
    scale.x = 0.0125f;
    scale.y = 0.0125f;
    for(const unsigned char* c = (const unsigned char*)text.c_str(); *c; c++) {
        if(*c >= ' ' && *c <= 128) {
            stbtt_aligned_quad quad;
            stbtt_GetBakedQuad(this->charData.data(),
                               TEX_SIZE, TEX_SIZE,
                               *c - ' ',
                               &cursor.x, &cursor.y,
                               &quad,
                               1);

            /*
             * Coordinates transformation (linear interpolation)
             *  x = ((2.0 * char_x) / screenWidth) - 1.0
             *  y = ((-2.0 * char_y) / screenHeight) + 1.0
             */
#define X(x) (((2.0f * (x)) / (float)windowWidth) - 1.0f)
#define Y(y) (((-2.0f * (y)) / (float)windowHeight) + 1.0f)
            quad.x0 = X(quad.x0);
            quad.y0 = Y(quad.y0);
            quad.x1 = X(quad.x1);
            quad.y1 = Y(quad.y1);

            float text_vertices[] = {
                quad.x0,    quad.y0,    0.0f,   quad.s0,    quad.t0,               /* top left */
                quad.x1,    quad.y1,    0.0f,   quad.s1,    quad.t1,               /* bottom right */
                quad.x0,    quad.y1,    0.0f,   quad.s0,    quad.t1,               /* bottom left */

                quad.x0,    quad.y0,    0.0f,   quad.s0,    quad.t0,               /* top left */
                quad.x1,    quad.y0,    0.0f,   quad.s1,    quad.t0,               /* top right */
                quad.x1,    quad.y1,    0.0f,   quad.s1,    quad.t1,               /* bottom right */
            };
            glBufferData(GL_ARRAY_BUFFER, sizeof(text_vertices), text_vertices, GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 6);

#undef X
#undef Y
        }
    }
}

text_context::text_context():
    impl(new text_context_impl())
{
}

void text_context::draw(float ticks)
{
    this->impl->draw(ticks);
}

void text_context::drawText(float x, float y, const std::string& text)
{
    impl->drawText(text, x, y);
}

std::shared_ptr<text_context> make_text_context()
{
    return std::make_shared<text_context>();
}





