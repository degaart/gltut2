#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum class CameraMovement {
    FORWARD, BACKWARD, LEFT, RIGHT,
    UP, DOWN
};

class Camera {
    public:
        constexpr static const float DEFAULT_YAW = -90.0f;
        constexpr static const float DEFAULT_PITCH = 0.0f;
        constexpr static const float DEFAULT_ROLL = 0.0f;
        constexpr static const float DEFAULT_SPEED = 2.5f;
        constexpr static const float DEFAULT_SENSIVITY = 0.1f;
        constexpr static const float DEFAULT_ZOOM = 45.0f;

    private:
        glm::vec3 _position, _front, _up, _right, _worldUp;
        float _yaw, _pitch;
        float _movementSpeed, _mouseSensitivity, _zoom;

    public:

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), 
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
               float yaw = DEFAULT_YAW, 
               float pitch = DEFAULT_PITCH);
        Camera(float posX, float posY, float posZ, 
               float upX, float upY, float upZ, 
               float yaw, float pitch);

        glm::mat4 getViewMatrix();
        void processKeyboard(CameraMovement direction, float deltaTime);
        void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
        void processMouseScroll(float xoffset, float yoffset);

        const glm::vec3& position() const;
        const glm::vec3& front() const;
        const glm::vec3& up() const;

        float yaw() const;
        float pitch() const;
        float zoom() const;

    private:
        void updateCameraVectors();
};


