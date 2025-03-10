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


void Intersection::cameraRay(AppContext &context, double xPos, double yPos, Ray &ray)
{
    // Calcul des valeurs du rayon lancé
    float x = (2.0f * xPos) / context.SCR_WIDTH - 1.0f;
    float y = 1.0f - (2.0f * yPos) / context.SCR_HEIGHT;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(context.getProjection()) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec4 rayWorld = glm::inverse(context.getView()) * rayEye;

    glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));
    glm::vec3 nearPoint = context.getCamera()->Position;

    ray.setDirection(rayDir);
    ray.setOrigin(nearPoint);
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


glm::vec3 Intersection::rayColorPoint(AppContext &context, const Ray &ray)
{
    float minDistance = -1.f;
    glm::vec3 minColor = context.getBackgroundColor();

    for(int i = 0; i < context.size(); ++i) {
        // SPHERE ---------------------------------------------------------------------------------
        Sphere* item = dynamic_cast<Sphere*>(context.getObject(i));
        if(item != nullptr) {
            Ray dumb;
            if(Ray_Sphere(ray, *item, dumb) &&
                (minDistance < 0 || glm::length(dumb.getOrigin() - ray.getOrigin()) < minDistance))
            {
                minColor = item->getColor();
                minDistance = glm::length(dumb.getOrigin() - ray.getOrigin());
            }
        }
    }

    return minColor;
}


void Intersection::raySavePNG(AppContext &context, std::string filename)
{
    std::vector<unsigned char> image;
    image.resize(context.SCR_WIDTH * context.SCR_HEIGHT * 4);

    for(unsigned int y = 0; y < context.SCR_HEIGHT; ++y)
    {
        for(unsigned int x = 0; x < context.SCR_WIDTH; ++x)
        {
            Ray tmp;
            cameraRay(context, x, y, tmp);
            glm::vec3 color = rayColorPoint(context, tmp);

            image[4 * context.SCR_WIDTH * y + 4 * x + 0] = 255 * color.x;
            image[4 * context.SCR_WIDTH * y + 4 * x + 1] = 255 * color.y;
            image[4 * context.SCR_WIDTH * y + 4 * x + 2] = 255 * color.z;
            image[4 * context.SCR_WIDTH * y + 4 * x + 3] = 255;
        }
    }

    unsigned error = lodepng::encode(filename, image, context.SCR_WIDTH, context.SCR_HEIGHT);
    if(!error) std::cout << "Image saved as '" << filename << "'" << std::endl;
}
