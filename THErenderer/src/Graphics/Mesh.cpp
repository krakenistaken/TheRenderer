#include "Mesh.h"

Mesh::~Mesh() { cleanup(); }

Mesh::Mesh(Mesh&& other) noexcept
    : vao(other.vao), vbo(other.vbo), ebo(other.ebo), instanceVBO(other.instanceVBO), indexCount(other.indexCount) {
    other.vao = other.vbo = other.ebo = other.instanceVBO = 0;
    other.indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        vao = other.vao; vbo = other.vbo; ebo = other.ebo; instanceVBO = other.instanceVBO;
        indexCount = other.indexCount;
        other.vao = other.vbo = other.ebo = other.instanceVBO = 0;
        other.indexCount = 0;
    }
    return *this;
}

void Mesh::upload(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    cleanup();
    indexCount = static_cast<uint32_t>(indices.size());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Mesh::setupInstancing(const std::vector<glm::mat4>& models) {
    if (instanceVBO == 0) {
        glGenBuffers(1, &instanceVBO);
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);

    std::size_t vec4Size = sizeof(glm::vec4);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
        glVertexAttribDivisor(3 + i, 1);
    }
    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::drawInstanced(uint32_t instanceCount) const {
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (instanceVBO) { glDeleteBuffers(1, &instanceVBO); instanceVBO = 0; }
    if (ebo) { glDeleteBuffers(1, &ebo); ebo = 0; }
    if (vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }
    if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
    indexCount = 0;
}
