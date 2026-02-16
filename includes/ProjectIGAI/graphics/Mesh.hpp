#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <ProjectIGAI/core/utils.hpp>
#include <ProjectIGAI/graphics/Object.hpp>

#define STD_DELIMITER " "
#define IDX_DELIMITER "/"

/**
 * @brief Representation of a mesh in the sense of renderable object.
 * 
 * A mesh is constructed by reading a .obj file containing information about it's vertices.
 */
class Mesh : public Object
{
public:

    /**
     * @brief Construct a new Mesh with a .obj file.
     * 
     * @param file path to the .obj file
     * @param transform transformation of the Mesh in the scene world
     * @param material material used to render the Mesh in the application
     */
    Mesh(std::string file,
        Transform transform = DEFAULT_TRANSFORM,
        std::string name = DEFAULT_NAME,
        Material material = DEFAULT_OBJECT_MATERIAL);

    /**
     * @brief Draw the mesh in the scene.
     * 
     * @param scene Scene (and by consequent the window) where the mesh will be drawn
     */
    void draw(Scene* scene) const override;

    /**
     * @brief Display mesh properties in logs for debug purposes
     */
    void displayInformations() const;

    /**
     * @brief Give information about the disponibility of the normals for this mesh.
     */
    bool hasNormals() const;

    /**
     * @brief Give information about the disponibility of the UVs for this mesh.
     */
    bool hasUVs() const;

    /**
     * @brief Return the filename used for this mesh.
     */
    std::string getName() const;

    std::vector<Vertex> getVertices() const;

    std::vector<unsigned int> getIndexes() const;

private:
    
    std::string m_filename;
};

#endif // MESH_HPP