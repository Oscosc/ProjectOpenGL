#ifndef BEZIER_CURVE_HPP
#define BEZIER_CURVE_HPP

#define MAX_DISCRETE_POINTS 100

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "utils.hpp"

#include <math.h>
#include <iostream>
#include <vector>


using ptsTab = std::vector<glm::vec3>;


class BezierCurve
{
public:

    ptsTab control_points;

    // Constructor
    // -----------
    BezierCurve(ptsTab control_points)
    {
        this->control_points = control_points;
    }

    void debugControlPoints()
    {
        for(glm::vec3 point : this->control_points) {
            std::cout << glm::to_string(point) << std::endl;
        }
    }

    glm::vec3 curveValue(float u)
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

    ptsTab discretize(int nb_points)
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

private:

    float bersteinValue(float u, int i, int n)
    {
        int p = PascalValue(i, n);
        /*
        std::cout << "Bernstein : " << p << " * " << u << "^"
            << i <<" * (1 - " << u << ")^" << n-i
            << std::endl;
        */
        return p * pow(u, i) * pow((1.0f - u), n-i);
    }
};

#endif // BEZIER_CURVE_HPP