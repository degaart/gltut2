#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <memory>
#include <fmt/format.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "shader.h"
#include "exception.h"
#include "image.h"
#include "system.h"

static int windowWidth = 800;
static int windowHeight = 600;

static std::string resDir;

#define BUFFER_OBJECT(i) ((void*)(i))

//#define USE_EBO


// Callback called when the window is resized
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

// Process inputs
static void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Prepare context for drawing on screen
struct gl_context {
    std::shared_ptr<Shader> program;
    unsigned int VAO;
    unsigned int textures[2];
};

static gl_context prepare_context()
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

    unsigned int VAO; 
    glGenVertexArrays(1, &VAO);
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
    unsigned int textures[2];
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

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Return results
    gl_context result;
    result.program = shader;
    result.VAO = VAO;
    memcpy(result.textures, textures, sizeof(result.textures));
    return result;
}

static void draw(gl_context* context, float ticks)
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
    context->program->use();
    context->program->setFloat("texMix", 0.5 + (cos(ticks) / 2.0));

    glBindVertexArray(context->VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, context->textures[0]);
    context->program->setInt("texture1", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, context->textures[1]);
    context->program->setInt("texture2", 1);


    // View matrix (move backwards by translating along Z axis in negative direction)
    float position = -3.0f;
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, position));
    context->program->setMatrix("view", view);

    // Projection matrix (45° fov)
    auto nearPlane = 0.1f;
    auto farPlane = 100.0f;
    auto projection = glm::perspective(glm::radians(45.0f), 
                                       (float)windowWidth / (float)windowHeight, 
                                       nearPlane, 
                                       farPlane);
    context->program->setMatrix("projection", projection);

    for(int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
        // Model matrix (rotate along the X axis)
        auto model = glm::translate(glm::mat4(1.0f), cubePositions[i]);

        float angle = (20.0f * i) + (ticks * 10.0f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

        context->program->setMatrix("model", model);

#if defined(USE_EBO)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#else
        glDrawArrays(GL_TRIANGLES, 0, 36);
#endif
    }


    glBindVertexArray(0);
    glUseProgram(0);
}

int main(int argc, char** argv)
{
    // Get resource path
    auto exePath = sys::exepath(argc, argv);
    auto appPath = sys::dirname(exePath);
    resDir = fmt::format("{}/../res", appPath);
    if(!sys::exists(resDir)) {
        resDir = fmt::format("{}/../Resources", appPath);
    }

    // Init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create glfw window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "gltut", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // Set viewport
    glViewport(0, 0, windowWidth, windowHeight);

    // tell it to call a callback when window is resized    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Prepare OpenGL context
    gl_context context = prepare_context();

    // Event loop
    float lastTicks = glfwGetTime();
    long frames = 0;
    int lastFps = 0;
    while(!glfwWindowShouldClose(window)) {
        float ticks = glfwGetTime();

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw(&context, ticks);

        glfwSwapBuffers(window);

        frames++;
        if(ticks - lastTicks > 1.0f) {
            double fps = double(frames) / (ticks - lastTicks);
            if(fps != lastFps) {
                std::string title = fmt::format("gltut - {} fps", (int)fps);
                glfwSetWindowTitle(window, title.c_str());
                lastFps = fps;
            }
            lastTicks = ticks;
            frames = 0;
        }

        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
  
    return 0;
}

