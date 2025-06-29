#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderManager.hpp"

class HUD
{
public:
    HUD(const unsigned int screenWidth, const unsigned int screenHidth);
    HUD() = default;
    ~HUD() = default;

    void initCursor();

    void renderCursor();
    void render();

private:
    unsigned int m_cursorVAO, m_cursorVBO;

    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
};