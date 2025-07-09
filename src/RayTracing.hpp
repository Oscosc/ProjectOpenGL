#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "../includes/camera.hpp"
#include "Ray.hpp"

class RayTracing
{
public:
    static void computeImage(const std::string& filename, Scene& scene);
    static glm::vec3 rayValue(Ray& ray, Scene& scene);

private:

    static void writePixel(std::vector<unsigned char>& pixels, const unsigned int x, const unsigned int y, glm::vec4 RGBA);
};