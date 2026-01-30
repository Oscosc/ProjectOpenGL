#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <math.h>

#include <ProjectIGAI/graphics/Object.hpp>
#include <ProjectIGAI/raytracing/Hittable.hpp>
#include <ProjectIGAI/core/utils.hpp>

#define DEFAULT_STACKS 64
#define DEFAULT_SECTORS 64

/**
 * @brief Class used to define a Sphere object that can be rendered on the screen.
 */
class Sphere : public Object, public Hittable
{
public:

    /**
     * @brief Construct Vertices of a sphere and save it as an object
     * 
     * @param radius Radius of the sphere to create
     * @param transform Sphere default transformation
     * @param material Sphere default material
     */
    Sphere(float radius,
        Transform transform = DEFAULT_TRANSFORM,
        Material material = DEFAULT_OBJECT_MATERIAL
    );

    bool hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;

    /**
     * @brief Draw sphere in the scene (in consequent on the linked app window).
     * 
     * @param scene scene where the sphere is rendered
     */
    void draw(Scene* scene) const override;

    /**
     * @brief Gives the radius of the sphere.
     */
    float getRadius() const;

private:

    /**
     * @brief Code from : https://www.songho.ca/opengl/gl_sphere.html#sphere
     */
    static std::vector<Vertex> generateVertices(unsigned int stackCount, unsigned int sectorCount, float radius);

    /**
     * @brief Code also from : https://www.songho.ca/opengl/gl_sphere.html#sphere
     */
    static std::vector<unsigned int> generateIndexes(unsigned int stackCount, unsigned int sectorCount);
    
    float m_radius;
};

#endif // SPHERE_HPP