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

#include "camera.h"
#include "context.h"
#include "main_context.h"
#include "text_context.h"

int windowWidth = 800;
int windowHeight = 600;
std::string resDir;
std::shared_ptr<text_context> textContext;

Camera camera;
static bool firstMouse = true;
static float lastMouseX = 0.0f;
static float lastMouseY = 0.0f;

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

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::FORWARD, deltaTicks);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTicks);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::LEFT, deltaTicks);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::RIGHT, deltaTicks);
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;
    lastMouseX = xpos;
    lastMouseY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
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

