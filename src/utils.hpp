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

#endif // UTILS_HPP