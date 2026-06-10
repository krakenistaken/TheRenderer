#pragma once
#include "Camera.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Shader.h"
#include "../Engine/Renderer.h"
#include "../Engine/Window.h"
#include <glm/glm.hpp>
#include <vector>

class Texture;
struct InstanceGroup {
    Mesh* mesh = nullptr;
    Texture* texture = nullptr;
    std::vector<glm::mat4> transforms;
};

class Scene {
public:
    Scene();

    void clear();
    void addInstance(Mesh* mesh, Texture* texture, const glm::mat4& transform);
    void prepareInstancing();
    void update(const Window& window, float deltaTime);
    void render(Renderer& renderer, Shader& meshShader, float aspectRatio);

    Camera& getCamera() { return camera; }
    uint32_t getTotalPolyCount() const;

private:
    Camera camera;
    std::vector<InstanceGroup> groups;
};
