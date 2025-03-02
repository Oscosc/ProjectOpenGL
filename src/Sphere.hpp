#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Object.hpp"
#include <math.h>

#define DEFAULT_STACKS 16
#define DEFAULT_SECTORS 16

class Sphere : public Object
{
public:
    Sphere(float radius);
    void draw(Shader shader) override;

private:

    /**
     * @brief Code from : https://www.songho.ca/opengl/gl_sphere.html#sphere
     */
    static ptsTab generateVertices(unsigned int stackCount, unsigned int sectorCount, float radius);

    /**
     * @brief Code also from : https://www.songho.ca/opengl/gl_sphere.html#sphere
     */
    static std::vector<unsigned int> generateIndexes(unsigned int stackCount, unsigned int sectorCount);

    unsigned int EBO;
    unsigned int m_nbVertices;
};

#endif // SPHERE_HPP