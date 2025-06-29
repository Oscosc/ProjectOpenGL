#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Object.hpp"
#include "utils.hpp"
#include <math.h>

#define DEFAULT_STACKS 64
#define DEFAULT_SECTORS 64

class Sphere : public Object
{
public:
    Sphere(float radius, glm::vec3 position = {0, 0, 0}, glm::vec3 color = {1, 1, 1});

    void draw(Scene* scene) override;

    float getRadius() const;

private:

    /**
     * @brief Code from : https://www.songho.ca/opengl/gl_sphere.html#sphere
     */
    static std::vector<Vertex> generateVertices(unsigned int stackCount, unsigned int sectorCount, float radius);

    /**
     * @brief Code also from : https://www.songho.ca/opengl/gl_sphere.html#sphere
     */
    static std::vector<unsigned int> generateIndexes(unsigned int stackCount, unsigned int sectorCount);
    
    float m_radius;
};

#endif // SPHERE_HPP