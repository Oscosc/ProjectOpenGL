#include <ProjectIGAI/geometry/GeometryFactory.hpp>

#include <fstream>
#include <ProjectIGAI/graphics/ObjLoader.hpp>

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

void GeometryFactory::loadFromObj(const std::string path,
    std::vector<Vertex> &vertices, std::vector<unsigned int> &indexes)
{
    ObjLoader::loadObject(path, vertices, indexes);
}