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

    static void loadFromObj(const std::string path,
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indexes);
};