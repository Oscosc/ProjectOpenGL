#ifndef BEZIER_CURVE_HPP
#define BEZIER_CURVE_HPP

#define MAX_DISCRETE_POINTS 100
#define DISCRETIZATION_STEP 0.05

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

    BezierCurve(ptsTab control_points);

    void debugControlPoints();
    glm::vec3 curveValue(float u);
    ptsTab discretize(int nb_points);
    ptsTab discretizeEqualy(int segment_size);

private:

    ptsTab control_points;

    float bersteinValue(float u, int i, int n);
};

#endif // BEZIER_CURVE_HPP