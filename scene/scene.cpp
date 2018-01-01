#include <fmt/format.h>
#include <fmt/printf.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "context.h"
#include "scene.h"
#include "shader.h"
#include "image.h"
#include "meshes.h"

static unsigned int vao;
static std::shared_ptr<Shader> shader;

static unsigned int diffuseMap;
static unsigned int specularMap;

static unsigned int lampVao;
static std::shared_ptr<Shader> lampShader;

static void init(context* ctx)
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
    
    Image diffuseTexture(fmt::format("{}/container2.png", ctx->resDir));
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

    Image specularTexture(fmt::format("{}/container2_specular.png", ctx->resDir)); 
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

    shader = std::make_shared<Shader>(fmt::format("{}/lighting.vs", ctx->resDir),
                                      fmt::format("{}/lighting.fs", ctx->resDir));
    
    // Create Lamp
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

    lampShader= std::make_shared<Shader>(fmt::format("{}/lamp.vs", ctx->resDir),
                                          fmt::format("{}/lamp.fs", ctx->resDir));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

static void release(context* ctx)
{
}

static void draw(float ticks, context* ctx)
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

    static glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f, 0.2f, 2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3( 0.0f, 0.0f, -3.0f)
    };

    // Draw container
    shader->use();
    shader->setVec3("viewPos", ctx->camera.position());

    // Directional light
    shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader->setVec3("dirLight.ambient",  0.1f, 0.1f, 0.1f);
    shader->setVec3("dirLight.diffuse",  0.50f, 0.50f, 0.50f);
    shader->setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f); 
    
    // Point Lights
    shader->setInt("pointLightCount", sizeof(pointLightPositions) / sizeof(pointLightPositions[0]));
    for(int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
        shader->setVec3(fmt::sprintf("pointLights[%d].position", i), pointLightPositions[i]);
        shader->setFloat(fmt::sprintf("pointLights[%d].constant", i), 1.0f);
        shader->setFloat(fmt::sprintf("pointLights[%d].linear", i), 0.09f);
        shader->setFloat(fmt::sprintf("pointLights[%d].quadratic", i), 0.032f);

        shader->setVec3(fmt::sprintf("pointLights[%d].ambient", i),  0.2f, 0.2f, 0.2f);
        shader->setVec3(fmt::sprintf("pointLights[%d].diffuse", i),  0.75f, 0.75f, 0.75f);
        shader->setVec3(fmt::sprintf("pointLights[%d].specular", i), 1.0f, 1.0f, 1.0f); 
    }

    shader->setFloat("material.shininess", 64.0f);

    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    shader->setInt("material.diffuse", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    shader->setInt("material.specular", 1);

    auto view = glm::lookAt(ctx->camera.position(),
                            ctx->camera.position() + ctx->camera.front(),
                            ctx->camera.up());

    auto projection = glm::perspective(glm::radians(ctx->camera.zoom()),
                                       (float)ctx->windowWidth / (float)ctx->windowHeight,
                                       0.1f, 100.0f);

    shader->setMatrix("view", view);
    shader->setMatrix("projection", projection);

    for(int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);

        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

        shader->setMatrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // draw lamp
    lampShader->use();
    glBindVertexArray(lampVao);

    lampShader->setMatrix("view", view);
    lampShader->setMatrix("projection", projection);

    for(int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
        auto model = glm::translate(glm::mat4(), pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));

        lampShader->setMatrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

extern "C" void getScene(scene* scene_buf)
{
    scene_buf->init = init;
    scene_buf->release = release;
    scene_buf->draw = draw;
}


