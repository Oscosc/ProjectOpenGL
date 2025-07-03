#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "utils.hpp"
#include "Object.hpp"
#include "../includes/shader.hpp"

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
     * @brief Enumeration for representing types of lines in a .obj file
     * 
     * This enumeration is used to call the right function in the parsing context.
     */
    enum LineType {
        COMMENT,    // Starts with '#'
        POSITION,   // Starts with 'v'
        NORMAL,     // Starts with 'vn'
        UV,         // Starts with 'vt'
        INDEX,      // Starts with 'f'
        NONE        // Default value
    };

    /**
     * @brief Construct a new Mesh with a .obj file.
     * 
     * @param file path to the .obj file
     * @param transform transformation of the Mesh in the scene world
     * @param material material used to render the Mesh in the application
     */
    Mesh(std::string file,
        Transform transform = DEFAULT_OBJECT_TRANSFORM,
        Material material = DEFAULT_OBJECT_MATERIAL
    );

    /**
     * @brief Draw the mesh in the scene.
     * 
     * @param scene Scene (and by consequent the window) where the mesh will be drawn
     */
    void draw(Scene* scene) override;

    /**
     * @brief Display mesh properties in logs for debug purposes
     */
    void displayInformations();

    /**
     * @brief Give information about the disponibility of the normals for this mesh.
     */
    bool hasNormals();

    /**
     * @brief Give information about the disponibility of the UVs for this mesh.
     */
    bool hasUVs();

    /**
     * @brief Return the filename used for this mesh.
     */
    std::string getName();

private:

    /**
     * @brief Identify the type of line based on the first token, according to .obj documentation.
     * 
     * @param token fisrt token of the line obtained after line parsing
     */
    LineType identify(std::string token);

    /**
     * @brief Main fuction called while creating a Mesh object to compute vertices and init GL
     * properties.
     * 
     * @param filename filename of the mesh (.obj)
     */
    void loadInitMesh(std::string filename);
    
    /**
     * @brief Parses the line as a line with data information
     * (position, normal, or UV). Updates the list corresponding to the identified line type.
     *
     * WARNING: It is assumed that the function is called correctly based on the parsed ID.
     * Otherwise, the behavior is undefined.
     *
     * @param id Line identifier (token 0)
     * @param tokens List of tokens in the line (including token 0!)
     * @param positions List of positions already read since the beginning of mesh construction
     * @param normals List of normals already read since the beginning of mesh construction
     * @param uvs List of UVs already read since the beginning of mesh construction
     */
    void parseAsData(const LineType id, const std::vector<std::string> tokens,
        vec3Array &positions, vec3Array &normals, vec2Array &uvs);

    /**
     * @brief Parses the line as a line with index information.
     * Updates the list of indexes passed as a parameter for future construction.
     *
     * @param id Line identifier (token 0)
     * @param tokens List of tokens in the line (including token 0!)
     * @param indexes List of indexes already read since the start of mesh construction
     */
    void parseAsIndexes(const LineType id, const std::vector<std::string> tokens,
        std::vector<VertexIndex> &indexes);

    /**
     * @brief Constructs the final vertex list that will compose the mesh from the lists of
     * positions, normals, UVs, and indexes extracted in the parsing functions when
     * reading the mesh.
     *
     * This function does not require the .obj file to be open to work.
     *
     * @param positions List of positions read from the .obj file during parsing
     * @param normals List of normals read from the .obj file during parsing
     * @param uvs List of UVs read from the .obj file during parsing
     * @param indexes List of indexes read from the .obj file during parsing
     */
    void computeUniques(const vec3Array &positions, const vec3Array &normals, const vec2Array &uvs,
        const std::vector<VertexIndex> &indexes);

    /**
     * @brief If normals are not defined in the .obj file, this function will be called to generate
     * default normals for this mesh.
     * 
     * @param positions positions of the vertices loaded, used to define normals of triangles
     * @param normals normal array, empty at the beggining but filled during the process
     * @param indexes indexes of the Vertices, will be updated during the process to reflect
     * normals informations
     */
    void subComputeNormals(const vec3Array &positions, vec3Array &normals,
        std::vector<VertexIndex> &indexes);
    
    std::string m_filename;
};

#endif // MESH_HPP