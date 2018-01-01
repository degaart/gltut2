#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <memory>
#include <fmt/format.h>
#include <fmt/printf.h>
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
#include "scene.h"
#include "scene_loader.h"

static context ctx;

#if 0
#endif
static bool firstMouse = true;
static float lastMouseX = 0.0f;
static float lastMouseY = 0.0f;

//#define USE_EBO

// Callback called when the window is resized
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    ctx.windowWidth = width;
    ctx.windowHeight = height;
}

// Process inputs
static void processInput(GLFWwindow *window, float deltaTicks)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ctx.camera.processKeyboard(CameraMovement::FORWARD, deltaTicks);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ctx.camera.processKeyboard(CameraMovement::BACKWARD, deltaTicks);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ctx.camera.processKeyboard(CameraMovement::LEFT, deltaTicks);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ctx.camera.processKeyboard(CameraMovement::RIGHT, deltaTicks);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        ctx.camera.processKeyboard(CameraMovement::UP, deltaTicks);
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        ctx.camera.processKeyboard(CameraMovement::DOWN, deltaTicks);
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

    ctx.camera.processMouseMovement(xoffset, yoffset);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ctx.camera.processMouseScroll(xoffset, yoffset);
}

int main(int argc, char** argv)
{
    // Get resource path
    auto exePath = sys::exepath(argc, argv);
    auto appPath = sys::dirname(exePath);
    ctx.resDir = fmt::format("{}/../res", appPath);
    if(!sys::exists(ctx.resDir)) {
        ctx.resDir = fmt::format("{}/../Resources", appPath);
    }
    fmt::printf("appPath: %s\n", appPath);

    // Init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create glfw window
    ctx.windowWidth = 800;
    ctx.windowHeight = 600;
    GLFWwindow* window = glfwCreateWindow(ctx.windowWidth, 
                                          ctx.windowHeight, 
                                          "gltut", 
                                          NULL, 
                                          NULL);
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
    glViewport(0, 0, 
               ctx.windowWidth, ctx.windowHeight);

    // tell it to call a callback when window is resized    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Add some input callbacks
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Load scene
    std::string sceneFile = fmt::sprintf("%s/../../../../scene/libscene.dylib", appPath);
    fmt::printf("sceneFile: %s\n", sceneFile);
    SceneLoader sceneLoader(sceneFile);

    // Initial camera pos
    ctx.camera = Camera(1.14f, 0.89f, 1.85f,
                        0.0f, 1.0f, 0.0f,
                        239.90f, -24.0f);

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

        processInput(window, deltaTicks);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        sceneLoader.draw(ticks, &ctx);

        frames++;
        if(frames > 30 && ticks - lastFrames > 0.0f) {
            fps = (int)(double(frames) / (ticks - lastFrames));
            frames = 0;
            lastFrames = ticks;

            sceneLoader.update(&ctx);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
  
    return 0;
}


