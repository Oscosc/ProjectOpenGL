#pragma once

#include <ProjectIGAI/raytracing/Ray.hpp>
#include <ProjectIGAI/graphics/Object.hpp>

/**
 * @brief Classe used to simulate CPU ray-tracing (based on RT in one weekend page)
 * 
 * @warning DEPRECATED, UNUSED
 */
enum HitType {
    DIFFUSE,
    METAL,
    GLASS
};

/**
 * @brief Classe used to simulate CPU ray-tracing (based on RT in one weekend page)
 * 
 * @warning DEPRECATED, UNUSED
 */
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

/**
 * @brief Classe used to simulate CPU ray-tracing (based on RT in one weekend page)
 * 
 * @warning DEPRECATED, UNUSED
 */
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