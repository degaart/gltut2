#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <cmath>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600



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

static std::string readFile(const std::string& filename)
{
    FILE* f = fopen(filename.c_str(), "rt");
    if(!f) {
        std::cerr << "Failed to open file " << filename << "\n";
        exit(1);
    }

    std::string result;
    while(!feof(f)) {
        char buffer[512];
        size_t ret = fread(buffer, 1, sizeof(buffer), f);
        if(!ret) {
            if(ferror(f)) {
                std::cerr << "I/O error reading from file " << filename << "\n";
                exit(1);
            } else {
                break;
            }
        }
        result.append(buffer, ret);
    }

    fclose(f);

    return result;
}

static unsigned int compileShader(GLenum type, const std::string& filename)
{
    std::string contents = readFile(filename);

    unsigned int shader = glCreateShader(type);
    const char* sources[1] = { contents.c_str() };
    glShaderSource(shader, 1, sources, NULL);
    glCompileShader(shader);

    int success;
    char msg[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, sizeof(msg), NULL, msg);

        const char* stype = NULL;
        switch(type) {
            case GL_VERTEX_SHADER:
                stype = "vertex shader";
                break;
            case GL_FRAGMENT_SHADER:
                stype = "fragment shader";
                break;
            default:
                stype = "unknown shader";
                break;
        }
        std::cerr << "Error compiling " 
                  << stype 
                  << " from \"" 
                  << filename
                  << "\": "
                  << msg
                  << "\n";
        exit(0);
    }

    return shader;
}

static unsigned int createShaderProgram(const std::string& vertexShaderFile,
                                        const std::string& fragmentShaderFile)
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderFile);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderFile);
 
    // Shader program (link vertex & fragment shader)
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char msg[512];
        glGetProgramInfoLog(shaderProgram, sizeof(msg), NULL, msg);
        std::cerr << "Error linking shader program: " << msg << "\n";
        exit(0);
    }

    // Shaders can be deleted once they are linked into the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    return shaderProgram;
}

// Prepare context for drawing on screen
struct gl_context {
    unsigned int program;
    unsigned int VAO;
};

static gl_context prepare_context()
{
    // data for 2 triangles next to each other
    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };    

    // Create 2 VAO for the two triangles
    unsigned int VAO; 
    glGenVertexArrays(1, &VAO);

    // Create 2 VBO for the 2 datasets
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Triangle 0, buffer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex position attributes
    glVertexAttribPointer(0,                    /* index (in the vertex shader source, position=0) */
                          3,                    /* size of vertex attribute */ 
                          GL_FLOAT,             /* type */
                          GL_FALSE,             /* do not normalize */
                          6 * sizeof(float),    /* space between consecutive vertex attribute sets */
                          (void*)0);            /* starting position */
    glEnableVertexAttribArray(0);               /* enable attribute at location 0 */

    // Vertex color attribute
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Create shaders
    unsigned int program;
    program = createShaderProgram("res/shader3.vs",
                                  "res/shader3.fs");

    // Return results
    gl_context result;
    result.program = program;
    result.VAO = VAO;
    return result;
}

static void draw(gl_context* context, float ticks)
{

    float greenValue = (sin(ticks) / 2.0f) + 0.5f;
    int vertexColorLocation = glGetUniformLocation(context->program, "ourColor");

    glUseProgram(context->program);
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    glBindVertexArray(context->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

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

