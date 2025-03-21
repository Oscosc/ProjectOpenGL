#include "Sphere.hpp"


Sphere::Sphere(float radius) : m_radius(radius)
{
    // Completing Object constructor with EBO init
    glGenBuffers(1, &EBOTriangles);
    glGenBuffers(1, &EBOLines);

    std::vector<unsigned int> lineIndexes;

    unsigned int stacks = (radius > 1.0f) ? DEFAULT_STACKS : round(DEFAULT_STACKS * radius);
    unsigned int sectors = (radius > 1.0f) ? DEFAULT_SECTORS : round(DEFAULT_SECTORS * radius);

    ptsTab vertices = generateVertices(stacks, sectors, m_radius);
    std::vector<unsigned int> triangleIndexes = generateIndexes(stacks, sectors, &lineIndexes);

    m_nbVertices = triangleIndexes.size();
    m_nbVerticesLines = lineIndexes.size();

    updateVertices(vertices);
    updateEBO(triangleIndexes, lineIndexes);
}


Sphere::Sphere(float radius, glm::vec3 position, glm::vec3 color) : Sphere(radius)
{
    setOrigin(position);
    setColor(color);
}


void Sphere::draw(Shader shader)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_origin);
    shader.setMat4("model", model);

    glBindVertexArray(VAO);

    shader.setVec3("color", m_color);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOTriangles);
    glDrawElements(GL_TRIANGLES, m_nbVertices, GL_UNSIGNED_INT, (void*)0);

    //shader.setVec3("color", 0.f, 0.f, 0.f);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOLines);
    //glDrawElements(GL_LINES, m_nbVerticesLines, GL_UNSIGNED_INT, (void*)0);
}


float Sphere::getRadius() const
{
    return m_radius;
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


std::vector<unsigned int> Sphere::generateIndexes(unsigned int stackCount, unsigned int sectorCount,
    std::vector<unsigned int>* lineIndices)
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

            // Pour afficher les lignes
            lineIndices->push_back(k1);
            lineIndices->push_back(k2);
            if(i != 0)  // horizontal lines except 1st stack, k1 => k+1
            {
                lineIndices->push_back(k1);
                lineIndices->push_back(k1 + 1);
            }
        }
    }

    return indices;
}


void Sphere::updateEBO(std::vector<unsigned int> triangleIndexes, std::vector<unsigned int> lineIndexes)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOTriangles);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbVertices * sizeof(unsigned int), triangleIndexes.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOLines);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbVerticesLines * sizeof(unsigned int), lineIndexes.data(), GL_STATIC_DRAW);
}
