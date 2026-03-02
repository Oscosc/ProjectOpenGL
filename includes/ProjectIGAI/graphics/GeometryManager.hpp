#pragma once

#include <map>

#include <ProjectIGAI/geometry/Geometry.hpp>
#include <ProjectIGAI/core/Singleton.hpp>

class GeometryManager : public Singleton<GeometryManager>
{
    friend class Singleton<GeometryManager>;

public:

    Geometry* getSphere(const float radius, const int segments);

    Geometry* getCube(const float halfLength);

    Geometry* getMesh(const std::string path);

    Geometry* getRawGeometry(const std::string& name,
        const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indexes);

private:

    /** Geometry manager constructor (private for Singleton) */
    GeometryManager() {}

    std::map<std::string, Geometry*> m_geometries;

};