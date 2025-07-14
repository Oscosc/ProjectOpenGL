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
    bool frontFace;

    void setFaceNormal(const Ray& r, const glm::vec3& outwardNormal) {
        frontFace = glm::dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
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