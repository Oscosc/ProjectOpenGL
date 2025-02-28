#include "BezierCurve.hpp"


BezierCurve::BezierCurve(ptsTab control_points)
{
    this->control_points = control_points;
}


void BezierCurve::debugControlPoints()
{
    for(glm::vec3 point : this->control_points) {
        std::cout << glm::to_string(point) << std::endl;
    }
}


glm::vec3 BezierCurve::curveValue(float u)
{
    if(0.0f > u || u > 1.0f) {
        std::cout << "u value out of range [0;1]" << std::endl;
        return glm::vec3(0.0f);
    }

    glm::vec3 result(0.0f);
    for(int i=0; i < this->control_points.size(); ++i) {
        result += bersteinValue(u, i, this->control_points.size()-1) * control_points[i];
    }

    return result;
}


ptsTab BezierCurve::discretize(int nb_points)
{
    // LIMIT for VBO
    if (nb_points > MAX_DISCRETE_POINTS)
        nb_points = MAX_DISCRETE_POINTS;
    // LIMIT for rendering
    if (nb_points < 2)
        nb_points = 2;

    ptsTab discretized_values;
    for(float i=0; i < nb_points; ++i) {
        discretized_values.push_back(curveValue(i/(nb_points-1)));
    }
    return discretized_values;
}


ptsTab BezierCurve::discretizeEqualy(float segment_size)
{
    ptsTab vertices = {control_points[0]};

    unsigned int current = 0;
    for(float i = 0; i <= 1; i += DISCRETIZATION_STEP) {
        glm::vec3 new_point = curveValue(i);
        if (glm::length(vertices[current] - new_point) >= segment_size) {
            vertices.push_back(new_point);
            ++current;
        }
    }

    vertices.push_back(control_points[control_points.size() - 1]);

    return vertices;
}


float BezierCurve::bersteinValue(float u, int i, int n)
{
    int p = PascalValue(i, n);
    /*
    std::cout << "Bernstein : " << p << " * " << u << "^"
        << i <<" * (1 - " << u << ")^" << n-i
        << std::endl;
    */
    return p * pow(u, i) * pow((1.0f - u), n-i);
}