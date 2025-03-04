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


#include "callbacks.hpp" // Contient GLFW et glad : à appeler en premier
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
#include "Ray.hpp"
#include "Sphere.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

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
 * @brief Traite les inputs "longs" du clavier.
 * 
 * Traite les entrées claviers de type ZQSD qui doivent ête effectuées à chaque frame pour donner
 * un effet "lisse" dans les déplacements par exemple.
 * @param window Fenêtre à laquelle on veut assigner le traitement des saisies clavier.
 */
void processInput(GLFWwindow *window);


// camera
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

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
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Projet IGAI", NULL, NULL);
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
    AppContext contextIGAI(SCREEN_WIDTH, SCREEN_HEIGHT);
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
        contextIGAI.setProjection(glm::perspective(glm::radians(contextIGAI.getCamera()->Zoom),
            (float)contextIGAI.SCR_WIDTH / (float)contextIGAI.SCR_HEIGHT, 0.1f, 100.0f));
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
        crosshairShader.setVec2("screenSize", contextIGAI.SCR_WIDTH, contextIGAI.SCR_HEIGHT);
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

    if(context->isMouseActive()) return;

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