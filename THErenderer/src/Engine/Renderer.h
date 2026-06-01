#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cstdint>

class Mesh;
class Shader;
class Texture;

class Renderer {
public:
    void init();
    void beginFrame();
    void drawMesh(const Mesh& mesh, Shader& shader, const glm::mat4& model,
                  const glm::mat4& view, const glm::mat4& projection,
                  const glm::vec3& viewPos);
    void drawMeshInstanced(const Mesh& mesh, Shader& shader, uint32_t instanceCount,
                           const glm::mat4& view, const glm::mat4& projection,
                           const glm::vec3& viewPos, const Texture* texture = nullptr);
    uint32_t getFramePolyCount() const { return framePolyCount; }

private:
    uint32_t framePolyCount = 0;
};
