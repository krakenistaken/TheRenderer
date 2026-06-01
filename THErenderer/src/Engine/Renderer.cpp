#include "Renderer.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Texture.h"

void Renderer::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
}

void Renderer::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    framePolyCount = 0;
}

void Renderer::drawMesh(const Mesh& mesh, Shader& shader, const glm::mat4& model,
                        const glm::mat4& view, const glm::mat4& projection,
                        const glm::vec3& viewPos) {
    shader.use();
    shader.setMat4("uModel", model);
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);
    shader.setVec3("uViewPos", viewPos);

    mesh.draw();
    framePolyCount += mesh.getTriangleCount();
}

void Renderer::drawMeshInstanced(const Mesh& mesh, Shader& shader, uint32_t instanceCount,
                                 const glm::mat4& view, const glm::mat4& projection,
                                 const glm::vec3& viewPos, const Texture* texture) {
    shader.use();
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);
    shader.setVec3("uViewPos", viewPos);

    if (texture) {
        shader.setInt("uTexture", 0);
        shader.setInt("uHasTexture", 1);
        texture->bind(0);
    } else {
        shader.setInt("uHasTexture", 0);
    }

    mesh.drawInstanced(instanceCount);
    framePolyCount += mesh.getTriangleCount() * instanceCount;
}
