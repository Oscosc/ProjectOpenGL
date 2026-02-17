#pragma once

#include <vector>
#include <ProjectIGAI/geometry/Vertex.hpp>

class GeometryFactory
{
public:

    static void createSphere(const float radius, const int segments,
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indexes);
    
    static void createCube(const float halfLength,
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indexes);

    static void createBezierCurve(const vec3Array& controlPoints, const unsigned int nbPoints,
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indexes);

    static void createBezierSurface(const vec3Grid& controlPoints,
        const unsigned int nbPointsU, const unsigned int nbPointsV,
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indexes);

    static void loadFromObj(const std::string path,
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indexes);
};