#ifndef BEZIER_SURFACE
#define BEZIER_SURFACE

#include "Object.hpp"
#include "utils.hpp"

using ptsGrid = std::vector<std::vector<glm::vec3>>;

class BezierSurface : public Object
{
public:
    BezierSurface(ptsGrid control_points);
    glm::vec3 surfaceValue(float u, float v);

    void draw(Shader shader) override;

private:
    ptsGrid m_controlPoints;
    unsigned int m_sizeU;
    unsigned int m_sizeV;
    unsigned int m_nbCurvePointsU;
    unsigned int m_nbCurvePointsV;
    unsigned int m_nbVertices;

    unsigned int EBO;

    ptsGrid normalDiscretization();
    ptsGrid computeNormals(const ptsGrid& sampledPoints);
    void gridToGlfwDisplayableRepresentation(const ptsGrid &points, const ptsGrid &normals,
        ptsTab &tableVBO, std::vector<unsigned int> &tableEBO);
};

#endif // BEZIER_SURFACE
