#include <ProjectIGAI/geometry/Splines.hpp>

int Splines::PascalValue(int i, int n)
{
    if(i == 0 || i == n)
        return 1;
    else
        return PascalValue(i, n-1) + PascalValue(i-1, n-1);
}

float Splines::bersteinValue(float u, int i, int n)
{
    int p = PascalValue(i, n);
    return p * pow(u, i) * pow((1.0f - u), n-i);
}

glm::vec3 Splines::Bezier::getValue(float u, std::vector<glm::vec3> controlPoints)
{
    if(0.0f > u || u > 1.0f) {
        return glm::vec3(0.0f);
    }

    unsigned int sizeU = controlPoints.size();

    glm::vec3 result(0.0f);
    for(int i=0; i < sizeU; ++i) {
        float n_i = bersteinValue(u, i, sizeU-1);

        result += n_i * controlPoints[i];
    }

    return result;
}

glm::vec3 Splines::Bezier::getValue(float u, float v, std::vector<std::vector<glm::vec3>> controlPoints)
{
    if(0.0f > u || u > 1.0f || 0.0f > v || v > 1.0f) {
        return glm::vec3(0.0f);
    }

    unsigned int sizeU = controlPoints.size();
    unsigned int sizeV = controlPoints[0].size();

    glm::vec3 result(0.0f);
    for(int i=0; i < sizeU; ++i) {
        for(int j=0; j < sizeV; ++j) {
            float n_i = bersteinValue(u, i, sizeU-1);
            float n_j = bersteinValue(v, j, sizeV-1);

            result += n_j * n_i * controlPoints[i][j];
        }
    }

    return result;
}
