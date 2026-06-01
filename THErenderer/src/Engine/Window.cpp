#include "Window.h"
#include <cstdio>

bool Window::init(int w, int h, const char* title) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    handle = glfwCreateWindow(w, h, title, nullptr, nullptr);
    if (!handle) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    width = w;
    height = h;
    glfwMakeContextCurrent(handle);
    glfwSetWindowUserPointer(handle, this);
    glfwSetFramebufferSizeCallback(handle, framebufferSizeCallback);
    glfwSwapInterval(0);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(err));
        return false;
    }

    return true;
}

void Window::shutdown() {
    if (handle) {
        glfwDestroyWindow(handle);
        handle = nullptr;
    }
    glfwTerminate();
}

Window::~Window() {
    shutdown();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle);
}

void Window::swapBuffers() {
    glfwSwapBuffers(handle);
}

void Window::pollEvents() {
    glfwPollEvents();
}

bool Window::isKeyPressed(int key) const {
    return glfwGetKey(handle, key) == GLFW_PRESS;
}

bool Window::isMouseButtonPressed(int button) const {
    return glfwGetMouseButton(handle, button) == GLFW_PRESS;
}

void Window::getMousePos(double& x, double& y) const {
    glfwGetCursorPos(handle, &x, &y);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int w, int h) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->width = w;
    self->height = h;
    glViewport(0, 0, w, h);
}
