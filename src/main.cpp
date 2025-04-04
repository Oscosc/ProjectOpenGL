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
#include "callbacks.hpp"
#include "BezierCurve.hpp"
#include "BezierSurface.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define DISPERSION_RATE 2


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
    
    // glfw custom user pointer
    // ------------------------
    AppContext contextIGAI(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        glm::vec3(0.2f, 0.3f, 0.3f),
        glm::vec3(1.f, 1.0f, 1.0f)
    );
    glfwSetWindowUserPointer(window, &contextIGAI);
    
    // glfw callbacks setup
    // --------------------
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Mouse capture config
    // --------------------
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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
    Shader monochromeShader("shaders/lighted.vs", "shaders/lighted.fs");


    // Creating sphere
    srand(time(0));
    for(int i=0; i < 10; ++i) {
        glm::vec3 randVec3 = glm::vec3((float)(std::rand()) / (float)(std::rand()) * DISPERSION_RATE,
                                       (float)(std::rand()) / (float)(std::rand()) * DISPERSION_RATE,
                                       (float)(std::rand()) / (float)(std::rand()) * DISPERSION_RATE);
        float randRadius = (float)(std::rand()) / (float)(RAND_MAX);
        /* glm::vec3 randColor = glm::vec3((float)(std::rand()) / (float)(RAND_MAX) * DISPERSION_RATE,
                                       (float)(std::rand()) / (float)(RAND_MAX) * DISPERSION_RATE,
                                       (float)(std::rand()) / (float)(RAND_MAX) * DISPERSION_RATE); */
        contextIGAI.addObject(std::make_unique<Sphere>(randRadius, randVec3, glm::vec3(1.0f)));
    }

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
    contextIGAI.getObject(contextIGAI.size()-1)->setOrigin({-1.f, 0.5f, -0.5f});

    // Creating Bezier Surface
    // -----------------------
    /*
    std::vector<std::vector<glm::vec3>> controlPolygonSurface = {
        { {0.0f, 0.0f, 0.0f}, {2.0f, 1.99f, 0.0f}, {4.0f, 0.28f, 0.0f}, {6.0f, -1.96f, 0.0f}, {8.0f, -0.56f, 0.0f}, {10.0f, 1.88f, 0.0f} },
        { {0.0f, 0.0f, 2.0f}, {2.0f, 0.14f, 2.0f}, {4.0f, 0.02f, 2.0f}, {6.0f, -0.14f, 2.0f}, {8.0f, -0.04f, 2.0f}, {10.0f, 0.13f, 2.0f} },
        { {0.0f, 0.0f, 4.0f}, {2.0f, -1.98f, 4.0f}, {4.0f, -0.28f, 4.0f}, {6.0f, 1.94f, 4.0f}, {8.0f, 0.55f, 4.0f}, {10.0f, -1.86f, 4.0f} },
        { {0.0f, 0.0f, 6.0f}, {2.0f, -0.42f, 6.0f}, {4.0f, -0.06f, 6.0f}, {6.0f, 0.41f, 6.0f}, {8.0f, 0.12f, 6.0f}, {10.0f, -0.40f, 6.0f} },
        { {0.0f, 0.0f, 8.0f}, {2.0f, 1.92f, 8.0f}, {4.0f, 0.27f, 8.0f}, {6.0f, -1.88f, 8.0f}, {8.0f, -0.54f, 8.0f}, {10.0f, 1.80f, 8.0f} },
        { {0.0f, 0.0f, 10.0f}, {2.0f, 0.69f, 10.0f}, {4.0f, 0.10f, 10.0f}, {6.0f, -0.68f, 10.0f}, {8.0f, -0.19f, 10.0f}, {10.0f, 0.65f, 10.0f} }
    };
    */
    ptsGrid controlPolygonSurface = {
        {
            {-0.5f,  0.0f,  0.0f},
            {-0.5f,  1.0f,  0.0f},
            { 0.5f,  1.0f,  0.0f},
            { 0.5f,  0.0f,  0.0f}
        },
        {
            {-0.5f,  0.0f, -1.0f},
            {-0.5f,  1.0f, -1.0f},
            { 0.5f,  1.0f, -1.0f},
            { 0.5f,  0.0f, -1.0f}
        }
    };
    contextIGAI.addObject(std::make_unique<BezierSurface>(controlPolygonSurface));
    contextIGAI.getObject(contextIGAI.size()-1)->setOrigin({-3.f, 0.f, 0.f});

    // Creating Sphere
    // ---------------
    contextIGAI.addObject(std::make_unique<Sphere>(0.5f, glm::vec3(-5.f, 0.5f, 0.f), glm::vec3(1.0f)));

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
        contextIGAI.setDeltaTime(currentFrame - contextIGAI.getLastFrame());
        contextIGAI.setLastFrame(currentFrame);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glm::vec3 clColor = contextIGAI.getBackgroundColor();
        glClearColor(clColor.x, clColor.y, clColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw context elements
        // ---------------------
        contextIGAI.drawContext(monochromeShader);


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