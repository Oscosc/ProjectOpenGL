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
#include "../includes/shader.hpp"

#define STD_DELIMITER " "
#define IDX_DELIMITER "/"

using vec3Array = std::vector<glm::vec3>;
using vec2Array = std::vector<glm::vec2>;

/**
 * @brief Représentation complète d'un vertex au sens graphique.
 * Encapsule la position, la normale et les UVs, ainsi que l'opérateur d'égalité.
 * 
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    /**
     * @brief Implémentation de l'opérateur d'égalité pour les Vertex
     * 
     * Si la position, la normale et les UV sont égaux, alors les Vertex sont égaux. Sinon non.
     * 
     * @param other Vertex à comparer
     * @return true si les deux Vertex sont les mêmes, false sinon
     */
    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

/**
 * @brief Représentation d'un indexe pour un vertex. Cette structure est une structure
 * intermédiaire avant de former une liste de Vertex qui sera passée au VAO.
 * 
 */
struct VertexIndex {
    int position;
    int normal;
    int uv;
};

/**
 * @brief Représentation d'un Mesh au sens d'un objet graphique qui peut être rendu.
 * 
 * Le mesh est construit à partir d'un fichier .obj dont le chemin d'accès est passé en paramètre.
 * 
 */
class Mesh
{
public:

    /**
     * @brief Enumération pour représenter les différents types de lignes dans un fichier .obj
     * 
     * Cette enum sert uniquement pour appeler la bonne méthode dans les fonctions de parsing.
     * 
     */
    enum LineType {
        COMMENT,    // Commence par '#'
        POSITION,   // Commence par 'v'
        NORMAL,     // Commence par 'vn'
        UV,         // Commence par 'vt'
        INDEX,      // Commence par 'f'
        NONE        // Valeur par défaut
    };

    /**
     * @brief Construit un nouveau Mesh à partir d'un fichier .obj
     * 
     * @param file chemin d'accès vers le fichier .obj
     */
    Mesh(std::string file);

    /**
     * @brief "Dessine" le mesh à l'écran (au sens graphique) en s'appuyant sur le shader passé en
     * paramètre.
     * 
     * @param shader shader utilisé pour afficher le mesh. Le shader doit accepter les mêmes
     * attributs de vertex que ceux du mesh. Par défaut, si vous ne connaissez pas les attributs
     * du mesh chargé, il est conseillé d'activer la position, la normale et les UVs.
     */
    void draw(Shader shader);

    /**
     * @brief Affiche les propriétés du mesh dans la console (pour du debug)
     * 
     */
    void displayInformations();

    /**
     * @brief Donne l'information d'état des normales du mesh
     * 
     * @return état des normales (on/off)
     */
    bool hasNormals();

    /**
     * @brief Donne l'information d'état des UVs du mesh
     * 
     * @return état des UVs (on/off)
     */
    bool hasUVs();

    /**
     * @brief Retourne le nom du fichier contenant les infos sur l'objet
     * 
     * @return nom de l'objet
     */
    std::string getName();

private:

    /**
     * @brief Identifie le type de ligne à partir du token passé en entrée
     * 
     * @param token premier token de la ligne lue dans le .obj
     * @return type de ligne trouvée, NONE si inconnue ou non implémenté
     */
    LineType identify(std::string token);
    
    /**
     * @brief Parse la ligne en la considérant comme une ligne avec de l'information de donnée
     * (position, normale ou UV). Mets à jour la liste correspondant au type de ligne identifié.
     * 
     * ATTENTION : On suppose que la fonction est appelée correctement par rapport à l'ID parsé.
     * Sinon, le comportement est indéterminé
     * 
     * @param id identifiant de la ligne (token 0)
     * @param tokens liste des tokens de la ligne (token 0 compris !)
     * @param positions liste des positions déjà lues depuis le début de la construction mesh
     * @param normals liste des normales déjà lues depuis le début de la construction du mesh
     * @param uvs liste des UVs déjà lus depuis le début de la construction du mesh
     */
    void parseAsData(const LineType id, const std::vector<std::string> tokens,
        vec3Array &positions, vec3Array &normals, vec2Array &uvs);

    /**
     * @brief Parse la ligne en la considérant comme une ligne avec de l'information sur les index.
     * Mets à jour la liste des indexes passée en paramètre pour la construction future.
     * 
     * @param id identifiant de la ligne (token 0)
     * @param tokens liste des tokens de la ligne (token 0 compris !)
     * @param indexes liste des indexes déjà lus depuis le début de la construction du mesh
     */
    void parseAsIndexes(const LineType id, const std::vector<std::string> tokens,
        std::vector<VertexIndex> &indexes);

    /**
     * @brief Construit la liste de vertex finale qui composera le mesh à partir des listes de
     * positions, normales, UVs et indexes extraites dans les fonctions de parsing lors de la
     * lecture du mesh.
     * 
     * Cette fonction n'a pas besoin que le fichier .obj soit ouverte pour fonctionner.
     * 
     * @param positions liste des positions lues dans le fichier .obj lors du parsing
     * @param normals liste des normales lues dans le fichier .obj lors du parsing
     * @param uvs liste des UVs lus dans le fichier .obj lors du parsing
     * @param indexes liste des indexes lus dans le fichier .obj lors du parsing
     */
    void computeUniques(const vec3Array &positions, const vec3Array &normals, const vec2Array &uvs,
        const std::vector<VertexIndex> &indexes);
    

    /*********************************************************************************************
     **                                        ATTRIBUTS                                        **
     *********************************************************************************************/

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    bool m_hasNormals;
    bool m_hasUVs;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indexes;
    std::string m_filename;

    glm::vec3 m_origin;
};

#endif // MESH_HPP