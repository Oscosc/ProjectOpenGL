#include "Intersections.hpp"

bool Intersection::Ray_Sphere(const Ray &ray, const Sphere &sphere, Ray &reflexion)
{
    // CALCUL DU POINT D'INTERSECTION

    float t0, t1; // Solutions for t if the ray intersects the sphere
    glm::vec3 L = ray.getOrigin() - sphere.getOrigin();
    float a = glm::dot(ray.getDirection(), ray.getDirection());
    float b = 2 * glm::dot(ray.getDirection(), L);
    float c = glm::dot(L, L) - sphere.getRadius() * sphere.getRadius();
    if (!solveQuadratic(a, b, c, t0, t1)) return false;
    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1; // If t0 is negative, let's use t1 instead.
        if (t0 < 0) return false; // Both t0 and t1 are negative.
    }

    reflexion.setOrigin(ray.getPoint(t0));

    // CALCUL DE LA NOUVELLE DIRECTION
    glm::vec3 norm = glm::normalize(ray.getPoint(t0) - sphere.getOrigin());
    glm::vec3 dir = ray.getDirection();
    glm::vec3 new_dir = dir - 2 * glm::dot(dir, norm) * norm;

    reflexion.setDirection(new_dir);

    return true;
}


void Intersection::rayContextPath(AppContext &context, const Ray &ray, ptsTab &intersections,
    glm::vec3 &reflexion)
{

    unsigned int bouncesCount = 0;
    int objectIdBounceFrom = -1; // Pour ne pas regarder les collision avec l'objet courant
    Ray currentRay = ray;
    Ray nextRay(glm::vec3(0.0f), glm::vec3(0.0f));

    while(bouncesCount < MAX_RAY_BOUNCES)
    {
        std::vector<float> interDist;
        std::vector<unsigned int> interInd;

        // Compute all intersections for this ray
        for(int i = 0; i < context.size(); ++i)
        {
            if(i == objectIdBounceFrom) continue;

            // SPHERE -----------------------------------------------------------------------------
            Sphere* item = dynamic_cast<Sphere*>(context.getObject(i));
            if(item != nullptr) {
                bool intersect = Ray_Sphere(currentRay, *item, nextRay);

                if(intersect) {
                    interDist.push_back(glm::length(nextRay.getOrigin() - currentRay.getOrigin()));
                    interInd.push_back(i);
                }
            }
        }

        // No intersections = finished
        if(interInd.size() == 0) return;

        // Choose closest one
        unsigned int minIndex = std::min_element(interDist.begin(), interDist.end()) - interDist.begin();
        unsigned int closestIndex = interInd[minIndex];

        // Re-compute intersection
        Sphere* item = dynamic_cast<Sphere*>(context.getObject(closestIndex));
        Ray_Sphere(currentRay, *item, nextRay);
        
        // Update variables
        intersections.push_back(nextRay.getOrigin());
        reflexion = nextRay.getDirection();
        currentRay = nextRay;
        objectIdBounceFrom = closestIndex;
        bouncesCount++;
    }
}
