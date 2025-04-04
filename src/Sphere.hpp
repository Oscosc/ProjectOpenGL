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
    Sphere(float radius, glm::vec3 position, glm::vec3 color);

    void draw(Shader shader) override;
    //void previous() override;
    //void next() override;

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

    void updateEBO(std::vector<unsigned int> triangleIndexes, std::vector<unsigned int> lineIndexes);

    unsigned int EBOTriangles;
    unsigned int EBOLines, VAOLines;
    
    float m_radius;
    unsigned int m_nbVertices, m_nbVerticesLines;
};

#endif // SPHERE_HPP