#include <fmt/format.h>
#include <fmt/printf.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "image.h"
#include "text_context.h"
#include "meshes.h"

class lighting_context : public context {
    private:
        unsigned int vao;
        std::shared_ptr<Shader> shader;

        unsigned int lampVao;
        std::shared_ptr<Shader> lampShader;
        glm::vec3 lightPos;
    public:
        lighting_context();
        void draw(float ticks);
};


lighting_context::lighting_context()
{
    // Create container
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube1), cube1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float),
                          BUFFER_OBJECT(0));
    glEnableVertexAttribArray(0);
    
    shader = std::make_shared<Shader>(fmt::format("{}/lighting.vs", resDir),
                                      fmt::format("{}/lighting.fs", resDir));

    
    // Create Lamp
    lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

    glGenVertexArrays(1, &lampVao);
    glBindVertexArray(lampVao);

    // we only need to bind to the VBO, the container's VBO's data already contains the correct data.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // set the vertex attributes (only position data for our lamp)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), 
                          BUFFER_OBJECT(0));
    glEnableVertexAttribArray(0);

    lampShader= std::make_shared<Shader>(fmt::format("{}/lamp.vs", resDir),
                                          fmt::format("{}/lamp.fs", resDir));

    glEnable(GL_DEPTH_TEST);
}

void lighting_context::draw(float ticks)
{
    // Draw container
    shader->use();
    shader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    glBindVertexArray(vao);

    auto view = glm::lookAt(camera.position(),
                            camera.position() + camera.front(),
                            camera.up());

    auto projection = glm::perspective(glm::radians(camera.zoom()),
                                       (float)windowWidth / (float)windowHeight,
                                       0.1f, 100.0f);

    auto model = glm::mat4(1.0f);
    shader->setMatrix("view", view);
    shader->setMatrix("projection", projection);
    shader->setMatrix("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // draw lamp
    lampShader->use();
    glBindVertexArray(lampVao);

    model = glm::translate(glm::mat4(), lightPos);
    model = glm::scale(model, glm::vec3(0.2f));

    lampShader->setMatrix("view", view);
    lampShader->setMatrix("projection", projection);
    lampShader->setMatrix("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

std::shared_ptr<context> make_lighting_context()
{
    return std::make_shared<lighting_context>();
}




