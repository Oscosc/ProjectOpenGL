#include "BezierCurve.hpp"


BezierCurve::BezierCurve(ptsTab controlPoints) :
    m_controlPoints(controlPoints),
    m_nbCurvePoints(MIN_DISCRETE_POINTS)
{
    updateCurvePoints();
}


void BezierCurve::debugControlPoints()
{
    for(glm::vec3 point : this->m_controlPoints) {
        std::cout << glm::to_string(point) << std::endl;
    }
}


glm::vec3 BezierCurve::curveValue(float u)
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


ptsTab BezierCurve::normalDiscretization()
{
    ptsTab discretizedValues;
    for(float i=0; i < m_nbCurvePoints; ++i) {
        discretizedValues.push_back(curveValue(i/(m_nbCurvePoints-1)));
    }
    return discretizedValues;
}


ptsTab BezierCurve::equalDiscretization()
{
    ptsTab vertices = {m_controlPoints[0]};

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


void BezierCurve::draw(Shader shader)
{
    glBindVertexArray(VAO);

    // Dessine le polygone de controle
    shader.setVec3("color", 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINE_STRIP, 0, m_controlPoints.size());

    // Dessine la courbe de Bezier
    shader.setVec3("color", 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_LINE_STRIP, m_controlPoints.size(), m_curvePoints.size());

    // Optionnel : "déconnecte" le VAO
    glBindVertexArray(0);
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


float BezierCurve::bersteinValue(float u, int i, int n)
{
    int p = PascalValue(i, n);
    return p * pow(u, i) * pow((1.0f - u), n-i);
}