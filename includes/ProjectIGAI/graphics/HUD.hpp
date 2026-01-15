#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ProjectIGAI/graphics/ShaderManager.hpp>

/**
 * @brief This class contain functions to render a custom HUD.
 * All HUD elements have to be added to this class with a init() and render() method.
 */
class HUD
{
public:

    /**
     * @brief Create an HUD object to be rendered in an application.
     * 
     * @param screenWidth width of the calling application window in pixels
     * @param screenHeight height of the calling application window in pixels
     */
    HUD(const unsigned int screenWidth, const unsigned int screenHeight);

    /**
     * @brief Default destructor.
     */
    ~HUD() = default;

    /**
     * @brief Construct the cursor (quad) sub-object of HUD class
     */
    void initCursor();

    /**
     * @brief Render the cursor (quad) sub-object of HUD class
     */
    void renderCursor() const;

    /**
     * @brief Render all HUD components with functions "renderNameOfComponent()".
     * All the shaders used in HUD must be integrated first using the ShaderManager.
     */
    void render() const;

    void setScreenSize(const unsigned int w, const unsigned int h);

private:
    unsigned int m_cursorVAO, m_cursorVBO;

    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
};