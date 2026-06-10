#include "HUD.h"
#include "FontData.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdio>

HUD::~HUD() { cleanup(); }

void HUD::cleanup() {
    if (fontTexture) { glDeleteTextures(1, &fontTexture); fontTexture = 0; }
    if (vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }
    if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
    textShader.cleanup();
}

void HUD::createFontTexture() {
    const int ATLAS_COLS = 16;
    const int ATLAS_ROWS = 8;
    const int CHAR_W = 8;
    const int CHAR_H = 8;
    const int TEX_W = ATLAS_COLS * CHAR_W;  // 128
    const int TEX_H = ATLAS_ROWS * CHAR_H;  // 64

    std::vector<uint8_t> pixels(TEX_W * TEX_H, 0);

    for (int ch = 0; ch < 128; ch++) {
        int col = ch % ATLAS_COLS;
        int row = ch / ATLAS_COLS;

        for (int y = 0; y < CHAR_H; y++) {
            unsigned char rowBits = font8x8_basic[ch][y];
            for (int x = 0; x < CHAR_W; x++) {
                if (rowBits & (1 << x)) {
                    int px = col * CHAR_W + x;
                    int py = row * CHAR_H + y;
                    pixels[py * TEX_W + px] = 255;
                }
            }
        }
    }

    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, TEX_W, TEX_H, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool HUD::init(int w, int h) {
    screenWidth = w;
    screenHeight = h;

    if (!textShader.loadFromFile("shaders/text.vert", "shaders/text.frag")) {
        fprintf(stderr, "Failed to load text shader\n");
        return false;
    }

    createFontTexture();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6 * 256, nullptr, GL_DYNAMIC_DRAW);

    // Position (vec2)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // TexCoord (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
    return true;
}

void HUD::updateScreenSize(int w, int h) {
    screenWidth = w;
    screenHeight = h;
}

void HUD::renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    textShader.use();
    textShader.setVec3("uTextColor", color);

    const float CHAR_W = 8.0f * scale;
    const float CHAR_H = 8.0f * scale;
    const float UV_W = 1.0f / 16.0f;
    const float UV_H = 1.0f / 8.0f;

    std::vector<float> vertices;
    vertices.reserve(text.size() * 24);

    float curX = x;
    for (char c : text) {
        int idx = (int)(unsigned char)c;
        if (idx < 0 || idx >= 128) idx = '?';

        float col = (float)(idx % 16);
        float row = (float)(idx / 16);
        float u0 = col * UV_W;
        float v0 = row * UV_H;
        float u1 = u0 + UV_W;
        float v1 = v0 + UV_H;

        float x0 = curX, y0 = y;
        float x1 = curX + CHAR_W, y1 = y + CHAR_H;

        // Triangle 1
        vertices.insert(vertices.end(), {x0, y0, u0, v0});
        vertices.insert(vertices.end(), {x1, y0, u1, v0});
        vertices.insert(vertices.end(), {x1, y1, u1, v1});
        // Triangle 2
        vertices.insert(vertices.end(), {x0, y0, u0, v0});
        vertices.insert(vertices.end(), {x1, y1, u1, v1});
        vertices.insert(vertices.end(), {x0, y1, u0, v1});

        curX += CHAR_W;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(text.size() * 6));
    glBindVertexArray(0);
}

void HUD::beginRender() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    textShader.use();
    glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
    textShader.setMat4("uProjection", projection);
    textShader.setInt("uFontAtlas", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
}

void HUD::endRender() {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
