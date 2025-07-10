#pragma once

#include "Ray.hpp"
#include "Object.hpp"

struct HitRecord {
    glm::vec3 point;
    glm::vec3 normal;
    ShaderMaterial material;
    float t;
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const = 0;
};