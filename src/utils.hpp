#ifndef UTILS_HPP
#define UTILS_HPP

/**
 * @file utils.hpp
 * @brief Fonctions utilitaires usuelles.
 * 
 * Ce fichier contient des fonctions utilitaires usuelles pour l'ensemble du programme.
 * 
 * @author Oscar G.
 * @date 2025-03-01
 */

#include <vector>
#include <glm/glm.hpp>

/**
 * @brief Renvoie la valeur du triangle de Pascal pour un i et un n donnés en paramètre.
 */
int PascalValue(int i, int n);

/**
 * @brief Renvoie la liste des entiers de 0 (inclus) à n (exclus) à la mnière de la fonction
 * range() en Python.
 */
std::vector<unsigned int> Range(unsigned int n);

/**
 * @brief Combine les deux vecteurs A et B passés en paramètre dans un nouveau vecteur qui est
 * renvoyé par la fonction. Cette fonction garantit donc de ne pas modifier A et B.
 */
std::vector<glm::vec3> combine(std::vector<glm::vec3> vecA, std::vector<glm::vec3> vecB);
std::vector<unsigned int> combine(std::vector<unsigned int> vecA, std::vector<unsigned int> vecB);

/**
 * @brief Fonction récupérée ici :
 * https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
 * 
 * Calcule les racines d'un polynome du second debré si elles existent et les stocke dans x0 et x1 (x1 > x0).
 * @param a, b, c coefficient du polynome
 * @param x0, x1 entiers où écire les racines du polynome
 * @return true si des racines ont été trouvées, false sinon
 */
bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);


#endif // UTILS_HPP