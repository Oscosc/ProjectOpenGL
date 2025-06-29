#include "HUD.hpp"

HUD::HUD(const unsigned int screenWidth, const unsigned int screenHeight) : m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
    initCursor();
}

void HUD::initCursor()
{
    float cursorVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    glGenVertexArrays(1, &this->m_cursorVAO);
    glGenBuffers(1, &this->m_cursorVBO);

    glBindVertexArray(this->m_cursorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_cursorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cursorVertices), &cursorVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
}

void HUD::renderCursor()
{
    Shader* sh = ShaderManager::getInstance().getShader("quad");
    sh->use();
    sh->setVec2("screenSize", this->m_screenWidth, this->m_screenHeight);
    glBindVertexArray(this->m_cursorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
        std::cerr << "[OpenGL Error] Code: " << err << std::endl;
}

void HUD::render()
{
    renderCursor();
}
