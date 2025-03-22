#include "utils.hpp"


int PascalValue(int i, int n)
{
    if(i == 0 || i == n)
        return 1;
    else
        return PascalValue(i, n-1) + PascalValue(i-1, n-1);
}


float bersteinValue(float u, int i, int n)
{
    int p = PascalValue(i, n);
    return p * pow(u, i) * pow((1.0f - u), n-i);
}


std::vector<unsigned int> Range(unsigned int n)
{
    std::vector<unsigned int> rangeVector;
    for(unsigned int i; i < n; ++i)
        rangeVector.push_back(i);
    return rangeVector;
}


std::vector<glm::vec3> combine(std::vector<glm::vec3> vecA, std::vector<glm::vec3> vecB)
{
    std::vector<glm::vec3> tmp;
    tmp.reserve(vecA.size() + vecB.size());
    tmp.insert(tmp.end(), vecA.begin(), vecA.end());
    tmp.insert(tmp.end(), vecB.begin(), vecB.end());
    return tmp;
}

std::vector<unsigned int> combine(std::vector<unsigned int> vecA, std::vector<unsigned int> vecB)
{
    std::vector<unsigned int> tmp;
    tmp.reserve(vecA.size() + vecB.size());
    tmp.insert(tmp.end(), vecA.begin(), vecA.end());
    tmp.insert(tmp.end(), vecB.begin(), vecB.end());
    return tmp;
}


bool solveQuadratic(const float &a, const float &b, const float &c, 
					float &x0, float &x1) {
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = -0.5 * b / a;
    else {
        float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);
    
    return true;
}