#pragma once

class Timer {
public:
    void init();
    void tick();
    float getDeltaTime() const { return deltaTime; }
    float getFPS() const { return fps; }

private:
    double lastTime = 0.0;
    float deltaTime = 0.0f;
    float fps = 0.0f;
    float fpsAccumulator = 0.0f;
    int frameCount = 0;
    static constexpr float FPS_UPDATE_INTERVAL = 0.5f;
};
