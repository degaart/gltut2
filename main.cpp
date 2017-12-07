#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <memory>
#include <fmt/format.h>
#include <fcntl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "stb_truetype.h"
#include "shader.h"
#include "exception.h"
#include "image.h"
#include "system.h"

#include "context.h"
#include "main_context.h"
#include "text_context.h"

int windowWidth = 800;
int windowHeight = 600;
std::string resDir;
std::shared_ptr<text_context> textContext;

//#define USE_EBO


// Callback called when the window is resized
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

// Process inputs
static void processInput(GLFWwindow *window, 
                         std::shared_ptr<context> ctx,
                         float deltaTicks)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    unsigned mask = 0;
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        mask |= context::KEY_UP;
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        mask |= context::KEY_DOWN;
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        mask |= context::KEY_LEFT;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        mask |= context::KEY_RIGHT;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mask |= context::KEY_Z;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mask |= context::KEY_S;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mask |= context::KEY_Q;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mask |= context::KEY_D;
   
    if(mask) {
        ctx->onInput(mask, deltaTicks);
    }
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    context* ctx = (context*)glfwGetWindowUserPointer(window);
    ctx->onMouseMove((float)xpos, (float)ypos);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    context* ctx = (context*)glfwGetWindowUserPointer(window);
    ctx->onMouseScroll(xoffset, yoffset);
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
    std::shared_ptr<context> main_context = make_main_context();
    textContext = make_text_context();


    // Add some input callbacks
    glfwSetWindowUserPointer(window, main_context.get());

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // Event loop
    long frames = 0;
    int fps = 0;
    float lastFrames = 0.0f;

    float deltaTicks = 0.0f;	// Time between current frame and last frame
    float lastTicks = 0.0f;     // Time of last frame

    while(!glfwWindowShouldClose(window)) {
        float ticks = glfwGetTime();
        deltaTicks = ticks - lastTicks;
        lastTicks = ticks;

        processInput(window, main_context, deltaTicks);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        main_context->draw(ticks);

        frames++;
        if(frames > 30 && ticks - lastFrames > 0.0f) {
            fps = (int)(double(frames) / (ticks - lastFrames));
            frames = 0;
            lastFrames = ticks;
        }

        textContext->drawText(0.0f, 0.0f, fmt::format("{} fps", fps));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
  
    return 0;
}

