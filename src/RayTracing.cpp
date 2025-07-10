#include "RayTracing.hpp"

#include "utils.hpp"
#include "Sphere.hpp"
#include "../includes/progressbar.hpp"

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

    progressbar bar(IMAGE_HEIGHT);
    for(unsigned int h = 0; h < IMAGE_HEIGHT; ++h) {
        for(unsigned int w = 0; w < IMAGE_WIDTH; ++w) {
            computePixel(w, h, scene, tanHalfFOV, cameraToWorld, pixels, sceneSpheres);
        }

        bar.update();
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

glm::vec3 RayTracing::rayValue(Ray &ray, const std::vector<Sphere*>& sceneSpheres)
{
    for(Sphere* sphere : sceneSpheres) {
        HitRecord rec;
        if(sphere->hit(ray, 0.f, 100.f, rec)) {
            return 0.5f * (rec.normal + 1.0f);
        }
    }

    // Background
    float a = 0.5*(ray.direction().y + 1.0);
    return (1.f - a) * glm::vec3(1.f) + a * glm::vec3(0.5, 0.7, 1.0);
}

void RayTracing::writePixel(std::vector<unsigned char>& pixels, const unsigned int x, const unsigned int y, glm::vec4 RGBA)
{
    pixels[4 * IMAGE_WIDTH * y + 4 * x + 0] = RGBA.x * 255;
    pixels[4 * IMAGE_WIDTH * y + 4 * x + 1] = RGBA.y * 255;
    pixels[4 * IMAGE_WIDTH * y + 4 * x + 2] = RGBA.z * 255;
    pixels[4 * IMAGE_WIDTH * y + 4 * x + 3] = RGBA.w * 255;
}