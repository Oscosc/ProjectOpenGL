#include <ProjectIGAI/core/BaseWindow.hpp>

BaseWindow::BaseWindow(Scene* refScene, const unsigned int width, const unsigned int height,
    const std::string& title, GLFWwindow* rootWindow) :
    m_scene(refScene), m_mouseActive(true), m_firstMouse(true), m_screenWidth(width), m_screenHeight(height)
{
    // Setting up root status
    m_isRoot = rootWindow == NULL;

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

    // Init callback for this window
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

void BaseWindow::render()
{
    // Making current window the active one
    glfwMakeContextCurrent(this->getGLFWwindow());

    // Updating frame time
    float currentFrame = static_cast<float>(glfwGetTime());
    this->m_deltaTime = currentFrame - this->m_lastFrame;
    this->m_lastFrame = currentFrame;

    // Clearing buffer before drawing
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calling window-specific rendering logic
    subClassRendering();

    // Swaping buffers to render new frame
    glfwSwapBuffers(this->getGLFWwindow());
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

    // ----------------------------------------------------
    // ROOTS COMMANDS ONLY
    // ----------------------------------------------------
    if(!isRoot()) return;

    // Switch mouse status
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        if(isMouseActive()) glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        switchMouseActive();
    }
}

void BaseWindow::onScroll(double xOffset, double yOffset)
{
    // ----------------------------------------------------
    // ROOTS COMMANDS ONLY
    // ----------------------------------------------------
    if(!isRoot()) return;

    this->getScene()->getActiveCamera()->ProcessMouseScroll(static_cast<float>(yOffset));
}

void BaseWindow::onCursorPos(double xPos, double yPos)
{
    // ----------------------------------------------------
    // ROOTS COMMANDS ONLY
    // ----------------------------------------------------
    if(!isRoot()) return;

    if(this->isMouseActive()) return;

    float xpos = static_cast<float>(xPos);
    float ypos = static_cast<float>(yPos);

    if (this->isFirstMouse())
    {
        this->setCursor(xpos, ypos);
        this->firstMouseDone();
    }

    float xoffset = xpos - this->getCursor().x;
    float yoffset = this->getCursor().y - ypos; // reversed since y-coordinates go from bottom to top

    this->setCursor(xpos, ypos);

    this->getScene()->getActiveCamera()->ProcessMouseMovement(xoffset, yoffset);
}
