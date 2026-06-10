#include "Timer.h"
#include <GLFW/glfw3.h>

void Timer::init() {
    lastTime = glfwGetTime();
    deltaTime = 0.0f;
    fps = 0.0f;
    fpsAccumulator = 0.0f;
    frameCount = 0;
}

void Timer::tick() {
    double currentTime = glfwGetTime();
    deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    // Clamp delta time to avoid huge jumps (e.g. after breakpoint)
    if (deltaTime > 0.25f) deltaTime = 0.25f;

    fpsAccumulator += deltaTime;
    frameCount++;

    if (fpsAccumulator >= FPS_UPDATE_INTERVAL) {
        fps = static_cast<float>(frameCount) / fpsAccumulator;
        frameCount = 0;
        fpsAccumulator = 0.0f;
    }
}
