#include <ProjectIGAI/graphics/Sphere.hpp>

#include <ProjectIGAI/core/Logger.hpp>

Sphere::Sphere(float radius, Transform transform, std::string name, Material material)
    : m_radius(radius), Object(transform, name, material)
{
    unsigned int stacks = (radius > 1.0f) ? DEFAULT_STACKS : round(DEFAULT_STACKS * radius);
    unsigned int sectors = (radius > 1.0f) ? DEFAULT_SECTORS : round(DEFAULT_SECTORS * radius);

    this->m_vertices = generateVertices(stacks, sectors, m_radius);
    this->m_indexes = generateIndexes(stacks, sectors);

    this->m_hasNormals = true;
    this->m_hasUVs = true;

    initGLObject();
}

bool Sphere::hit(const Ray &ray, float tMin, float tMax, HitRecord &record) const
{
    glm::vec3 oc = m_transform.position - ray.origin();
    
    float a = lengthSquared(ray.direction());
    float h = glm::dot(ray.direction(), oc);
    float c = lengthSquared(oc) - m_radius * m_radius;
    float discriminant = h*h - a*c;

    if(discriminant < 0) return false;

    float sqrtd = std::sqrt(discriminant);
    float root = (h - sqrtd) / a;
    if(root <= tMin || tMax <= root) {
        root = (h + sqrtd) / a;
        if(root <= tMin || tMax <= root)
            return false;
    }

    record.t = root;
    record.point = ray.at(record.t);
    record.setFaceNormal(ray, (record.point - m_transform.position) / m_radius);
    record.material = this->m_material.matShader;
    record.type = this->Type;
    return true;
}

void Sphere::draw(Scene* scene) const
{
    Shader* shader = this->getMaterial().shader;
    shader->use();

    glm::mat4 model = getModelMatrix();

    shader->setMat4("model", model);
    shader->setMat4("view", scene->getActiveCameraPV().view);
    shader->setMat4("projection", scene->getActiveCameraPV().projection);

    updateMaterial(shader);

    scene->updateLigth(shader);

    glBindVertexArray(this->m_VAO);
    glDrawElements(GL_TRIANGLES, this->m_indexes.size(), GL_UNSIGNED_INT, (void*)0);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        Logger::logError("in Sphere : GLError " + std::to_string(err));
    }
}


float Sphere::getRadius() const
{
    return m_radius;
}


std::vector<Vertex> Sphere::generateVertices(unsigned int stackCount, unsigned int sectorCount, float radius)
{
    std::vector<Vertex> vertices;

    float x, y, z, xy;                              // vertex position

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

            vertices.push_back({
                glm::vec3(x, y, z), // Position
                glm::normalize(glm::vec3(x, y, z)), // Normal
                glm::vec3(float(j)/sectorCount, float(i)/stackCount, 0.0f) // UV
            });
        }
    }

    return vertices;
}


std::vector<unsigned int> Sphere::generateIndexes(unsigned int stackCount, unsigned int sectorCount)
{
    std::vector<unsigned int> indices;

    int k1, k2;

    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return indices;
}
