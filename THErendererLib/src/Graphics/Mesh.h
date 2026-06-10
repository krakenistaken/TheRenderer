#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void upload(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    void setupInstancing(const std::vector<glm::mat4>& models);
    void draw() const;
    void drawInstanced(uint32_t instanceCount) const;
    uint32_t getTriangleCount() const { return indexCount / 3; }
    void cleanup();

private:
    GLuint vao = 0, vbo = 0, ebo = 0, instanceVBO = 0;
    uint32_t indexCount = 0;
};
