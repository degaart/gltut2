#include "main_context.h"

#include <fmt/format.h>
#include <fmt/printf.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "image.h"
#include "text_context.h"

class main_context : public context {
    private:
        std::shared_ptr<Shader> program;
        unsigned int VAO;
        unsigned int textures[2];
    public:
        main_context();
        void draw(float ticks);
};

main_context::main_context()
{
    // Cube
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,                // first triangle
        1, 2, 3                 // second triangle
    }; 

    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

#if defined(USE_EBO)
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    
    // Vertex position attributes
    glVertexAttribPointer(0,                    /* index (in the vertex shader source, position=0) */
                          3,                    /* size of vertex attribute */ 
                          GL_FLOAT,             /* type */
                          GL_FALSE,             /* do not normalize */
                          8 * sizeof(float),    /* space between consecutive vertex attribute sets */
                          BUFFER_OBJECT(0));    /* starting position */
    glEnableVertexAttribArray(0);               /* enable attribute at location 0 */

    // Vertex color attribute
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          BUFFER_OBJECT(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Vertex texture coords attribute
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          BUFFER_OBJECT(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
#else
    // Vertex position attributes
    glVertexAttribPointer(0,                    /* index (in the vertex shader source, position=0) */
                          3,                    /* size of vertex attribute */ 
                          GL_FLOAT,             /* type */
                          GL_FALSE,             /* do not normalize */
                          5 * sizeof(float),    /* space between consecutive vertex attribute sets */
                          BUFFER_OBJECT(0));    /* starting position */
    glEnableVertexAttribArray(0);               /* enable attribute at location 0 */

    // Vertex texture coords attribute
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          BUFFER_OBJECT(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
#endif

    // Texture stuffs
    glGenTextures(2, textures);

    Image image0(fmt::format("{}/container.jpg", resDir));
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x-coord
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y-coord

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // scaling downward
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // scaling upward

    int inputFormat = image0.getChannels() == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D,
                 0,                         /* mipmap level */
                 GL_RGB,                    /* format to store into */
                 image0.getWidth(), image0.getHeight(),
                 0,                         /* unused legacy stuff */
                 inputFormat,               /* input format */
                 GL_UNSIGNED_BYTE,          /* input datatype */
                 image0.getData());
    glGenerateMipmap(GL_TEXTURE_2D);

    Image image1(fmt::format("{}/awesomeface.png", resDir), true);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x-coord
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y-coord

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // scaling downward
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // scaling upward

    inputFormat = image1.getChannels() == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D,
                 0,                 /* mipmap level */
                 inputFormat,            /* format to store into */
                 image1.getWidth(), image1.getHeight(),
                 0,                 /* unused legacy stuff */
                 inputFormat,            /* input format */
                 GL_UNSIGNED_BYTE,  /* input datatype */
                 image1.getData());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Create shaders
    auto shader = std::make_shared<Shader>(fmt::format("{}/shader.vs", resDir), 
                                           fmt::format("{}/shader.fs", resDir));

    // Assign textures to shaders
    shader->use();
    shader->setInt("texture1", 0);
    shader->setInt("texture2", 1);
    this->program = shader;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
}

void main_context::draw(float ticks)
{
    static glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    // Init shader
    this->program->use();
    this->program->setFloat("texMix", 0.5 + (cos(ticks) / 2.0));

    glBindVertexArray(this->VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->textures[0]);
    this->program->setInt("texture1", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->textures[1]);
    this->program->setInt("texture2", 1);

    // View matrix
    auto view = glm::lookAt(camera.position(),                    /* Position */
                            camera.position() + camera.front(),   /* Target */
                            camera.up());                        /* Up */
    this->program->setMatrix("view", view);

    // Projection matrix (45° fov)
    auto nearPlane = 0.1f;
    auto farPlane = 100.0f;
    auto projection = glm::perspective(glm::radians(camera.zoom()), 
                                       (float)windowWidth / (float)windowHeight, 
                                       nearPlane,
                                       farPlane);
    this->program->setMatrix("projection", projection);

    for(int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
        // Model matrix (rotate along the X axis)
        auto model = glm::translate(glm::mat4(1.0f), cubePositions[i]);

        //float angle = (20.0f * i) + (ticks * 10.0f);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

        this->program->setMatrix("model", model);

#if defined(USE_EBO)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#else
        glDrawArrays(GL_TRIANGLES, 0, 36); // Number of vertices (not number of triangles)
#endif
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Cleanup
    glBindVertexArray(0);
    glUseProgram(0);

    textContext->drawText(0.0f, 16.0f, 
                          fmt::sprintf("cam: (%0.02f, %0.02f, %0.02f)", 
                                      camera.position().x,
                                      camera.position().y,
                                      camera.position().z));
    textContext->drawText(0.0f, 32.0f,
                          fmt::sprintf("yaw: %0.2f, pitch: %0.2f, fov: %0.2f",
                                       camera.yaw(), camera.pitch(), camera.zoom()));
}

std::shared_ptr<context> make_main_context()
{
    return std::make_shared<main_context>();
}


