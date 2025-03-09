#ifndef INTERSECTIONS_HPP
#define INTERSECTIONS_HPP

#include "Sphere.hpp"
#include "Ray.hpp"
#include "AppContext.hpp"

#define MAX_RAY_BOUNCES 5

class Intersection
{
public:
    static bool Ray_Sphere(const Ray &ray, const Sphere &sphere, Ray &reflexion);
    static void rayContextPath(AppContext &context, const Ray &ray, ptsTab &intersections, glm::vec3 &reflexion);

};

#endif // INTERSECTIONS_HPP