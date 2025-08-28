#include <ProjectIGAI/Ray.hpp>

Ray::Ray(const float& x, const float& y, const float& width, const float& height, const float& tanHalfFOV, const glm::mat4& cameraToWorld, const glm::vec3& origin) {
    float aspect = width / height;

    float px = ((x + 0.5f) / width - 0.5f) * 2.0f * aspect * tanHalfFOV;
    float py = (0.5f - (y + 0.5f) / height) * 2.0f * tanHalfFOV;
    float pz = -1.0f;

    glm::vec3 dirCameraSpace(px, py, pz);
    dirCameraSpace = glm::normalize(dirCameraSpace);

    this->m_direction = glm::normalize(glm::vec3(cameraToWorld * glm::vec4(dirCameraSpace, 0.0f)));
    this->m_origin = origin;
}
