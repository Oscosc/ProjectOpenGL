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

typedef struct planet_t {
    Sphere object;
    float position;
    float speed;
    float range;
} Planet;


int StarsMain()
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
        glm::vec3(1.f, 1.0f, 0.0f)
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


    // Creating planets
    // ----------------
    Sphere sun(2.f, glm::vec3(0.f), glm::vec3(1.f, 1.f, 0.f));
    Planet planetA = {
        Sphere(1.0f),   // object
        0.0f,           // position
        0.01f,           // speed
        4.f,            // range
    };

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

        // active shader
        monochromeShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        contextIGAI.setProjection(glm::perspective(glm::radians(contextIGAI.getCamera()->Zoom),
            (float)contextIGAI.SCR_WIDTH / (float)contextIGAI.SCR_HEIGHT, 0.1f, 100.0f));
        monochromeShader.setMat4("projection", contextIGAI.getProjection());

        // camera/view transformation
        contextIGAI.setView(contextIGAI.getCamera()->GetViewMatrix());
        monochromeShader.setMat4("view", contextIGAI.getView());

        // lightPos and lightColor
        monochromeShader.setVec3("lightColor", contextIGAI.getLightColor());
        monochromeShader.setVec3("lightPos", {0.f, 0.f, 0.f});

        // Update planets positions
        // ------------------------
        planetA.position += planetA.speed;
        glm::vec3 realPos = glm::vec3(cosf(planetA.position), 0.f, sinf(planetA.position)) * planetA.range;
        planetA.object.setOrigin(realPos);

        // draw planets in scene
        // ---------------------
        sun.draw(monochromeShader);
        planetA.object.draw(monochromeShader);


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