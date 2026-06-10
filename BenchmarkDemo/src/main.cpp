#include <THErenderer.h>
#include <cstdio>
#include <random>
#include <vector>

class BenchmarkDemo : public Application {
public:
    // Assets
    Mesh torusMesh, houseMesh, treeMesh;
    Texture checkerTex, houseTex, treeTex;

    int currentScene = 1;
    bool wasLeftMousePressed = false;

    static constexpr int SCENE_COUNT = 29;
    static constexpr int sceneCounts[SCENE_COUNT + 1] = {
        0, 250, 1000, 1500, 2000, 3000, 4000, 5000, 7500, 10000,
        15000, 20000, 30000, 50000, 75000, 100000, 125000, 150000,
        175000, 200000, 250000, 300000, 350000, 400000, 450000,
        500000, 750000, 1000000, 1500000, 10000
    };

    void buildTorusScene(int numObjects) {
        scene.clear();
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
            float scale = distScale(rng);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, candidate);
            glm::vec3 rotAxis = glm::normalize(glm::vec3(axisDist(rng), axisDist(rng), axisDist(rng)));
            if (glm::length(rotAxis) < 0.01f) rotAxis = glm::vec3(0,1,0);
            model = glm::rotate(model, glm::radians(distRot(rng)), rotAxis);
            model = glm::scale(model, glm::vec3(scale));
            scene.addInstance(&torusMesh, &checkerTex, model);
        }
        scene.prepareInstancing();
        printf("Loaded Torus Scene: %d objects.\n", numObjects);
    }

    void buildVillageScene(int totalObjects) {
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
            scene.addInstance(&houseMesh, &houseTex, model);
        }

        for (int i = 0; i < treeCount; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(distPos(rng), 0.0f, distPos(rng)));
            model = glm::scale(model, glm::vec3(1.5f));
            scene.addInstance(&treeMesh, &treeTex, model);
        }

        scene.prepareInstancing();
        printf("Loaded Village Scene: %d houses, %d trees.\n", houseCount, treeCount);
    }

    void loadScene(int idx) {
        if (idx >= 1 && idx <= 28) {
            buildTorusScene(sceneCounts[idx]);
        } else if (idx == 29) {
            buildVillageScene(sceneCounts[idx]);
        }
    }

    void onInit() override {
        // Load torus
        OBJLoader::generateTorus("assets/torus.obj", 48, 24, 1.0f, 0.4f);
        std::vector<Vertex> verts;
        std::vector<uint32_t> inds;
        OBJLoader::load("assets/torus.obj", verts, inds);
        torusMesh.upload(verts, inds);

        // Load house and tree
        std::vector<Vertex> hv, tv;
        std::vector<uint32_t> hi, ti;
        OBJLoader::load("assets/house.obj", hv, hi);
        OBJLoader::load("assets/tree.obj", tv, ti);
        houseMesh.upload(hv, hi);
        treeMesh.upload(tv, ti);

        // Load textures
        checkerTex.loadFromFile("assets/texture.png");
        houseTex.loadFromFile("assets/house.png");
        treeTex.loadFromFile("assets/tree.png");

        // Load first scene
        loadScene(currentScene);
    }

    void onUpdate(float dt) override {
        // Scene selector via mouse click
        bool isPressed = window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
        if (isPressed && !wasLeftMousePressed) {
            double mx, my;
            window.getMousePos(mx, my);
            int viewY = static_cast<int>(my);
            int viewX = static_cast<int>(mx);

            int w = window.getWidth();
            int startX = w - 420;
            if (viewY >= 10 && viewY <= 50) {
                if (viewX >= startX - 10 && viewX <= startX + 40) {
                    if (currentScene > 1) {
                        currentScene--;
                        loadScene(currentScene);
                    }
                } else if (viewX >= startX + 360 && viewX <= startX + 410) {
                    if (currentScene < SCENE_COUNT) {
                        currentScene++;
                        loadScene(currentScene);
                    }
                }
            }
        }
        wasLeftMousePressed = isPressed;
    }

    void onRender() override {
        hud.beginRender();

        // Render FPS and poly count
        char buf[64];
        snprintf(buf, sizeof(buf), "FPS: %.0f", getFPS());
        hud.renderText(buf, 12.0f, 12.0f, 2.5f, glm::vec3(0.1f, 1.0f, 0.4f));

        snprintf(buf, sizeof(buf), "Polys: %u", renderer.getFramePolyCount());
        hud.renderText(buf, 12.0f, 38.0f, 2.5f, glm::vec3(0.1f, 1.0f, 0.4f));

        // Render scene selector UI on top
        float rightEdge = (float)getWidth();
        float startX = rightEdge - 420.0f;

        hud.renderText("<", startX, 20.0f, 3.0f, glm::vec3(1.0f, 0.5f, 0.5f));

        char sceneText[64];
        int count = sceneCounts[currentScene];
        snprintf(sceneText, sizeof(sceneText), "SCENE %d (%d objs)", currentScene, count);
        hud.renderText(sceneText, startX + 50.0f, 20.0f, 2.0f, glm::vec3(1.0f));

        hud.renderText(">", startX + 370.0f, 20.0f, 3.0f, glm::vec3(1.0f, 0.5f, 0.5f));

        hud.endRender();
    }
};

int main() {
    BenchmarkDemo app;
    return app.run(1280, 720, "THErenderer - Benchmark Demo") ? 0 : -1;
}
