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
    Sphere(float radius);
    Sphere(float radius, glm::vec3 position);

    void draw(Shader shader) override;

    float getRadius() const;

private:

    /**
     * @brief Code from : https://www.songho.ca/opengl/gl_sphere.html#sphere
     */
    static ptsTab generateVertices(unsigned int stackCount, unsigned int sectorCount, float radius);

    /**
     * @brief Code also from : https://www.songho.ca/opengl/gl_sphere.html#sphere
     */
    static std::vector<unsigned int> generateIndexes(unsigned int stackCount, unsigned int sectorCount,
        std::vector<unsigned int>* lineIndices);

    unsigned int EBOTriangles;
    unsigned int EBOLines, VAOLines;
    
    float m_radius;
    unsigned int m_nbVertices, m_nbVerticesLines;
};

#endif // SPHERE_HPP