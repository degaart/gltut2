#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch):
    _front(glm::vec3(0.0f, 0.0f, -1.0f)), _movementSpeed(DEFAULT_SPEED),
    _mouseSensitivity(DEFAULT_SENSIVITY), _zoom(DEFAULT_ZOOM),
    _position(position), _worldUp(up),
    _yaw(yaw), _pitch(pitch)
{
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, 
               float upX, float upY, float upZ, 
               float yaw, float pitch):
    _front(glm::vec3(0.0f, 0.0f, -1.0f)), _movementSpeed(DEFAULT_SPEED),
    _mouseSensitivity(DEFAULT_SENSIVITY), _zoom(DEFAULT_ZOOM),
    _position(glm::vec3(posX, posY, posZ)), _worldUp(glm::vec3(upX, upY, upZ)),
    _yaw(yaw), _pitch(pitch)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(_position, _position + _front, _up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = _movementSpeed * deltaTime;
    if(direction == CameraMovement::FORWARD)
        _position += _front * velocity;
    if(direction == CameraMovement::BACKWARD)
        _position -= _front * velocity;
    if(direction == CameraMovement::LEFT)
        _position -= _right * velocity;
    if(direction == CameraMovement::RIGHT)
        _position += _right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= _mouseSensitivity;
    yoffset *= _mouseSensitivity;

    _yaw   = glm::mod(_yaw + xoffset, 360.0f);
    _pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (_pitch > 89.0f)
            _pitch = 89.0f;
        if (_pitch < -89.0f)
            _pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    if (_zoom >= 1.0f && _zoom <= 45.0f)
        _zoom -= yoffset;
    if (_zoom <= 1.0f)
        _zoom = 1.0f;
    if (_zoom >= 45.0f)
        _zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);

    // Also re-calculate the Right and Up vector
    _right = glm::normalize(glm::cross(_front, _worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up    = glm::normalize(glm::cross(_right, _front));
}


const glm::vec3& Camera::position() const
{
    return _position;
}

const glm::vec3& Camera::front() const
{
    return _front;
}

const glm::vec3& Camera::up() const
{
    return _up;
}

float Camera::yaw() const
{
    return _yaw;
}

float Camera::pitch() const
{
    return _pitch;
}

float Camera::zoom() const
{
    return _zoom;
}


