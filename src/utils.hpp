#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <glm/glm.hpp>

int PascalValue(int i, int n);
std::vector<unsigned int> Range(unsigned int n);
std::vector<glm::vec3> combine(std::vector<glm::vec3> vecA, std::vector<glm::vec3> vecB);

#endif // UTILS_HPP