#pragma once

#include <string>

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

    private:
        unsigned int _id;
};

