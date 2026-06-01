#pragma once
#include <GL/glew.h>
#include "../Graphics/Shader.h"
#include <string>
#include <cstdint>

class HUD {
public:
    HUD() = default;
    ~HUD();

    bool init(int screenWidth, int screenHeight);
    void render(float fps, uint32_t polyCount, int currentScene);
    void renderText(const std::string& text, float x, float y, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f));
    void updateScreenSize(int width, int height);
    void cleanup();

private:
    Shader textShader;
    GLuint fontTexture = 0;
    GLuint vao = 0, vbo = 0;
    int screenWidth = 0, screenHeight = 0;

    void createFontTexture();
};
