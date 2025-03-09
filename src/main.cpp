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
#include "Ray.hpp"
#include "Sphere.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define DISPERSION_RATE 1.5


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
    AppContext contextIGAI(SCREEN_WIDTH, SCREEN_HEIGHT);
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
    Shader bezierShader("shaders/bezier.vs", "shaders/bezier.fs");


    // Creating sphere
    srand(time(0));
    for(int i=0; i < 10; ++i) {
        glm::vec3 randVec3 = glm::vec3((float)(std::rand()) / (float)(std::rand()) * DISPERSION_RATE,
                                       (float)(std::rand()) / (float)(std::rand()) * DISPERSION_RATE,
                                       (float)(std::rand()) / (float)(std::rand()) * DISPERSION_RATE);
        float randRadius = (float)(std::rand()) / (float)(RAND_MAX);
        contextIGAI.addObject(std::make_unique<Sphere>(randRadius, randVec3));
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