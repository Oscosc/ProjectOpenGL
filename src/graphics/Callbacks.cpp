#include <ProjectIGAI/graphics/Callbacks.hpp>

#include <ProjectIGAI/core/RaytracingWindow.hpp>

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

    // Open a window for ray-tracing
    if(key == GLFW_KEY_R && action == GLFW_PRESS) {
        app->createExternalWindow(800, 600, "Ray-tracing");
    }

    /*
    // Switch surface displaying mode
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        app->setDisplayMode(STANDARD_DISPLAY_MODE);
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        app->setDisplayMode(NORMAL_DISPLAY_MODE);
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        app->setDisplayMode(UV_DISPLAY_MODE);
    }
    */

    /*
    // Remove all casted rays
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        app->clearRays();
    }
    */

    // Exit app
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    /*
    // Capture screen with ray tracing
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        std::string captureName = "screen_capture.png";
        Intersection::raySavePNG(*app, captureName);
    }
    */

    /*
    // Switch to next element in context
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        app->getActiveAsObject()->setAmbient(0.2f);                     // On repasse le precedent en faible lumiere
        app->nextObject();                                              // On passe à l'objet suivant
        app->getActiveAsObject()->setAmbient(1.0f);                     // On passe le nouveau en haute luminosité

        std::cout << "Current item : " << app->getActiveIndex() + 1 << "/" << app->size() << std::endl;
    }

    // Switch to previous element in context
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        app->getActiveAsObject()->setAmbient(0.2f);                     // On repasse le precedent en faible lumiere
        app->previousObject();                                          // On passe à l'objet suivant
        app->getActiveAsObject()->setAmbient(1.0f);                     // On passe le nouveau en haute luminosité

        std::cout << "Current item : " << app->getActiveIndex() + 1 << "/" << app->size() << std::endl;
    }
    */

    /*
    // ONLY SCALABLE ELEMENTS PART ----------------------------------------------------------------
    ScalableElement* activeElement = app->getActiveAsScalable();
    if(!activeElement) return;
    // --------------------------------------------------------------------------------------------

    // Update number of curve points in BezierCurve
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        activeElement->next();
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        activeElement->previous();
    }
    // Update type of curve points in BezierCurve
    if (key == GLFW_KEY_SEMICOLON && action == GLFW_PRESS) {
        std::cout << "Switching Mode !" << std::endl;
        activeElement->switchMode();
    }
    */
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

    // If a movement has been recorded, ray-tracers needs to be reset
    if(movement) {
        for(unsigned int i = 1; i < app->getActiveWindowCount(); i++) {
            RaytracingWindow* rtWin = dynamic_cast<RaytracingWindow*>(app->getExternalWindow(i));
            if(rtWin != nullptr)
                rtWin->getRayTracer()->resetAccumulation();
        }
    }
}