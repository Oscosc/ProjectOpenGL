#include "Sphere.hpp"


Sphere::Sphere(float radius)
{
    ptsTab vertices = generateVertices(DEFAULT_STACKS, DEFAULT_SECTORS, radius);
    //std::cout << vertices.size() << std::endl;
    //for(auto v: vertices) {std::cout << glm::to_string(v) << std::endl;}
    std::vector<unsigned int> indexes = generateIndexes(DEFAULT_STACKS, DEFAULT_SECTORS);
    for(auto i: indexes) {std::cout << i << std::endl;}
    m_nbVertices = indexes.size();
    //std::cout << m_nbVertices << std::endl;

    // Completing Object constructor with EBO init
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbVertices * sizeof(unsigned int), indexes.data(), GL_STATIC_DRAW);

    updateVertices(vertices);
}


void Sphere::draw(Shader shader)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    shader.setVec3("color", 0.8f, 0.8f, 0.8f);
    glDrawElements(GL_TRIANGLES, m_nbVertices, GL_UNSIGNED_INT, 0);
}


ptsTab Sphere::generateVertices(unsigned int stackCount, unsigned int sectorCount, float radius)
{
    ptsTab vertices;

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
            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    return vertices;
}


std::vector<unsigned int> Sphere::generateIndexes(unsigned int stackCount, unsigned int sectorCount)
{
    std::vector<unsigned int> indices;
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
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    return indices;
}