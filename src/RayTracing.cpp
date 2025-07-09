#include "RayTracing.hpp"

#include "utils.hpp"

#include <chrono>
#define timer std::chrono::high_resolution_clock
#define duration std::chrono::duration_cast<std::chrono::milliseconds>

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600

void RayTracing::computeImage(const std::string &filename, Scene& scene)
{
    auto timerA = timer::now();

    std::vector<unsigned char> pixels;
    pixels.resize(IMAGE_WIDTH * IMAGE_HEIGHT * 4);

    for(unsigned int w = 0; w < IMAGE_WIDTH; ++w) {
        for(unsigned int h = 0; h < IMAGE_HEIGHT; ++h) {
            Ray ray = Ray(w, h, IMAGE_WIDTH, IMAGE_HEIGHT,
                scene.getActiveCamera()->Fov,
                scene.getActiveCamera()->GetViewMatrix(),
                scene.getActiveCamera()->Position
            );
            glm::vec3 rayColor = rayValue(ray, scene);
            writePixel(pixels, w, h, glm::vec4(rayColor, 1.0));
        }
    }

    auto timerB = timer::now();
    std::cout << "[PERFORMANCE] Ray-tracing computing time : " << duration(timerB - timerA).count() << " ms" << std::endl;

    savePNG(pixels, IMAGE_WIDTH, IMAGE_HEIGHT, filename);

    timerA  = timer::now();
    std::cout << "[PERFORMANCE] File saving computing time : " << duration(timerA - timerB).count() << " ms" << std::endl;
}

glm::vec3 RayTracing::rayValue(Ray &ray, Scene& scene)
{
    for(unsigned int i = 0; i < scene.objectsCount(); ++i) {
        Sphere* sphere = dynamic_cast<Sphere*>(scene.getObject(i));
        if(sphere != nullptr) {
            float t = ray.hitSphere(*sphere);
            if(t > 0.f) {
                glm::vec3 N = glm::normalize(ray.at(t) - sphere->getTransform().position);
                return 0.5f * (N + 1.0f);
            }
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