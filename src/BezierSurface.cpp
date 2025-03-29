#include "BezierSurface.hpp"

BezierSurface::BezierSurface(ptsGrid control_points) :
    m_controlPoints(control_points),
    m_nbCurvePointsU(100),
    m_nbCurvePointsV(100)
{
    m_sizeU = control_points.size();
    m_sizeV = control_points[0].size();

    ptsGrid vertices = normalDiscretization();
    ptsGrid normals = computeNormals(vertices);
    ptsTab tableVBO;
    std::vector<unsigned int> tableEBO;
    gridToGlfwDisplayableRepresentation(vertices, normals, tableVBO, tableEBO);

    m_nbVertices = tableEBO.size();
    updateVertices(tableVBO);

    // Completing Object constructor with EBO init
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tableEBO.size() * sizeof(unsigned int), tableEBO.data(), GL_STATIC_DRAW);
}


glm::vec3 BezierSurface::surfaceValue(float u, float v)
{
    if(0.0f > u || u > 1.0f) {
        return glm::vec3(0.0f);
    }

    glm::vec3 result(0.0f);
    for(int i=0; i < this->m_sizeU; ++i) {
        for(int j=0; j < this->m_sizeV; ++j) {
            float n_i = bersteinValue(u, i, this->m_sizeU-1);
            float n_j = bersteinValue(v, j, this->m_sizeV-1);

            result += n_j * n_i * m_controlPoints[i][j];
        }
    }

    return result;
}


void BezierSurface::draw(Shader shader)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_origin);
    shader.setMat4("model", model);

    shader.setVec3("color", m_color);
    shader.setFloat("ambientStrength", m_ambient);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, m_nbVertices, GL_UNSIGNED_INT, (void*)0);
}


ptsGrid BezierSurface::normalDiscretization()
{
    ptsGrid discretizedValues;
    for(float i=0; i < m_nbCurvePointsU; ++i) {
        discretizedValues.push_back(ptsTab());
        for(float j=0; j < m_nbCurvePointsV; ++j) {
            discretizedValues[i].push_back(surfaceValue(i/(m_nbCurvePointsU-1), j/(m_nbCurvePointsV-1)));
        }
    }

    return discretizedValues;
}


ptsGrid BezierSurface::computeNormals(const ptsGrid& sampledPoints) 
{
    size_t rows = sampledPoints.size();
    size_t cols = sampledPoints[0].size();
    ptsGrid normals(rows, std::vector<glm::vec3>(cols));

    for (size_t u = 0; u < rows - 1; ++u) {
        for (size_t v = 0; v < cols - 1; ++v) {
            glm::vec3 Su = sampledPoints[u + 1][v] - sampledPoints[u][v];
            glm::vec3 Sv = sampledPoints[u][v + 1] - sampledPoints[u][v];

            glm::vec3 normal = glm::normalize(glm::cross(Sv, Su));
            normals[u][v] = normal;
        }
    }

    // Dernier point
    glm::vec3 Su = sampledPoints[rows - 2][cols - 1] - sampledPoints[rows - 1][cols - 1];
    glm::vec3 Sv = sampledPoints[rows - 1][cols - 2] - sampledPoints[rows - 1][cols - 1];
    glm::vec3 normal = glm::normalize(glm::cross(Sv, Su));
    normals[rows - 1][cols - 1] = normal;

    return normals;
}


void BezierSurface::gridToGlfwDisplayableRepresentation(const ptsGrid &points, const ptsGrid &normals, ptsTab &tableVBO,
    std::vector<unsigned int> &tableEBO)
{
    /**
     * (i,j+1)    (i+1, j+1)
     *      +-----+
     *      | \   |
     *      |  \  |
     *      |   \ |
     *      +-----+
     * (i, j)     (i+1, j)
     * 
     * Triangle 1 : (i, j),    (i+1, j), (i, j+1)
     * = (i * sizeU + j), ((i+1) * sizeU + j), (i * sizeU + j+1)
     * 
     * Triangle 2 : (i+1,j+1), (i+1, j), (i, j+1)
     * = ((i+1) * sizeU + j+1), ((i+1) * sizeU + j), (i * sizeU + j+1)
     */
    
    for(int i=0; i < m_nbCurvePointsU; ++i) {
        for(int j=0; j < m_nbCurvePointsV; ++j) {
            // Ajout des point au VBO
            tableVBO.push_back(points[i][j]);
            tableVBO.push_back(normals[i][j]);
            tableVBO.push_back(glm::vec3(float(i)/m_nbCurvePointsU, float(j)/m_nbCurvePointsV, 0.0f));
            // std::cout << glm::to_string(glm::vec3(float(i)/m_nbCurvePointsU, float(j)/m_nbCurvePointsV, 0.0f)) << std::endl;
            
            // Ajout des index a l'EBO
            if(i != (m_nbCurvePointsU-1) && j != (m_nbCurvePointsV-1)) {
                tableEBO.push_back(i * m_nbCurvePointsU + j);
                tableEBO.push_back((i+1) * m_nbCurvePointsU + j);
                tableEBO.push_back(i * m_nbCurvePointsU + (j+1));

                tableEBO.push_back((i+1) * m_nbCurvePointsU + (j+1));
                tableEBO.push_back((i+1) * m_nbCurvePointsU + j);
                tableEBO.push_back(i * m_nbCurvePointsU + (j+1));
            }
        }
    }
}