#include <ProjectIGAI/graphics/Callbacks.hpp>

#include <extern/imgui/imgui.h>

void Callbacks::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void Callbacks::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    Application* app = Application::getApplicationFromWindow(window);
    if(app->isMouseActive()) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (app->isFirstMouse())
    {
        app->setCursor(xpos, ypos);
        app->firstMouseDone();
    }

    float xoffset = xpos - app->getCursor().x;
    float yoffset = app->getCursor().y - ypos; // reversed since y-coordinates go from bottom to top

    app->setCursor(xpos, ypos);

    app->getActiveCamera()->ProcessMouseMovement(xoffset, yoffset);
}


void Callbacks::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Application* app = Application::getApplicationFromWindow(window);
    if(app->isMouseActive()) return;

    app->getActiveCamera()->ProcessMouseScroll(static_cast<float>(yoffset));
}


void Callbacks::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Récupération du contexte
    Application* app = Application::getApplicationFromWindow(window);

    // Switch cursor visiblity and mode for ray casting
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        if(app->isMouseActive()) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        app->switchMouseActive();
    }

    // Exit app
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Callbacks::processInput(Application *app)
{
    // Retrieve main window to check for inputs
    BaseWindow* mainWindow = app->getMainWindow();

    bool movement = false;
    if (glfwGetKey(mainWindow->getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS) {
        mainWindow->getScene()->getActiveCamera()->ProcessKeyboard(FORWARD, mainWindow->getDeltaTime());
        movement = true;
    }
    if (glfwGetKey(mainWindow->getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS) {
        mainWindow->getScene()->getActiveCamera()->ProcessKeyboard(BACKWARD, mainWindow->getDeltaTime());
        movement = true;
    }
    if (glfwGetKey(mainWindow->getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS) {
        mainWindow->getScene()->getActiveCamera()->ProcessKeyboard(LEFT, mainWindow->getDeltaTime());
        movement = true;
    }
    if (glfwGetKey(mainWindow->getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS) {
        mainWindow->getScene()->getActiveCamera()->ProcessKeyboard(RIGHT, mainWindow->getDeltaTime());
        movement = true;
    }
}