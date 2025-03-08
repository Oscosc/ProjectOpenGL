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

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    ScalableElement* activeElement = context->getActiveObject();
    if(!activeElement) return;

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

        // Calcul des valeurs du rayon lancé
        float x = (2.0f * mouseX) / context->SCR_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / context->SCR_HEIGHT;
        glm::vec4 rayClip(x, y, -1.0f, 1.0f);

        glm::vec4 rayEye = glm::inverse(context->getProjection()) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        glm::vec4 rayWorld = glm::inverse(context->getView()) * rayEye;

        glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));
        glm::vec3 nearPoint = context->getCamera()->Position + rayDir * 1.0f; // 1.0 pour être un peu éloigné de l'écran

        // Ajout du rayon à l'affichage
        context->addObject(std::make_unique<Ray>(nearPoint, rayDir));

        // Temporaire : calcul d'intersection
        Ray tmp(nearPoint, rayDir);
        float a = 0.0f;
        glm::vec3 new_dir;

        Sphere* mySphere = dynamic_cast<Sphere*>(context->getObject(0));
        if(tmp.intersect(*mySphere, a, new_dir)) {
            std::cout << "INTERSECTION en " << glm::to_string(tmp.getPoint(a)) << std::endl;
            std::cout << "-> REFLEXION en " << glm::to_string(new_dir) << std::endl;
        }
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