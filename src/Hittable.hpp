#pragma once

#include "Ray.hpp"
#include "Object.hpp"

enum HitType {
    DIFFUSE,
    METAL,
    GLASS
};

struct HitRecord {
    glm::vec3 point;
    glm::vec3 normal;
    ShaderMaterial material;
    HitType type;
    float t;
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const = 0;

    static inline const std::unordered_map<std::string, HitType> HitTypeCatalog = {
        {"diffuse", DIFFUSE},
        {"metal", METAL},
        {"glass", GLASS}
    };

    HitType Type;
};