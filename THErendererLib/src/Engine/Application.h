#pragma once
#include "Window.h"
#include "Renderer.h"
#include "Timer.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Texture.h"
#include "../Graphics/OBJLoader.h"
#include "../Scene/Scene.h"
#include "../UI/HUD.h"
#include <string>

class Application {
public:
    Application() = default;
    virtual ~Application() = default;

    // Run the application. Returns true on clean exit.
    bool run(int width, int height, const char* title);

protected:
    // Override these in your subclass:
    virtual void onInit() {}           // Called once after GL context is ready
    virtual void onUpdate(float dt) {} // Called every frame before rendering
    virtual void onRender() {}         // Called every frame after the scene is rendered (for custom drawing)
    virtual void onShutdown() {}       // Called before the window closes

    // Engine subsystems available to subclasses:
    Window   window;
    Renderer renderer;
    Timer    timer;
    Shader   meshShader;
    Scene    scene;
    HUD      hud;

    // Helpers
    float getAspectRatio() const;
    float getFPS() const { return timer.getFPS(); }
    float getDeltaTime() const { return timer.getDeltaTime(); }
    int   getWidth() const { return window.getWidth(); }
    int   getHeight() const { return window.getHeight(); }
};
