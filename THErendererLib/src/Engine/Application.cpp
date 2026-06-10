#include "Application.h"
#include <cstdio>
#include <algorithm>

bool Application::run(int width, int height, const char* title) {
    // --- Initialize window ---
    if (!window.init(width, height, title)) {
        fprintf(stderr, "Fatal: Window initialization failed\n");
        return false;
    }

    printf("OpenGL %s | %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

    // --- Initialize renderer ---
    renderer.init();

    // --- Load default mesh shader ---
    if (!meshShader.loadFromFile("shaders/mesh.vert", "shaders/mesh.frag")) {
        fprintf(stderr, "Fatal: Failed to load mesh shaders\n");
        return false;
    }

    // --- Setup HUD ---
    if (!hud.init(window.getWidth(), window.getHeight())) {
        fprintf(stderr, "Fatal: HUD initialization failed\n");
        return false;
    }

    // --- User initialization ---
    onInit();

    // --- Main loop ---
    timer.init();

    while (!window.shouldClose()) {
        timer.tick();
        window.pollEvents();

        if (window.isKeyPressed(GLFW_KEY_ESCAPE))
            break;

        // User update
        onUpdate(timer.getDeltaTime());

        // Update scene camera
        scene.update(window, timer.getDeltaTime());
        hud.updateScreenSize(window.getWidth(), window.getHeight());

        // Render
        renderer.beginFrame();
        float aspect = getAspectRatio();
        scene.render(renderer, meshShader, aspect);

        // User custom rendering
        onRender();

        window.swapBuffers();
    }

    onShutdown();
    return true;
}

float Application::getAspectRatio() const {
    return (float)window.getWidth() / std::max((float)window.getHeight(), 1.0f);
}
