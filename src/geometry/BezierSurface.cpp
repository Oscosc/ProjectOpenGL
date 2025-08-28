#include <ProjectIGAI/geometry/BezierSurface.hpp>

#include <ProjectIGAI/geometry/Curve.hpp>

BezierSurface::BezierSurface(vec3Grid controlPoints, Transform transform, Material material) :
    Surface(transform, material)
{
    this->m_controlPoints = controlPoints;
    this->m_sizeU = controlPoints.size();
    this->m_sizeV = controlPoints[0].size();

    vec3Array points;
    points.resize(NB_CURVE_POINTS * NB_CURVE_POINTS);

    for(float i=0; i < NB_CURVE_POINTS; ++i) {
        for(float j=0; j < NB_CURVE_POINTS; ++j) {
            glm::vec2 uv = {i/(NB_CURVE_POINTS-1), j/(NB_CURVE_POINTS-1)};
            points[i * NB_CURVE_POINTS + j] = getValue(uv);
        }
    }

    this->m_vertices = defaultPointToVertex(points);
    computeNormals();
    computeIndexes();

    initGLObject();
}

glm::vec3 BezierSurface::getValue(glm::vec2 uv) const
{
    float u = uv.x;
    float v = uv.y;

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

void BezierSurface::draw(Scene* scene) const
{
    Shader* shader = this->getMaterial().shader;
    if(shader == nullptr) {
        Logger::logError("No shader instanciated for this object");
    }
    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(glm::mat4(1.0f), this->m_transform.position);
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.x), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.y), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.z), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, this->m_transform.scale);

    shader->setMat4("model", model);
    shader->setMat4("view", scene->getActiveCameraPV().view);
    shader->setMat4("projection", scene->getActiveCameraPV().projection);

    updateMaterial(shader);
    
    scene->updateLigth(shader);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(this->m_VAO);
    glDrawElements(GL_TRIANGLES, this->m_indexes.size(), GL_UNSIGNED_INT, (void*)0);

    // drawControlPolygon();

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        Logger::logError("in Mesh drawing : GLError "  + std::to_string(err));
    }
}

void BezierSurface::computeNormals() 
{
    for (size_t u = 0; u < NB_CURVE_POINTS - 1; ++u) {
        for (size_t v = 0; v < NB_CURVE_POINTS - 1; ++v) {
            glm::vec3 Su = this->m_vertices[(u+1) * NB_CURVE_POINTS + v].position - this->m_vertices[u * NB_CURVE_POINTS + v].position;
            glm::vec3 Sv = this->m_vertices[u * NB_CURVE_POINTS + (v+1)].position - this->m_vertices[u * NB_CURVE_POINTS + v].position;

            this->m_vertices[u * NB_CURVE_POINTS + v].normal = glm::normalize(glm::cross(Sv, Su));
        }
    }

    // Dernier point
    glm::vec3 Su = this->m_vertices[(NB_CURVE_POINTS-2) * NB_CURVE_POINTS + (NB_CURVE_POINTS-1)].position - this->m_vertices[NB_CURVE_POINTS * NB_CURVE_POINTS - 1].position;
    glm::vec3 Sv = this->m_vertices[NB_CURVE_POINTS * NB_CURVE_POINTS - 2].position - this->m_vertices[NB_CURVE_POINTS * NB_CURVE_POINTS - 1].position;
    
    this->m_vertices[NB_CURVE_POINTS * NB_CURVE_POINTS - 1].normal = glm::normalize(glm::cross(Sv, Su));
}

void BezierSurface::computeIndexes()
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
    
    for(int i=0; i < NB_CURVE_POINTS; ++i) {
        for(int j=0; j < NB_CURVE_POINTS; ++j) {
            if(i != (NB_CURVE_POINTS - 1) && j != (NB_CURVE_POINTS - 1)) {
                this->m_indexes.push_back(i * NB_CURVE_POINTS + j);
                this->m_indexes.push_back((i+1) * NB_CURVE_POINTS + j);
                this->m_indexes.push_back(i * NB_CURVE_POINTS + (j+1));

                this->m_indexes.push_back((i+1) * NB_CURVE_POINTS + (j+1));
                this->m_indexes.push_back((i+1) * NB_CURVE_POINTS + j);
                this->m_indexes.push_back(i * NB_CURVE_POINTS + (j+1));
            }
        }
    }
}