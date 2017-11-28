#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <memory>
#include "stb_image.h"
#include "shader.h"
#include "exception.h"

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
    unsigned int texture;
};

static gl_context prepare_context()
{
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,                // first triangle
        1, 2, 3                 // second triangle
    }; 

    unsigned int VAO; 
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    // Texture stuffs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x-coord
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y-coord

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // scaling downward
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // scaling upward

    int width, height, nrChannels;
    unsigned char *data = stbi_load("res/container.jpg", &width, &height, &nrChannels, 0); 
    if(!data) {
        throw Exception("Failed to load texture");
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,                 /* mipmap level */
                 GL_RGB,            /* format to store into */
                 width, height,
                 0,                 /* unused legacy stuff */
                 GL_RGB,            /* input format */
                 GL_UNSIGNED_BYTE,  /* input datatype */
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Create shaders
    auto shader = std::make_shared<Shader>("res/shader4.vs", 
                                           "res/shader4.fs");

    // Return results
    gl_context result;
    result.program = shader;
    result.VAO = VAO;
    result.texture = texture;
    return result;
}

static void draw(gl_context* context, float ticks)
{
    context->program->use();
    context->program->setFloat("off", sin(ticks) / 2.0f);

    glBindVertexArray(context->VAO);
    glBindTexture(GL_TEXTURE_2D, context->texture);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
    while(!glfwWindowShouldClose(window)) {
        float ticks = glfwGetTime();

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw(&context, ticks);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    // Cleanup
    glfwTerminate();
  
    return 0;
}

