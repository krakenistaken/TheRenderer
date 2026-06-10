#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Window;

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           float yaw = -90.0f, float pitch = 0.0f);

    void processInput(const Window& window, float deltaTime);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    glm::vec3 getPosition() const { return position; }

private:
    glm::vec3 position;
    float yaw;   // degrees
    float pitch; // degrees

    float moveSpeed = 5.0f;
    float rotateSpeed = 90.0f; // degrees per second
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;
};
