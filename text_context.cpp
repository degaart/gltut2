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

class text_context : public context {
    private:
        unsigned int text_vao;
        std::shared_ptr<Shader> text_program;
        unsigned int text_tex;
        std::vector<stbtt_bakedchar> charData;
        unsigned int text_vbo;
    public:
        text_context();
        void draw(float ticks);
};

#define TEX_SIZE   128

text_context::text_context()
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

    this->charData = std::vector<stbtt_bakedchar>(96); // 127 - ' ' + 1 = 96
    int ret = stbtt_BakeFontBitmap(fontData.data(), 0,
                                   16,
                                   pixels.data(), TEX_SIZE, TEX_SIZE,
                                   ' ', 96,
                                   charData.data());

    glGenTextures(1, &this->text_tex);
    glBindTexture(GL_TEXTURE_2D, this->text_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D,
                 0,                     /* mipmap level */
                 GL_RGBA,               /* format to store into */
                 TEX_SIZE, TEX_SIZE,
                 0,                     /* unused legacy stuff */
                 GL_RED,                /* input format */
                 GL_UNSIGNED_BYTE,      /* input datatype */
                 pixels.data());

    glEnable(GL_DEPTH_TEST);
}

void text_context::draw(float ticks)
{
    this->text_program->use();
    glBindVertexArray(this->text_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->text_tex);
    this->text_program->setInt("texture1", 0);

#if 0
    auto transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f));
    context->text_program->setMatrix("transform", transform);
#endif

    float cursor_x = 0.0f;
    float cursor_y = 0.0f;

    float char_x = 0.0f, char_y = 0.0f;
    stbtt_aligned_quad quad;
    stbtt_GetBakedQuad(this->charData.data(),
                       TEX_SIZE, TEX_SIZE,
                       '@' - ' ',
                       &char_x, &char_y,
                       &quad,
                       1);

    /*
     * TODO: Use screenWidth and screenHeight to generate vertices positions
     * Take into account that the y-coordinate is inverted in OpenGL
     */
    float offset_x = char_x;
    float offset_y = char_y;
    static const float text_vertices[] = {
        -1.0f,  1.0f,   0.0f,   quad.s0,    quad.t0,               /* top left */
        1.0f,   -1.0f,  0.0f,   quad.s1,    quad.t1,               /* bottom right */
        -1.0f,  -1.0f,  0.0f,   quad.s0,    quad.t1,               /* bottom left */

        -1.0f,  1.0f,   0.0f,   quad.s0,    quad.t0,               /* top left */
        1.0f,   1.0f,   0.0f,   quad.s1,    quad.t0,               /* top right */
        1.0f,   -1.0f,  0.0f,   quad.s1,    quad.t1                /* bottom right */
    };
    glBindBuffer(GL_ARRAY_BUFFER, this->text_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(text_vertices), text_vertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);
}

std::shared_ptr<context> make_text_context()
{
    return std::make_shared<text_context>();
}

