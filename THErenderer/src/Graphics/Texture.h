#pragma once
#include <GL/glew.h>
#include <string>

class Texture {
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    bool loadFromFile(const std::string& path);
    void bind(uint32_t slot = 0) const;
    void unbind() const;
    void cleanup();

private:
    GLuint id = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
};
