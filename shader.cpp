#include "shader.h"
#include "exception.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>

static std::string readFile(const std::string& filename)
{
    std::string result;
    std::ifstream is;

    is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    is.open(filename);

    std::stringstream ss;
    ss << is.rdbuf();

    is.close();

    return ss.str();
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

        std::string smsg = fmt::format("Error compiling {} from \"{}\": {}",
                                       stype,
                                       filename,
                                       msg);
        throw Exception(smsg);
    }

    return shader;
}

Shader::Shader(std::string vertexShaderFile, std::string fragmentShaderFile)
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

        auto smsg = fmt::format("Error linking shader program from \"{}\" and \"{}\": {}",
                                vertexShaderFile, fragmentShaderFile,
                                msg);
        throw Exception(smsg);
    }

    // Shaders can be deleted once they are linked into the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    _id = shaderProgram;
}

Shader::Shader(Shader&& shader) noexcept:
    _id(shader._id)
{
    shader._id = 0;
}

Shader& Shader::operator=(Shader&& shader) noexcept
{
    if(this != &shader) {
        _id = shader._id;
        shader._id = 0;
    }
    return *this;
}

Shader::~Shader()
{
    if(_id)
        glDeleteProgram(_id);
}

void Shader::use()
{
    glUseProgram(_id);
}

unsigned int Shader::getId() const
{
    return _id;
}

void Shader::setBool(const std::string& name, bool value) const
{
    setInt(name, (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    int location = glGetUniformLocation(_id, name.c_str());
    assert(location != -1);

    glUniform1i(location, (int)value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    int location = glGetUniformLocation(_id, name.c_str());
    assert(location != -1);

    glUniform1f(location, value);
}

void Shader::setMatrix(const std::string& name, const glm::mat4& value) const
{
    int location = glGetUniformLocation(_id, name.c_str());
    assert(location != -1);

    glUniformMatrix4fv(location,
                       1,                           /* Number of matrices to send */
                       GL_FALSE,                    /* Transpose? */
                       glm::value_ptr(value));
}



void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    int location = glGetUniformLocation(_id, name.c_str());
    assert(location != -1);

    glUniform3f(location,
                x,
                y,
                z);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const
{
    setVec3(name, v.x, v.y, v.z);
}

