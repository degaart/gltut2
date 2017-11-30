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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BUFFER_OBJECT(i) ((void*)(i))


// Callback called when the window is resized
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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

    unsigned int VBO;
    glGenBuffers(1, &VBO);

#if 0
    unsigned int EBO;
    glGenBuffers(1, &EBO);
#endif

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

#if 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
#endif


#if 0
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
#endif
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

    // Texture stuffs
    unsigned int textures[2];
    glGenTextures(2, textures);

    Image image0("res/container.jpg");
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x-coord
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y-coord

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // scaling downward
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // scaling upward

    int inputFormat = image0.getChannels() == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D,
                 0,                 /* mipmap level */
                 GL_RGB,            /* format to store into */
                 image0.getWidth(), image0.getHeight(),
                 0,                 /* unused legacy stuff */
                 inputFormat,            /* input format */
                 GL_UNSIGNED_BYTE,  /* input datatype */
                 image0.getData());
    glGenerateMipmap(GL_TEXTURE_2D);

    Image image1("res/awesomeface.png", true);
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
    auto shader = std::make_shared<Shader>("res/shader.vs", 
                                           "res/shader.fs");

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
    // Model matrix (rotate along the X axis)
    glm::mat4 model(1.0f);
    float angle = ticks * glm::radians(50.0f);
    model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.0f));

    // View matrix (move backwards by translating along Z axis in negative direction)
    //float position = -3.0f * abs(sin(ticks));
    float position = -3.0f;
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, position));

    // Projection matrix (45° fov)
    auto nearPlane = 0.1f;
    auto farPlane = 100.0f;
    auto projection = glm::perspective(glm::radians(45.0f), 
                                       (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 
                                       nearPlane, 
                                       farPlane);

    // Init shader
    context->program->use();
    context->program->setInt("texture1", 0);
    context->program->setInt("texture2", 1);
    context->program->setFloat("texMix", 0.5 + (cos(ticks) / 2.0));
    context->program->setMatrix("model", model);
    context->program->setMatrix("view", view);
    context->program->setMatrix("projection", projection);

    glBindVertexArray(context->VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, context->textures[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, context->textures[1]);

    glDrawArrays(GL_TRIANGLES, 0, 36);
#if 0
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#endif

    glBindVertexArray(0);
    glUseProgram(0);
}

int main()
{
    // Init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create glfw window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "gltut", NULL, NULL);
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
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

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

