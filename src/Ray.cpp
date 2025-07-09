#include "Ray.hpp"

Ray::Ray(const float& x, const float& y, const float& width, const float& height, const float& fov, const glm::mat4& view, const glm::vec3& origin) {
    float aspect = width / height;

    float radianFOV = glm::radians(fov);
    float px = ((x + 0.5f) / width - 0.5f) * 2.0f * aspect * tan(radianFOV / 2.0f);
    float py = (0.5f - (y + 0.5f) / height) * 2.0f * tan(radianFOV / 2.0f);
    float pz = -1.0f;

    glm::vec3 dirCameraSpace(px, py, pz);
    dirCameraSpace = glm::normalize(dirCameraSpace);

    this->m_direction = glm::normalize(glm::vec3(glm::inverse(view) * glm::vec4(dirCameraSpace, 0.0f)));
    this->m_origin = origin;
}

float Ray::hitSphere(Sphere &sphere)
{
    glm::vec3 oc = sphere.getTransform().position - m_origin;
    
    float a = glm::dot(m_direction, m_direction);
    float b = -2.f * glm::dot(m_direction, oc);
    float c = glm::dot(oc, oc) - sphere.getRadius() * sphere.getRadius();
    float discriminant = b*b - 4 *a*c;

    if(discriminant < 0) {
        return -1.0f;
    } else {
        return (-b - std::sqrt(discriminant)) / (2.0f * a);
    }
}
