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

        unsigned int diffuseMap;
        unsigned int specularMap;

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube3), cube3, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          BUFFER_OBJECT(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          BUFFER_OBJECT(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          BUFFER_OBJECT(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    Image diffuseTexture(fmt::format("{}/container2.png", resDir));
    glGenTextures(1, &diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 diffuseTexture.getWidth(), diffuseTexture.getHeight(),
                 0,
                 diffuseTexture.getChannels() == 3 ? GL_RGB : GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 diffuseTexture.getData());
    glGenerateMipmap(GL_TEXTURE_2D);

    Image specularTexture(fmt::format("{}/container2_specular.png", resDir)); 
    glGenTextures(1, &specularMap);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 specularTexture.getWidth(), specularTexture.getHeight(),
                 0,
                 specularTexture.getChannels() == 3 ? GL_RGB : GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 specularTexture.getData());
    glGenerateMipmap(GL_TEXTURE_2D);

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

    shader->setVec3("light.position", lightPos);
    shader->setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
    shader->setVec3("light.diffuse",  0.75f, 0.75f, 0.75f);
    shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f); 

    shader->setFloat("material.shininess", 64.0f);

    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    shader->setInt("material.diffuse", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    shader->setInt("material.specular", 1);

    auto view = glm::lookAt(camera.position(),
                            camera.position() + camera.front(),
                            camera.up());

    auto projection = glm::perspective(glm::radians(camera.zoom()),
                                       (float)windowWidth / (float)windowHeight,
                                       0.1f, 100.0f);

    auto model = glm::mat4(1.0f);
    auto angle = fmod(ticks * 40.0f, 360.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

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
                          fmt::sprintf("angle: %0.2f",
                                       angle));
}

std::shared_ptr<context> make_lighting_context()
{
    return std::make_shared<lighting_context>();
}




