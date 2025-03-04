/**
 * @file main.cpp
 * @brief Point d'entrée de l'application ProjetIGAI.
 * 
 * Ce fichier est le point d'entrée de l'application ProjetIGAI.
 * Il contient les fonctions de callback utiles à GLFW pour interpréter les saisies utilisateur et
 * les appels de rafraichissement d'écran.
 * 
 * @author Oscar G.
 * @date 2025-03-02
 */

// Standard imports
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Transformation imports
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// LearnOpenGL imports
#include "../includes/shader.hpp"
#include "../includes/camera.hpp"

// Custom classes imports
#include "BezierCurve.hpp"
#include "AppContext.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"

/**
 * @brief Frame-buffer size callback.
 * 
 * Fonction de callback pour que l'utilisateur puisse redimensionner dynamiquement la fenêtre.
 * @param window Fenêtre à laquelle on veut assigner le callback.
 * @param width Nouvelle largeur de la fenêtre.
 * @param height Nouvelle hauteur de la fenêtre.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/**
 * @brief Mouse callback.
 * 
 * Fonction de callback pour traiter les mouvements du curseur et déplacer la vue dans la scène 3D
 * en fonction de ce mouvement.
 * @param window Fenêtre à laquelle on veut assigner le callback.
 * @param xpos Nouvelle position X du curseur.
 * @param ypos Nouvelle position Y du curseur.
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/**
 * @brief Scroll callback.
 * 
 * Fonction de callback pour traiter le scroll de l'utilisateur et zoomer/dezoomer la camera.
 * @param window Fenêtre à laquelle on veut assigner le callback.
 * @param xoffset Mouvement en X du scroll.
 * @param yoffset Mouvement en Y du scroll.
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/**
 * @brief Key callback.
 * 
 * Fonction de callback pour les touches du clavier. Les touches du clavier actuellement traitées
 * sont :
 * - Flèche haut (comportement spécifique aux courbes de Bézier)
 * - Flèche bas (comportement spécifique aux courbes de Bézier)
 * - Tab (bascule du mode "curseur" au mode "souris")
 * - M (comportement spécifique aux courbes de Bézier)
 * @param window Fenêtre à laquelle on veut assigner le callback.
 * @param key Identifiant de la touche qui déclenche le callback.
 * @param scancode Scancode de la touche qui déclenche le callback.
 * @param action Type d'action effectuée sur la touche (pressée, relachée, etc).
 * @param mods Eventuels modifications sur la touche (si ALT est maintenu avec par exemple)
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

/**
 * @brief Traite les inputs "longs" du clavier.
 * 
 * Traite les entrées claviers de type ZQSD qui doivent ête effectuées à chaque frame pour donner
 * un effet "lisse" dans les déplacements par exemple.
 * @param window Fenêtre à laquelle on veut assigner le traitement des saisies clavier.
 */
void processInput(GLFWwindow *window);


// frame settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseActive = false;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Projet IGAI", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glfw custom user pointer
    // ------------------------
    AppContext contextIGAI;
    glfwSetWindowUserPointer(window, &contextIGAI);

    // Mouse capture config
    // --------------------
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    // build and compile our shader program
    // ------------------------------------
    Shader bezierShader("shaders/bezier.vs", "shaders/bezier.fs");


    // Creating Bezier Curve
    // ---------------------
    ptsTab controlPolygon = {
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, 0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f}
    };
    contextIGAI.addObject(std::make_unique<BezierCurve>(controlPolygon));
    
    // Creating sphere
    contextIGAI.addObject(std::make_unique<Sphere>(1.0f));

    // crosshair setup
    // ---------------
    Shader crosshairShader("shaders/quad.vs", "shaders/quad.fs");

    float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };
    
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    // render loop  
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // active shader
        bezierShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        contextIGAI.setProjection(glm::perspective(glm::radians(contextIGAI.getCamera()->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));
        bezierShader.setMat4("projection", contextIGAI.getProjection());

        // camera/view transformation
        contextIGAI.setView(contextIGAI.getCamera()->GetViewMatrix());
        bezierShader.setMat4("view", contextIGAI.getView());

        // draw context elements
        // ---------------------
        // std::cout << "Elements dans le contexte : " << meshes.size() << std::endl;
        for(const auto& element : contextIGAI) {
            glm::mat4 model = glm::mat4(1.0f);
            bezierShader.setMat4("model", model);

            element->draw(bezierShader);
        }


        // draw crosshair
        // --------------
        crosshairShader.use();
        crosshairShader.setVec2("screenSize", SCR_WIDTH, SCR_HEIGHT);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window)
{
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if(!context) return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        context->getCamera()->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        context->getCamera()->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        context->getCamera()->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        context->getCamera()->ProcessKeyboard(RIGHT, deltaTime);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if(!context) return;

    if(mouseActive) {
        return;
    }
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    context->getCamera()->ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if(!context) return;

    context->getCamera()->ProcessMouseScroll(static_cast<float>(yoffset));
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Récupération du contexte
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if(!context) return;

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

    // Switch cursor visiblity and mode for ray casting
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        if(mouseActive) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if(!mouseActive) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        mouseActive = !mouseActive;
    }

    // Remove all casted rays
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        context->removeRays();
    }
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Récupération du contexte
        AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
        if(!context) return;

        // Récupération du point cliqué
        double mouseX, mouseY;
        if(mouseActive) {glfwGetCursorPos(window, &mouseX, &mouseY);}
        else {mouseX = SCR_WIDTH/2.0f; mouseY = SCR_HEIGHT/2.0f;}

        // Calcul des valeurs du rayon lancé
        float x = (2.0f * mouseX) / SCR_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / SCR_HEIGHT;
        glm::vec4 rayClip(x, y, -1.0f, 1.0f);

        glm::vec4 rayEye = glm::inverse(context->getProjection()) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        glm::vec4 rayWorld = glm::inverse(context->getView()) * rayEye;
        glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));

        glm::vec3 nearPoint = context->getCamera()->Position + rayDir * 1.0f;
        glm::vec3 farPoint = context->getCamera()->Position + rayDir * 100.0f;

        // Ajout du rayon à l'affichage
        context->addObject(std::make_unique<Ray>(nearPoint, farPoint));
    }
}