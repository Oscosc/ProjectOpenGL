#ifndef INTERSECTIONS_HPP
#define INTERSECTIONS_HPP

#include "Sphere.hpp"
#include "Ray.hpp"
#include "AppContext.hpp"

#include "lodepng.h"

#define MAX_RAY_BOUNCES 100

class Intersection
{
public:
    static bool Ray_Sphere(const Ray &ray, const Sphere &sphere, Ray &reflexion);

    static void cameraRay(AppContext &context, double xPos, double yPos, Ray &ray);
    static void rayContextPath(AppContext &context, const Ray &ray, ptsTab &intersections, glm::vec3 &reflexion);

    static glm::vec3 rayColorPoint(AppContext &context, const Ray &ray);
    static void raySavePNG(AppContext &context, std::string filename);

};

#endif // INTERSECTIONS_HPP