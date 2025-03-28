#ifndef OBJECT_HPP
#define OBJECT_HPP

/**
 * @file Object.hpp
 * @brief Définition de la classe abstraite Object.
 * 
 * Ce fichier contient la définition d'un objet affichable au sens d'OpenGL.
 * 
 * @author Oscar G.
 * @date 2025-03-01
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../includes/shader.hpp"

#define OBJECT_AMBIENT_STRENGTH 0.2


typedef struct s_Triangle {
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
} Triangle;


using ptsTab = std::vector<glm::vec3>;


/**
 * @class Object
 * @brief Classe abstraite pour les objets qui doivent être rendus en OpenGL.
 * 
 * Pour chaque objet, on définit un VAO et un VBO qui serviront au rendu OpenGL.
 * Chaque classe dérivée doit implémenter la fonction "draw()" qui dessine l'objet à l'écran selon
 * les spécificités de rendu de cet objet (type de traits, prétraitement, etc).
 */
class Object
{
public:

    /**
     * @brief Constructeur par défaut de la classe Object (ne peut pas être appelé sauf par une
     * classe dérivée de celle-ci).
     */
    Object(bool enableNormal = true);

    /**
     * @brief Destructeur par défaut de la classe Object.
     */
    virtual ~Object();

    /**
     * @brief Fonction à redéfinir pour chaque classe dérivée.
     * 
     * Cette fonction est supposée s'appuyer sur les fonctions OpenGL de rendu pour dessiner
     * l'objet en question à l'écran.
     */
    virtual void draw(Shader shader) = 0;

    glm::vec3 getOrigin() const;
    void setOrigin(glm::vec3 value);
    glm::vec3 getColor() const;
    void setColor(glm::vec3 value);
    float getAmbient() const;
    void setAmbient(float value);

    const std::vector<Triangle>* getTriangles();
    // virtual void setTriangles() = 0;

protected:

    GLuint VAO, VBO;
    glm::vec3 m_origin;
    glm::vec3 m_color;
    float m_ambient;

    //std::vector<Triangle> m_triangles; TODO

    /**
     * @brief Mets à jour le VBO et le VAO avec les nouvelles données en paramètre.
     * @param points Liste des nouveaux points qui seront stockées dans le buffer GPU.
     */
    void updateVertices(ptsTab points);
};

#endif // OBJECT_HPP