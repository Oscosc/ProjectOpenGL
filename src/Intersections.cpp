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


bool Intersection::Ray_Triangle(const Ray &ray, const Triangle &triangle, Ray &reflexion)
{
    glm::vec3 e1 = triangle.b - triangle.a;
    glm::vec3 e2 = triangle.c - triangle.a;
    
    glm::vec3 N = e1 * e2;
    glm::vec3 P = ray.getDirection() * e2;

    float det = glm::dot(e1, P);
    // Le rayon est parallèle au plan 
    if(det <= ZERO_THRESHOLD && det >= -ZERO_THRESHOLD) return false;

    float invDet = 1 / det;

    glm::vec3 T = ray.getOrigin() - triangle.a;
    float u = glm::dot(T, P) * invDet;
    // Si u n'est pas compris dans [0;1] alors le point d'intersection est hors du triangle
    if(u < 0 || u > 1) return false;

    glm::vec3 Q = T * e1;
    float v = glm::dot(ray.getDirection(), Q) * invDet;
    // Si v n'est pas compris dans [0;1] alors le point d'intersection est hors du triangle
    if(v < 0 || u > 1) return false;

    float t = glm::dot(e2, Q) * invDet;
    // Si t est inférieur à 0 alors l'intersection est derrière le rayon
    if(t < 0) return false;

    // Sinon, rayon ok donc on met a jour et on renvoie true
    reflexion.setOrigin(ray.getPoint(t));
    glm::vec3 new_dir = ray.getDirection() - 2 * glm::dot(ray.getDirection(), N) * N;
    reflexion.setDirection(new_dir);



    return true;
}


void Intersection::cameraRay(AppContext &context, double xPos, double yPos, Ray &ray)
{
    // Calcul des valeurs de position du rayon lancé
    float x = (2.0f * xPos) / context.SCR_WIDTH - 1.0f;
    float y = 1.0f - (2.0f * yPos) / context.SCR_HEIGHT;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f); // Vecteur clippé

    // Inversion de la matrice de projection : retour dans la view
    glm::vec4 rayEye = glm::inverse(context.getProjection()) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // Inversion de la matrice de view : retour dans l'espace 3D
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
                
                continue;
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
