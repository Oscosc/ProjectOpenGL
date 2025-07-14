#include "RayTracing.hpp"

#include "utils.hpp"
#include "Sphere.hpp"
#include "../includes/progressbar.hpp"

#include <omp.h>

#include <chrono>
#define timer std::chrono::high_resolution_clock
#define duration std::chrono::duration_cast<std::chrono::milliseconds>
#define micro_duration std::chrono::duration_cast<std::chrono::microseconds>


void RayTracing::computeImage(const std::string &filename, Scene& scene)
{
    auto timerA = timer::now();
    std::cout << "[PERFORMANCE] Starting Ray-tracing computation..." << std::endl;

    glm::mat4 cameraToWorld = glm::inverse(scene.getActiveCamera()->GetViewMatrix());
    float tanHalfFOV = tan(glm::radians(scene.getActiveCamera()->Fov) / 2.0);

    std::vector<unsigned char> pixels;
    pixels.resize(IMAGE_WIDTH * IMAGE_HEIGHT * 4);
    auto sceneSpheres = scene.getSpheresRT();

    progressbar bar(IMAGE_HEIGHT * IMAGE_WIDTH);

    #pragma omp parallel for collapse(2)
    for(unsigned int h = 0; h < IMAGE_HEIGHT; ++h) {
        for(unsigned int w = 0; w < IMAGE_WIDTH; ++w) {
            computePixel(w, h, scene, tanHalfFOV, cameraToWorld, pixels, sceneSpheres);
            
            #pragma omp critical
            bar.update();
        }
    }
    std::cout << std::endl;

    auto timerB = timer::now();
    std::cout << "[PERFORMANCE] Ray-tracing computation time : " << duration(timerB - timerA).count() << " ms" << std::endl;

    savePNG(pixels, IMAGE_WIDTH, IMAGE_HEIGHT, filename);

    auto timerC  = timer::now();
    std::cout << "[PERFORMANCE] File saving computation time : " << duration(timerC - timerB).count() << " ms" << std::endl;
}

void RayTracing::computePixel(
    const float& x,
    const float& y,
    Scene& scene,
    const float& tanHalfFOV,
    const glm::mat4& cameraToWorld,
    std::vector<unsigned char>& pixels,
    std::vector<Sphere*> sceneSpheres
) {
#if RAYS_PER_PIXEL != 1

    glm::vec3 finalColor = glm::vec3(0.f);
    for(unsigned int i = 0; i < RAYS_PER_PIXEL; ++i) {
        glm::vec2 noised = noise2D(x, y);

        Ray ray = Ray(noised.x, noised.y, IMAGE_WIDTH, IMAGE_HEIGHT,
            tanHalfFOV,
            cameraToWorld,
            scene.getActiveCamera()->Position
        );

        finalColor += rayValue(ray, sceneSpheres) / (float)RAYS_PER_PIXEL;
    }
    writePixel(pixels, x, y, glm::vec4(finalColor, 1.0));

#else

    Ray ray = Ray(x, y, IMAGE_WIDTH, IMAGE_HEIGHT,
        tanHalfFOV,
        cameraToWorld,
        scene.getActiveCamera()->Position
    );
    writePixel(pixels, x, y, glm::vec4(rayValue(ray, sceneSpheres), 1.0));

#endif
}

glm::vec3 RayTracing::rayValue(const Ray &ray, const std::vector<Sphere*>& sceneSpheres, const unsigned int depth)
{
    // Max bounces
    if(depth <= 0) return glm::vec3(0.f);

    HitRecord finalRec;
    finalRec.t = INFINITY;

    // Loop through scene objects
    for(Sphere* sphere : sceneSpheres) {
        HitRecord rec;
        if(sphere->hit(ray, 0.001f, 100.f, rec)) {
            if(rec.t < finalRec.t) finalRec = rec;
        }
    }

    // Compute first object touched
    if(finalRec.t != INFINITY) {
        Ray scattered;
        glm::vec3 attenuation;
        if(callScatter(finalRec.type, finalRec.material.ambient, ray, finalRec, attenuation, scattered))
            return attenuation * rayValue(scattered, sceneSpheres, depth-1);
        
        return glm::vec3(0.f);
    }

    // Background
    glm::vec3 unitDirection = glm::normalize(ray.direction());
    float a = 0.5*(unitDirection.y + 1.0);
    return (1.f - a) * glm::vec3(1.f) + a * glm::vec3(0.5, 0.7, 1.0);
}

void RayTracing::writePixel(std::vector<unsigned char>& pixels, const unsigned int x, const unsigned int y, glm::vec4 RGBA)
{
    glm::vec4 corrected = glm::sqrt(RGBA);
    pixels[4 * IMAGE_WIDTH * y + 4 * x + 0] = corrected.x * 255;
    pixels[4 * IMAGE_WIDTH * y + 4 * x + 1] = corrected.y * 255;
    pixels[4 * IMAGE_WIDTH * y + 4 * x + 2] = corrected.z * 255;
    pixels[4 * IMAGE_WIDTH * y + 4 * x + 3] = RGBA.w * 255;
}

bool RayTracing::callScatter(const HitType& type, const glm::vec3& color, const Ray& rayIn, const HitRecord& record,
    glm::vec3& attenuation, Ray& scattered)
{
    switch (type) {
    case DIFFUSE: return scatterDiffuse(color, rayIn, record, attenuation, scattered);
    case METAL: return scatterMetal(color, rayIn, record, attenuation, scattered);
    case GLASS: return scatterDielectric(color, rayIn, record, attenuation, scattered);

    default:
        std::cout << "[ERROR] Error while reading ray-tracing type" << std::endl;
        std::cout << "[ERROR] Trace of HitRecord :" << std::endl;
        std::cout << "\t|- point : " << glm::to_string(record.point) << std::endl;
        std::cout << "\t|- normal : " << glm::to_string(record.normal) << std::endl;
        std::cout << "\t|- frontFace : " << record.frontFace << std::endl;
        std::cout << "\t|- type : " << record.type << std::endl;
        std::cout << std::endl;
        return false;
    }
}

bool RayTracing::scatterDiffuse(const glm::vec3 &color, const Ray &rayIn, const HitRecord &record,
    glm::vec3 &attenuation, Ray &scattered)
{
    glm::vec3 direction = record.normal + randomUnitVec3();
    
    scattered = Ray(record.point, direction);
    attenuation = color;
    return true;
}

bool RayTracing::scatterMetal(const glm::vec3 &color, const Ray &rayIn, const HitRecord &record,
    glm::vec3 &attenuation, Ray &scattered)
{
    glm::vec3 reflected = reflectVec3(rayIn.direction(), record.normal);
    reflected = glm::normalize(reflected) + (record.material.shininess * randomUnitVec3());

    scattered = Ray(record.point, reflected);
    attenuation = color;
    return (dot(scattered.direction(), record.normal) > 0);
}

bool RayTracing::scatterDielectric(const glm::vec3 &color, const Ray &rayIn, const HitRecord &record, glm::vec3 &attenuation, Ray &scattered)
{
    float ri = record.frontFace ? 1.f / record.material.shininess : record.material.shininess;
    glm::vec3 unitDirection = glm::normalize(rayIn.direction());
    glm::vec3 refracted = refractVec3(unitDirection, record.normal, ri);

    double cosTheta = std::fmin(glm::dot(-unitDirection, record.normal), 1.0);
    double sinTheta = std::sqrt(1.0 - cosTheta*cosTheta);

    bool cannotRefract = ri * sinTheta > 1.0;
    glm::vec3 direction;

    if (cannotRefract || reflectance(cosTheta, ri) > randomFloat())
        direction = reflectVec3(unitDirection, record.normal);
    else
        direction = refractVec3(unitDirection, record.normal, ri);

    scattered = Ray(record.point, direction);
    attenuation = glm::vec3(1.f);
    return true;
}
