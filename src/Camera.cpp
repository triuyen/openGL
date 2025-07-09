#include "Camera.hpp"
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : position(position), worldUp(up), yaw(yaw), pitch(pitch),
          front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED),
          mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : position(glm::vec3(posX, posY, posZ)), worldUp(glm::vec3(upX, upY, upZ)),
          yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f)),
          movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(zoom), aspectRatio, 0.1f, 100.0f);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    switch(direction) {
        case FORWARD:
            position += front * velocity;
            break;
        case BACKWARD:
            position -= front * velocity;
            break;
        case LEFT:
            position -= right * velocity;
            break;
        case RIGHT:
            position += right * velocity;
            break;
        case UP:
            position += worldUp * velocity;
            break;
        case DOWN:
            position -= worldUp * velocity;
            break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain pitch to prevent screen flipping
    if (constrainPitch) {
        pitch = std::clamp(pitch, -89.0f, 89.0f);
    }

    // Update front, right and up vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    zoom -= yoffset;
    zoom = std::clamp(zoom, 1.0f, 90.0f);
}

void Camera::setPosition(const glm::vec3& newPosition)
{
    position = newPosition;
}

void Camera::setTarget(const glm::vec3& target)
{
    glm::vec3 direction = glm::normalize(target - position);

    // Calculate yaw and pitch from direction vector
    yaw = glm::degrees(atan2(direction.z, direction.x));
    pitch = glm::degrees(asin(-direction.y));

    updateCameraVectors();
}

void Camera::setMovementSpeed(float speed)
{
    movementSpeed = speed;
}

void Camera::setMouseSensitivity(float sensitivity)
{
    mouseSensitivity = sensitivity;
}

void Camera::setZoom(float newZoom)
{
    zoom = std::clamp(newZoom, 1.0f, 90.0f);
}

glm::vec3 Camera::getPosition() const
{
    return position;
}

glm::vec3 Camera::getFront() const
{
    return front;
}

glm::vec3 Camera::getUp() const
{
    return up;
}

glm::vec3 Camera::getRight() const
{
    return right;
}

float Camera::getZoom() const
{
    return zoom;
}

float Camera::getYaw() const
{
    return yaw;
}

float Camera::getPitch() const
{
    return pitch;
}

void Camera::reset()
{
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    yaw = YAW;
    pitch = PITCH;
    zoom = ZOOM;
    movementSpeed = SPEED;
    mouseSensitivity = SENSITIVITY;
    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    // Calculate the new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    // Calculate right and up vectors
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}