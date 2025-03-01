#ifndef BEZIER_CURVE_HPP
#define BEZIER_CURVE_HPP

#define MIN_DISCRETE_POINTS 2
#define DISCRETIZATION_STEP 0.01

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "utils.hpp"
#include "Object.hpp"
#include "ScalableElement.hpp"

#include <math.h>
#include <iostream>
#include <vector>


class BezierCurve : public Object, public ScalableElement
{
public:

    BezierCurve(ptsTab control_points);

    void debugControlPoints();
    glm::vec3 curveValue(float u);
    ptsTab discretizeEqualy(float segment_size);
    
    // Fonctions héritées de Object
    void draw(Shader shader) override;

    // Fonctions héritées de ScalableElement
    void next() override;
    void previous() override;

private:

    ptsTab m_controlPoints;
    ptsTab m_curvePoints;
    unsigned int m_nbCurvePoints;

    GLuint controlVAO, controlVBO;

    ptsTab normalDiscretization();
    ptsTab equalDiscretization();
    void updateCurvePoints();
    float bersteinValue(float u, int i, int n);
};

#endif //BEZIER_CURVE_HPP