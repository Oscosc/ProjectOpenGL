#include <ProjectIGAI/core/BaseWindow.hpp>
#include "BaseWindow.hpp"

BaseWindow::BaseWindow(const unsigned int width, const unsigned int height = DEFAULT_WINDOW_HEIGHT,
    const std::string& title, GLFWwindow* rootWindow)
{
    // Creating the OpenGL window object
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, rootWindow);
    glfwSetWindowUserPointer(m_window, this);
    if(!m_window) {
        Logger::logError("Could not create window instance");
        exit(-1);
    }

    // Switching to new context
    glfwMakeContextCurrent(m_window);
    // Creating local pointer
    glfwSetWindowUserPointer(m_window, this);

    initCallbacks();
}

void BaseWindow::initCallbacks()
{
    glfwSetKeyCallback(m_window, [](GLFWwindow* win, int key, int scancode, int action, int mods){
        if(auto* instance = static_cast<BaseWindow*>(glfwGetWindowUserPointer(win))) {
            instance->onKey(key, scancode, action, mods);
        }
    });

    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* win, int w, int h){
        if(auto* instance = static_cast<BaseWindow*>(glfwGetWindowUserPointer(win))) {
            instance->onResize(w, h);
        }
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* win, double x, double y){
        if(auto* instance = static_cast<BaseWindow*>(glfwGetWindowUserPointer(win))) {
            instance->onCursorPos(x, y);
        }
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* win, int button, int action, int mods){
        if(auto* instance = static_cast<BaseWindow*>(glfwGetWindowUserPointer(win))) {
            instance->onMouseButton(button, action, mods);
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* win, double xOffset, double yOffset){
        if(auto* instance = static_cast<BaseWindow*>(glfwGetWindowUserPointer(win))) {
            instance->onScroll(xOffset, yOffset);
        }
    });
}

void BaseWindow::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void BaseWindow::onKey(int key, int scancode, int action, int mods)
{
    // Exit app
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }
}