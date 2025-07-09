#pragma once

#include "Ray.hpp"

struct HitRecord {
    glm::vec3 point;
    glm::vec3 normal;
    float t;
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const = 0;
};