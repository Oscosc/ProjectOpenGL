#include "callbacks.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if(!context) {
        std::cout << "Erreur d'initialisation context dans mouse_callack()" << std::endl;
        return;
    }

    if(context->isMouseActive()) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (context->isFirstMouse())
    {
        context->setCursorX(xpos);
        context->setCursorY(ypos);
        context->firstMouseDone();
    }

    float xoffset = xpos - context->getCursorX();
    float yoffset = context->getCursorY() - ypos; // reversed since y-coordinates go from bottom to top

    context->setCursorX(xpos);
    context->setCursorY(ypos);

    context->getCamera()->ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if(!context) {
        std::cout << "Erreur d'initialisation context dans scroll_callack()" << std::endl;
        return;
    }

    context->getCamera()->ProcessMouseScroll(static_cast<float>(yoffset));
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Récupération du contexte
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if(!context) {
        std::cout << "Erreur d'initialisation context dans key_callack()" << std::endl;
        return;
    }

    // Switch cursor visiblity and mode for ray casting
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        if(context->isMouseActive()) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        context->switchMouseActive();
    }

    // Remove all casted rays
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        context->removeRays();
    }

    // Exit app
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Capture screen with ray tracing
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        std::string captureName = "screen_capture.png";
        Intersection::raySavePNG(*context, captureName);
    }

    // Switch to next element in context
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        context->getActiveAsObject()->setColor(glm::vec3(1.0f));            // On repasse le precedent en blanc
        context->nextObject();                                              // On passe à l'objet suivant
        context->getActiveAsObject()->setColor(glm::vec3(0.f, 1.0f, 0.f));  // On passe le nouveau en vert
        std::cout << "Current item : " << context->getActiveIndex() << "/" << context->size() << std::endl;
    }

    // Switch to previous element in context
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        context->getActiveAsObject()->setColor(glm::vec3(1.0f));            // On repasse le precedent en blanc
        context->previousObject();                                          // On passe à l'objet suivant
        context->getActiveAsObject()->setColor(glm::vec3(0.f, 1.0f, 0.f));  // On passe le nouveau en vert
        std::cout << "Current item : " << context->getActiveIndex() << "/" << context->size() << std::endl;
    }

    // ONLY SCALABLE ELEMENTS PART ----------------------------------------------------------------
    ScalableElement* activeElement = context->getActiveAsScalable();
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
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Récupération du contexte
        AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
        if(!context) {
            std::cout << "Erreur d'initialisation context dans mouse_button_callack()" << std::endl;
            return;
        }

        // Récupération du point cliqué
        double mouseX, mouseY;
        if(context->isMouseActive()) {glfwGetCursorPos(window, &mouseX, &mouseY);}
        else {mouseX = context->SCR_WIDTH/2.0f; mouseY = context->SCR_HEIGHT/2.0f;}

        // Calcul du rayon initial
        Ray original;
        Intersection::cameraRay(*context, mouseX, mouseY, original);
        
        // Calcul d'intersections
        ptsTab intersections;
        glm::vec3 reflexion;
        Intersection::rayContextPath(*context, original, intersections, reflexion);
        context->addObject(std::make_unique<Ray>(original.getOrigin(), original.getDirection(), intersections, reflexion));
    }
}

void processInput(GLFWwindow *window)
{

    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if(!context) {
        std::cout << "Erreur d'initialisation context dans processInput()" << std::endl;
        return;
    }

    if(context->isMouseActive()) return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        context->getCamera()->ProcessKeyboard(FORWARD, context->getDeltaTime());
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        context->getCamera()->ProcessKeyboard(BACKWARD, context->getDeltaTime());
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        context->getCamera()->ProcessKeyboard(LEFT, context->getDeltaTime());
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        context->getCamera()->ProcessKeyboard(RIGHT, context->getDeltaTime());
}