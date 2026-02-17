#include <ProjectIGAI/geometry/GeometryFactory.hpp>

#include <fstream>
#include <ProjectIGAI/graphics/ObjLoader.hpp>
#include <ProjectIGAI/geometry/Splines.hpp>
#include <ProjectIGAI/core/Logger.hpp>

void GeometryFactory::createSphere(const float radius, const int segments,
    std::vector<Vertex>& vertices, std::vector<unsigned int>& indexes)
{
    unsigned int sectorCount = segments;
    unsigned int stackCount = segments;

    /********************* VERTEX CREATION PART *********************/
    float x, y, z, xy;                              // vertex position

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

            vertices.push_back({
                glm::vec3(x, y, z), // Position
                glm::normalize(glm::vec3(x, y, z)), // Normal
                glm::vec3(float(j)/sectorCount, float(i)/stackCount, 0.0f) // UV
            });
        }
    }

    /********************* INDEXES CREATION PART *********************/
    int k1, k2;

    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indexes.push_back(k1);
                indexes.push_back(k2);
                indexes.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indexes.push_back(k1 + 1);
                indexes.push_back(k2);
                indexes.push_back(k2 + 1);
            }
        }
    }
}

void GeometryFactory::createCube(const float halfLength,
    std::vector<Vertex> &vertices, std::vector<unsigned int> &indexes)
{
    /** TODOOOOOOO */
}

void GeometryFactory::createBezierCurve(const vec3Array &controlPoints, const unsigned int nbPoints,
    std::vector<Vertex> &vertices, std::vector<unsigned int> &indexes)
{
    vertices.resize(nbPoints);
    indexes.resize(nbPoints);

    for(int i = 0; i < nbPoints; ++i) {
        vertices[i] = {
            Splines::Bezier::getValue((float)i / (nbPoints - 1), controlPoints),
            glm::vec3(0.f),
            glm::vec2(0.f)
        };
        indexes[i] = i;
    }
}

void GeometryFactory::createBezierSurface(const vec3Grid &controlPoints,
    const unsigned int nbPointsU, const unsigned int nbPointsV,
    std::vector<Vertex> &vertices, std::vector<unsigned int> &indexes)
{
    vec3Array points;
    points.resize(nbPointsU * nbPointsV);

    for(float i=0; i < nbPointsU; ++i) {
        for(float j=0; j < nbPointsV; ++j) {
            points[i * nbPointsU + j] = Splines::Bezier::getValue(i/(nbPointsU-1), j/(nbPointsV-1), controlPoints);
        }
    }

    // Normals reset (for safety)
    for (auto& vertex : vertices) {
        vertex.normal = glm::vec3(0.0f);
    }

    // Quads iterating
    int k = 0;
    for (size_t u = 0; u < nbPointsU - 1; ++u) {
        for (size_t v = 0; v < nbPointsV - 1; ++v) {
            
            //********** NORMALS **********//
            // Indexes precomputation
            int i0 =  u      * nbPointsU +  v;
            int i1 = (u + 1) * nbPointsU +  v;
            int i2 =  u      * nbPointsU + (v + 1);
            int i3 = (u + 1) * nbPointsU + (v + 1);

            // Positions
            glm::vec3 p0 = vertices[i0].position;
            glm::vec3 p1 = vertices[i1].position;
            glm::vec3 p2 = vertices[i2].position;
            glm::vec3 p3 = vertices[i3].position;

            // Both triangles normals
            glm::vec3 n1 = glm::cross(p1 - p0, p2 - p0);
            glm::vec3 n2 = glm::cross(p3 - p1, p2 - p1);

            // Accumulation
            vertices[i0].normal += n1;
            vertices[i1].normal += n1 + n2;
            vertices[i2].normal += n1 + n2;
            vertices[i3].normal += n2;

            //********** INDEXES **********//
            if(u != (nbPointsU - 1) && v != (nbPointsV - 1)) {
                indexes.push_back(i0);
                indexes.push_back(i1);
                indexes.push_back(i2);

                indexes.push_back(i3);
                indexes.push_back(i1);
                indexes.push_back(i2);
            }

            //********** UVS **********//
            vertices[k].uv = {u/(nbPointsU-1), v/(nbPointsV-1)};
            k++;
        }
    }

    // Final normalization
    for (auto& vertex : vertices) {
        if (glm::length(vertex.normal) > 0.0f)
            vertex.normal = glm::normalize(vertex.normal);
    }
}

void GeometryFactory::loadFromObj(const std::string path,
                                  std::vector<Vertex> &vertices, std::vector<unsigned int> &indexes)
{
    ObjLoader::loadObject(path, vertices, indexes);
}