#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
    public:
        Shader(std::string vertexShaderFile, std::string fragmentShaderFile);
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        void use();
        unsigned int getId() const;
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setMatrix(const std::string& name, const glm::mat4& value) const;
        void setVec3(const std::string& name, float x, float y, float z) const;
        void setVec3(const std::string& name, const glm::vec3& v) const;

    private:
        unsigned int _id;
};

