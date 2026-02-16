#include <ProjectIGAI/graphics/Mesh.hpp>

#include <map>
#include <tuple>
#include <ProjectIGAI/core/Logger.hpp>
#include <ProjectIGAI/graphics/GeometryManager.hpp>

Mesh::Mesh(std::string file, Transform transform, std::string name, Material material) :
    m_filename(file), Object(transform, name, material)
{
    m_geometry = GeometryManager::getInstance().getMesh(file);
}

void Mesh::draw(Scene* scene) const
{
    Shader* shader = this->getMaterial().shader;
    if(shader == nullptr) {
        Logger::logError("No shader instanciated for this object");
    }
    shader->use();

    glm::mat4 model = getModelMatrix();

    shader->setMat4("model", model);
    shader->setMat4("view", scene->getActiveCameraPV().view);
    shader->setMat4("projection", scene->getActiveCameraPV().projection);

    updateMaterial(shader);
    
    scene->updateLigth(shader);

    bindTexture(shader, scene);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if(m_geometry) m_geometry->draw();
}

void Mesh::displayInformations() const
{
    std::string normals = hasNormals() ? "YES" : "NO";
    std::string uvs = hasUVs() ? "YES" : "NO";

    std::cout << "Object \"" << getName() << "\"" << std::endl;
    std::cout << "  |- Normals : " << normals << std::endl;
    std::cout << "  |- UVs : " << uvs << std::endl;
    std::cout << "  |- " << this->m_vertices.size() << " vertices computed" << std::endl;
    std::cout << "  |- " << this->m_indexes.size() << " indexes computed" << std::endl;
    std::cout << "  |- Position : " << glm::to_string(this->getTransform().position) << std::endl;
    std::cout << "  |- Rotation : " << glm::to_string(this->getTransform().rotation) << std::endl;
    std::cout << "  |- Scale : " << glm::to_string(this->getTransform().scale) << std::endl;

    debugMaterial();

    std::cout << std::endl;
}

bool Mesh::hasNormals() const
{
    return this->m_hasNormals;
}

bool Mesh::hasUVs() const
{
    return this->m_hasUVs;
}

std::string Mesh::getName() const
{
    return this->m_filename;
}

std::vector<Vertex> Mesh::getVertices() const
{
    return m_vertices;
}

std::vector<unsigned int> Mesh::getIndexes() const
{
    return m_indexes;
}