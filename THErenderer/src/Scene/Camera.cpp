#include "Camera.h"
#include "../Engine/Window.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

Camera::Camera(glm::vec3 pos, float y, float p)
    : position(pos), yaw(y), pitch(p) {}

glm::vec3 Camera::getForward() const {
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);
    return glm::normalize(glm::vec3(
        std::cos(pitchRad) * std::cos(yawRad),
        std::sin(pitchRad),
        std::cos(pitchRad) * std::sin(yawRad)
    ));
}

glm::vec3 Camera::getRight() const {
    return glm::normalize(glm::cross(getForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::vec3 Camera::getUp() const {
    return glm::normalize(glm::cross(getRight(), getForward()));
}

void Camera::processInput(const Window& window, float deltaTime) {
    glm::vec3 forward = getForward();
    glm::vec3 right = getRight();

    float velocity = moveSpeed * deltaTime;

    // WASD - movement relative to camera direction
    if (window.isKeyPressed(GLFW_KEY_W)) position += forward * velocity;
    if (window.isKeyPressed(GLFW_KEY_S)) position -= forward * velocity;
    if (window.isKeyPressed(GLFW_KEY_A)) position -= right * velocity;
    if (window.isKeyPressed(GLFW_KEY_D)) position += right * velocity;

    // Space/Shift for vertical movement
    if (window.isKeyPressed(GLFW_KEY_SPACE))      position.y += velocity;
    if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT))  position.y -= velocity;

    // Arrow keys - rotation
    float rotVelocity = rotateSpeed * deltaTime;
    if (window.isKeyPressed(GLFW_KEY_LEFT))  yaw -= rotVelocity;
    if (window.isKeyPressed(GLFW_KEY_RIGHT)) yaw += rotVelocity;
    if (window.isKeyPressed(GLFW_KEY_UP))    pitch += rotVelocity;
    if (window.isKeyPressed(GLFW_KEY_DOWN))  pitch -= rotVelocity;

    // Clamp pitch to avoid gimbal flip
    pitch = std::clamp(pitch, -89.0f, 89.0f);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + getForward(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}
