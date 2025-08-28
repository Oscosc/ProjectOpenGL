#include <ProjectIGAI/utils.hpp>

#include <iostream>

#include "lodepng.h"
#include <ProjectIGAI/Object.hpp>
#include <ProjectIGAI/Logger.hpp>


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


std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}


std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end;

    std::string ss = trim(s);

    while ((end = ss.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(ss.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(ss.substr(start));

    return tokens;
}

unsigned int mean(const std::vector<unsigned int> &values)
{
    unsigned int meanValue = 0;
    for(unsigned int element : values) { meanValue += element; }
    return meanValue / values.size();
}

void savePNG(const std::vector<unsigned char> &pixels, const unsigned int width, const unsigned int height, const std::string &filename)
{
    if((width * height * 4) != pixels.size()) {
        Logger::logError(std::to_string(width) + "x" + std::to_string(height) + " do not match pixels number (" + std::to_string(pixels.size()) + ")");
    }

    unsigned error = lodepng::encode(filename, pixels, width, height);
    if(!error) Logger::logInfo("Image saved as '" + filename + "'");
}

float lengthSquared(glm::vec3 v) {
    return pow(v.x, 2.f) + pow(v.y, 2.f) + pow(v.z, 2.f);
}

float randomFloat(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * std::rand() / (RAND_MAX + 1.0f);
}

glm::vec3 randomUnitVec3(float min, float max)
{
    return glm::normalize(glm::vec3(
        randomFloat(min, max),
        randomFloat(min, max),
        randomFloat(min, max)
    ));
}

glm::vec3 randomEmisphereVec3(const glm::vec3 &normal)
{
    glm::vec3 randomVec = randomUnitVec3();
    return glm::dot(randomVec, normal) > 0.0 ? randomVec : -randomVec;
}

glm::vec2 noise2D(const float &x, const float &y, const float &intensity)
{
    return {
        (randomFloat() - 0.5f + x) * intensity,
        (randomFloat() - 0.5f + y) * intensity, 
    };
}

glm::vec3 reflectVec3(const glm::vec3 &v, const glm::vec3 &n)
{
    return v - 2 * glm::dot(v, n) * n;
}

glm::vec3 refractVec3(const glm::vec3 &v, const glm::vec3 &n, float etaCoeff)
{
    float cosTheta = std::fmin(dot(-v, n), 1.f);
    glm::vec3 rayOutPerpendicular = etaCoeff * (v + cosTheta * n);
    glm::vec3 rayOutParallel = -std::sqrt(std::fabs(1.f - lengthSquared(rayOutPerpendicular))) * n;
    
    return rayOutPerpendicular + rayOutParallel;
}

float reflectance(const float &cosine, const float &refractionIndex)
{
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 *= r0; // Squared
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}
