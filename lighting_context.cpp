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
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube2), cube2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float),
                          BUFFER_OBJECT(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float),
                          BUFFER_OBJECT(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    shader = std::make_shared<Shader>(fmt::format("{}/lighting.vs", resDir),
                                      fmt::format("{}/lighting.fs", resDir));
    
    // Create Lamp
    lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

    glGenVertexArrays(1, &lampVao);
    glBindVertexArray(lampVao);

    unsigned int lightVbo;
    glGenBuffers(1, &lightVbo);
    glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube1), cube1, GL_STATIC_DRAW);

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
    shader->setVec3("viewPos", camera.position());

    glm::vec3 lightColor;
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    float diffuse = 0.75f;
    float ambient = 0.2f;
    glm::vec3 diffuseColor = lightColor   * glm::vec3(diffuse);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(ambient);
    
    lightPos.x = abs(cos(ticks));
    lightPos.y = 1.0f + abs(sin(ticks));
    lightPos.z = 2.0f + abs(sin(ticks));
    shader->setVec3("light.position", lightPos);
    shader->setVec3("light.ambient",  ambientColor);
    shader->setVec3("light.diffuse",  diffuseColor); // darken the light a bit to fit the scene
    shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f); 

    shader->setVec3("material.ambient",  1.0f, 0.5f, 0.31f);
    shader->setVec3("material.diffuse",  1.0f, 0.5f, 0.31f);
    shader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    shader->setFloat("material.shininess", 32.0f);

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

    textContext->drawText(0.0f, 16.0f*3.0f, 
                          fmt::sprintf("ambient: %0.2f, diffuse: %0.2f",
                                       ambient, diffuse));
}

std::shared_ptr<context> make_lighting_context()
{
    return std::make_shared<lighting_context>();
}




