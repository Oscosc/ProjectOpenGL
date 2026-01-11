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
#include <string>
#include <glm/glm.hpp>

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

/**
 * @brief Raccourci une chaine de caractere qui contiendrai des espaces ou des tabulations inutiles
 * 
 * @param str chaîne a réduire
 * @return chaine sans espaces et tabulations inutiles
 */
std::string trim(const std::string& str);

/**
 * @brief Coupe une chaine de caractères en plusieurs sous-chaînes, selon un delimiter
 * 
 * @param s chaine a decouper
 * @param delimiter delimiter pour les différents segments
 * @return Liste de chaines découpées
 */
std::vector<std::string> split(const std::string& s, const std::string& delimiter);

/**
 * @brief Calcule la moyenne d'un vecteur d'éléments
 * 
 * @param values vecteur d'elements
 * @return moyenne
 */
unsigned int mean(const std::vector<unsigned int>& values);




/* PARTIE POUR LE RAY TRACING */
// TODO : Commentaires

void savePNG(
    const std::vector<unsigned char>& pixels,
    const unsigned int width,
    const unsigned int height,
    const std::string& filename
);

float lengthSquared(glm::vec3 v);

float randomFloat(float min = 0, float max = 1);

glm::vec3 randomUnitVec3(float min = -1, float max = 1);

glm::vec3 randomEmisphereVec3(const glm::vec3& normal);

glm::vec2 noise2D(const float& x, const float& y, const float &intensity = 1.f);

glm::vec3 reflectVec3(const glm::vec3& v, const glm::vec3& n);

glm::vec3 refractVec3(const glm::vec3& v, const glm::vec3& n, float etaCoeff);

float reflectance(const float& cosine, const float& refractionIndex);

#endif // UTILS_HPP