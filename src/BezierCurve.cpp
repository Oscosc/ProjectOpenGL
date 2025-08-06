#include "BezierCurve.hpp"
#include "Logger.hpp"

BezierCurve::BezierCurve(vec3Array controlPoints, Transform transform, Material material) : Curve(transform, material)
{
    this->m_controlPoints = controlPoints;
    this->m_controlPolygonVisibility = Curve::Visibility::LINES;

    this->m_vertices.resize(NB_CURVE_POINTS);
    this->m_indexes.resize(NB_CURVE_POINTS);

    for(int i = 0; i < NB_CURVE_POINTS; ++i) {
        this->m_vertices[i] = {
            getValue((float)i / (NB_CURVE_POINTS - 1)),
            glm::vec3(0.f),
            glm::vec2(0.f)
        };
        this->m_indexes[i] = i;
    }

    initGLObject();
    initGLControlPolygon();
}

glm::vec3 BezierCurve::getValue(float u) const
{
    if(0.0f > u || u > 1.0f) {
        return glm::vec3(0.0f);
    }

    glm::vec3 result(0.0f);
    for(int i=0; i < this->m_controlPoints.size(); ++i) {
        result += bersteinValue(u, i, this->m_controlPoints.size()-1) * m_controlPoints[i];
    }

    return result;
}

void BezierCurve::draw(Scene* scene) const
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
    glDrawElements(GL_LINE_STRIP, this->m_indexes.size(), GL_UNSIGNED_INT, (void*)0);

    drawControlPolygon();

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        Logger::logError("in Mesh drawing : GLError "  + std::to_string(err));
    }
}

/*
vec3Array BezierCurve::normalDiscretization()
{
    vec3Array discretizedValues;
    for(float i=0; i < m_nbCurvePoints; ++i) {
        discretizedValues.push_back(curveValue(i/(m_nbCurvePoints-1)));
    }
    return discretizedValues;
}


vec3Array BezierCurve::equalDiscretization()
{
    vec3Array vertices = {m_controlPoints[0]};

    unsigned int current = 0;
    for(float i = 0; i <= 1; i += DISCRETIZATION_STEP) {
        glm::vec3 new_point = curveValue(i);
        if (glm::length(vertices[current] - new_point) >= (EQUALY_BASE_SEGMENT / m_nbCurvePoints)) {
            vertices.push_back(new_point);
            ++current;
        }
    }
    vertices.push_back(m_controlPoints[m_controlPoints.size() - 1]);

    return vertices;
}

void BezierCurve::updateCurvePoints()
{
    (isAltModeOn()) ? m_curvePoints = equalDiscretization() : m_curvePoints = normalDiscretization();
    updateVertices(combine(m_controlPoints, m_curvePoints));
}

void BezierCurve::next()
{
    m_nbCurvePoints++;
    updateCurvePoints();
}

void BezierCurve::previous()
{
    if(m_nbCurvePoints > MIN_DISCRETE_POINTS) {
        m_nbCurvePoints--;
        updateCurvePoints();
    }
}

void BezierCurve::switchMode()
{
    altModeOn = !altModeOn;
    updateCurvePoints();
}
*/