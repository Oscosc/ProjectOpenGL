#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include "../includes/camera.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600
#define RAYS_PER_PIXEL 10

class RayTracing
{
public:
    static void computeImage(const std::string& filename, Scene& scene);
    static void computePixel(
        const float& x,
        const float& y,
        Scene& scene,
        const float& radianFOV,
        const glm::mat4& cameraToWorld,
        std::vector<unsigned char>& pixels,
        std::vector<Sphere*> sceneSpheres
    );

    static glm::vec3 rayValue(Ray& ray, const std::vector<Sphere*>& sceneSpheres);

private:

    static void writePixel(std::vector<unsigned char>& pixels, const unsigned int x, const unsigned int y, glm::vec4 RGBA);
};