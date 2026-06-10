#include "Scene.h"

Scene::Scene()
    : camera(glm::vec3(0.0f, 0.5f, 3.0f), -90.0f, -5.0f) {}

void Scene::addInstance(Mesh* mesh, Texture* texture, const glm::mat4& transform) {
    for (auto& group : groups) {
        if (group.mesh == mesh && group.texture == texture) {
            group.transforms.push_back(transform);
            return;
        }
    }
    InstanceGroup newGroup;
    newGroup.mesh = mesh;
    newGroup.texture = texture;
    newGroup.transforms.push_back(transform);
    groups.push_back(std::move(newGroup));
}

void Scene::clear() {
    groups.clear();
}

void Scene::prepareInstancing() {
    for (auto& group : groups) {
        if (group.mesh && !group.transforms.empty()) {
            group.mesh->setupInstancing(group.transforms);
        }
    }
}

void Scene::update(const Window& window, float deltaTime) {
    camera.processInput(window, deltaTime);
}

void Scene::render(Renderer& renderer, Shader& meshShader, float aspectRatio) {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
    glm::vec3 viewPos = camera.getPosition();

    for (const auto& group : groups) {
        if (group.mesh && !group.transforms.empty()) {
            renderer.drawMeshInstanced(*group.mesh, meshShader, static_cast<uint32_t>(group.transforms.size()), view, projection, viewPos, group.texture);
        }
    }
}

uint32_t Scene::getTotalPolyCount() const {
    uint32_t count = 0;
    for (const auto& group : groups) {
        if (group.mesh) {
            count += group.mesh->getTriangleCount() * static_cast<uint32_t>(group.transforms.size());
        }
    }
    return count;
}
