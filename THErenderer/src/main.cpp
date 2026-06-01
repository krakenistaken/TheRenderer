#include "Engine/Window.h"
#include "Engine/Timer.h"
#include "Engine/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Graphics/OBJLoader.h"
#include "Graphics/Texture.h"
#include "Scene/Scene.h"
#include "UI/HUD.h"
#include <cstdio>
#include <random>
#include <vector>

void buildTorusScene(Scene& scene, Mesh* mesh, Texture* texture, int numObjects) {
    scene.clear();
    std::vector<glm::vec3> spawnedPositions;
    std::mt19937 rng(1337); 
    std::uniform_real_distribution<float> jitter(-1.0f, 1.0f);
    std::uniform_real_distribution<float> distRot(0.0f, 360.0f);
    std::uniform_real_distribution<float> distScale(0.6f, 1.2f);
    std::uniform_real_distribution<float> axisDist(-1.0f, 1.0f);

    int side = (int)std::ceil(std::cbrt(numObjects)); 
    float spacing = 4.0f;

    for (int i = 0; i < numObjects; i++) {
        int x = i % side;
        int y = (i / side) % side;
        int z = i / (side * side);

        glm::vec3 candidate(
            (x - side / 2.0f) * spacing + jitter(rng),
            (y - side / 2.0f) * spacing + jitter(rng),
            (z - side / 2.0f) * spacing + jitter(rng)
        );
        spawnedPositions.push_back(candidate);
        float scale = distScale(rng);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, candidate);
        glm::vec3 rotAxis = glm::normalize(glm::vec3(axisDist(rng), axisDist(rng), axisDist(rng)));
        if (glm::length(rotAxis) < 0.01f) rotAxis = glm::vec3(0,1,0);
        model = glm::rotate(model, glm::radians(distRot(rng)), rotAxis);
        model = glm::scale(model, glm::vec3(scale));
        scene.addInstance(mesh, texture, model);
    }
    scene.prepareInstancing();
    printf("Loaded Torus Scene: %d objects.\n", numObjects);
}

void buildVillageScene(Scene& scene, Mesh* houseMesh, Texture* houseTex, Mesh* treeMesh, Texture* treeTex, int totalObjects) {
    scene.clear();
    
    std::mt19937 rng(42); 
    std::uniform_real_distribution<float> distPos(-200.0f, 200.0f);
    std::uniform_real_distribution<float> distRot(0.0f, 360.0f);
    
    int houseCount = totalObjects / 2;
    int treeCount = totalObjects - houseCount;

    for (int i = 0; i < houseCount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(distPos(rng), 0.0f, distPos(rng)));
        model = glm::rotate(model, glm::radians(distRot(rng)), glm::vec3(0,1,0));
        scene.addInstance(houseMesh, houseTex, model);
    }
    
    for (int i = 0; i < treeCount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(distPos(rng), 0.0f, distPos(rng)));
        model = glm::scale(model, glm::vec3(1.5f));
        scene.addInstance(treeMesh, treeTex, model);
    }
    
    scene.prepareInstancing();
    printf("Loaded Village Scene: %d houses, %d trees.\n", houseCount, treeCount);
}

int main() {
    // --- Initialize window ---
    Window window;
    if (!window.init(1280, 720, "THE Renderer - OpenGL")) {
        fprintf(stderr, "Fatal: Window initialization failed\n");
        return -1;
    }

    printf("OpenGL %s | %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

    // --- Initialize renderer ---
    Renderer renderer;
    renderer.init();

    // --- Load mesh shader ---
    Shader meshShader;
    if (!meshShader.loadFromFile("shaders/mesh.vert", "shaders/mesh.frag")) {
        fprintf(stderr, "Fatal: Failed to load mesh shaders\n");
        return -1;
    }

    // --- Generate and load torus ---
    OBJLoader::generateTorus("assets/torus.obj", 48, 24, 1.0f, 0.4f);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    if (!OBJLoader::load("assets/torus.obj", vertices, indices)) return -1;
    Mesh mesh;
    mesh.upload(vertices, indices);

    // --- Load house and tree ---
    std::vector<Vertex> houseVerts, treeVerts;
    std::vector<uint32_t> houseInds, treeInds;
    OBJLoader::load("assets/house.obj", houseVerts, houseInds);
    OBJLoader::load("assets/tree.obj", treeVerts, treeInds);
    Mesh houseMesh, treeMesh;
    houseMesh.upload(houseVerts, houseInds);
    treeMesh.upload(treeVerts, treeInds);

    // --- Load textures ---
    Texture checkerTexture, houseTex, treeTex;
    checkerTexture.loadFromFile("assets/texture.png");
    houseTex.loadFromFile("assets/house.png");
    treeTex.loadFromFile("assets/tree.png");

    // --- Setup scene ---
    Scene scene;
    int currentScene = 1;
    const int sceneCounts[] = {0, 250, 1000, 1500, 2000, 3000, 4000, 5000, 7500, 10000, 15000, 20000, 30000, 50000, 75000, 100000, 125000, 150000, 175000, 200000, 250000, 300000, 350000, 400000, 450000, 500000, 750000, 1000000, 1500000, 10000};
    
    auto loadScene = [&](int sceneIdx) {
        if (sceneIdx >= 1 && sceneIdx <= 28) {
            buildTorusScene(scene, &mesh, &checkerTexture, sceneCounts[sceneIdx]);
        } else if (sceneIdx == 29) {
            buildVillageScene(scene, &houseMesh, &houseTex, &treeMesh, &treeTex, sceneCounts[sceneIdx]);
        }
    };
    
    loadScene(currentScene);

    // --- Setup HUD ---
    HUD hud;
    if (!hud.init(window.getWidth(), window.getHeight())) {
        fprintf(stderr, "Fatal: HUD initialization failed\n");
        return -1;
    }

    // --- Game loop ---
    Timer timer;
    timer.init();
    bool wasLeftMousePressed = false;

    while (!window.shouldClose()) {
        timer.tick();
        window.pollEvents();

        if (window.isKeyPressed(GLFW_KEY_ESCAPE))
            break;

        bool isPressed = window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
        if (isPressed && !wasLeftMousePressed) {
            double mx, my;
            window.getMousePos(mx, my);
            int viewY = static_cast<int>(my);
            int viewX = static_cast<int>(mx);

            // Hitboxes for HUD text
            int w = window.getWidth();
            int startX = w - 420;
            if (viewY >= 10 && viewY <= 50) {
                if (viewX >= startX - 10 && viewX <= startX + 40) { // Clicked <
                    if (currentScene > 1) {
                        currentScene--;
                        loadScene(currentScene);
                    }
                } else if (viewX >= startX + 360 && viewX <= startX + 410) { // Clicked >
                    if (currentScene < 29) {
                        currentScene++;
                        loadScene(currentScene);
                    }
                }
            }
        }
        wasLeftMousePressed = isPressed;

        // Update
        scene.update(window, timer.getDeltaTime());
        hud.updateScreenSize(window.getWidth(), window.getHeight());

        // Render
        renderer.beginFrame();
        float aspect = (float)window.getWidth() / std::max((float)window.getHeight(), 1.0f);
        scene.render(renderer, meshShader, aspect);
        hud.render(timer.getFPS(), renderer.getFramePolyCount(), currentScene);

        window.swapBuffers();
    }

    return 0;
}
