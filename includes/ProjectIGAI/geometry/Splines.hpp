#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Splines
{
    /**
     * @brief Renvoie la valeur du triangle de Pascal pour un i et un n donnés en paramètre.
     */
    int PascalValue(int i, int n);

    /**
     * @brief Calcule la valeur du polynôme de Bernstein au point u en tenant compte de n et i.
     * 
     * https://fr.wikipedia.org/wiki/Polyn%C3%B4me_de_Bernstein 
     */
    float bersteinValue(float u, int i, int n);

    namespace Bezier
    {
        glm::vec3 getValue(float u, std::vector<glm::vec3> controlPoints);

        glm::vec3 getValue(float u, float v, std::vector<std::vector<glm::vec3>> controlPoints);
    }
}