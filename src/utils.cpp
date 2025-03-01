#include "utils.hpp"


int PascalValue(int i, int n)
{
    if(i == 0 || i == n)
        return 1;
    else
        return PascalValue(i, n-1) + PascalValue(i-1, n-1);
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
