#pragma once

#include <iostream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Scene.hpp"
#include "Callbacks.hpp"
#include "ShaderManager.hpp"
#include "HUD.hpp"
#include "Mesh.hpp"
#include "Sphere.hpp"
#include "PointLight.hpp"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

class Application
{
public:
    Application();
    Application(const unsigned int screenWidth, const unsigned int screenHeight);
    ~Application() = default;

    void initWindow();
    void initGLComponents();
    void initCallbacks();
    void initShaders();
    void initScene(const std::string& file);
    void initHUD();
    void loop();

    void run(const std::string& sceneFile);

    const unsigned int getScreenWidth() { return m_screenWidth; }
    const unsigned int getScreenHeight() { return m_screenHeight; }

    // TODO : Passer la gestion de la souris dans une classe
    const bool isMouseActive() { return m_mouseActive; }
    void switchMouseActive() { m_mouseActive = !m_mouseActive; }
    const bool isFirstMouse() { return m_firstMouse; }
    void firstMouseDone() { m_firstMouse = false; }

    const glm::vec2 getCursor() { return m_cursor; }
    void setCursor(glm::vec2 pos) { m_cursor = pos; }
    void setCursor(float x, float y) { m_cursor = glm::vec2(x, y); }

    const float getDeltaTime() { return m_deltaTime; }
    void setDeltaTime(float value) { m_deltaTime = value; }

    Camera* getActiveCamera() { return this->m_scene->getActiveCamera(); }

    static Application* getApplicationFromWindow(GLFWwindow* window);

private:
    /* --- WINDOW --- */
    GLFWwindow* m_window;
    unsigned int m_screenWidth;
    unsigned int m_screenHeight;

    /* --- TIME --- */
    float m_deltaTime;
    float m_lastFrame;

    /* --- RENDER --- */
    HUD* m_HUD;
    Scene* m_scene;

    /* --- UTILS --- */
    bool m_mouseActive = true;
    bool m_firstMouse = true;
    glm::vec2 m_cursor;
};

