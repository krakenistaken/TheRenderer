#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window() = default;
    ~Window();

    bool init(int width, int height, const char* title);
    void shutdown();
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    bool isKeyPressed(int key) const;
    bool isMouseButtonPressed(int button) const;
    void getMousePos(double& x, double& y) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    GLFWwindow* getHandle() const { return handle; }

private:
    GLFWwindow* handle = nullptr;
    int width = 0, height = 0;
    static void framebufferSizeCallback(GLFWwindow* window, int w, int h);
};
