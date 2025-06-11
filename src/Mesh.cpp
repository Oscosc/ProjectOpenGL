#include "Mesh.hpp"

Mesh::Mesh(std::string filename)
{
    std::string lineBuffer;
    std::ifstream reader(filename);

    vec3Array positions;
    vec3Array normals;
    vec2Array uvs;
    std::vector<VertexIndex> indexes;

    while(std::getline(reader, lineBuffer)) {
        std::vector<std::string> tokens = split(lineBuffer, STD_DELIMITER);
        Mesh::LineType id = identify(tokens[0]);

        switch (id) {
        case LineType::POSITION:
        case LineType::NORMAL:
        case LineType::UV:
            parseAsData(id, tokens, positions, normals, uvs);
            break;

        case LineType::INDEX:
            parseAsIndexes(id, tokens, indexes);
            // TODO
            break;
        
        default: // => LineType::NONE || LineType::COMMENT
            break;
        }
    }

    reader.close();

    glGenVertexArrays(1, &this->m_VAO);
    glGenBuffers(1, &this->m_VBO);
    glGenBuffers(1, &this->m_EBO);

    glBindVertexArray(this->m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        this->m_vertices.size() * sizeof(glm::vec3),
        this->m_vertices.data(),
        GL_DYNAMIC_DRAW
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        this->m_indexes.size() * sizeof(unsigned int),
        this->m_indexes.data(),
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Mesh::draw(Shader shader)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), this->m_origin);
    shader.setMat4("model", model);

    shader.setVec3("color", glm::vec3(1.f)); // TODO : color

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(this->m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_EBO);
    glDrawElements(GL_TRIANGLES, this->m_indexes.size(), GL_UNSIGNED_INT, (void*)0);
}

bool Mesh::hasNormals()
{
    return this->m_hasNormals;
}

bool Mesh::hasUVs()
{
    return this->m_hasUVs;
}

Mesh::LineType Mesh::identify(std::string token)
{
    if(token.length() == 1) {
        if      (token[0] == '#') return LineType::COMMENT;
        else if (token[0] == 'v') return LineType::POSITION;
        else if (token[0] == 'f') return LineType::INDEX;
        else                      return LineType::NONE;
    }
    if(token.length() == 2 && token[0] == 'v') {
        if      (token[1] == 'n') return LineType::NORMAL;
        else if (token[1] == 't') return LineType::UV;
        else                      return LineType::NONE;
    }
    else return LineType::NONE;
}

void Mesh::parseAsData(const Mesh::LineType id, const std::vector<std::string> tokens,
    vec3Array &positions, vec3Array &normals, vec2Array &uvs)
{
    if(tokens.size() != 3 && tokens.size() != 4) {
        std::cout << "[ERREUR] Le fichier .obj est malformé ou corrompu" << std::endl;
        exit(1);
    }

    switch (id)
    {
    case LineType::POSITION:
        positions.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
        break;

    case LineType::NORMAL:
        normals.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
        break;

    case LineType::UV:
        uvs.push_back(glm::vec2(std::stof(tokens[1]), std::stof(tokens[2])));
        break;
    
    default:
        std::cout << "[ERREUR] Erreur rencontrée lors du parsing de l'id" << std::endl;
        exit(2);
        break;
    }
}

void Mesh::parseAsIndexes(const LineType id, const std::vector<std::string> tokens,
    std::vector<VertexIndex> &indexes)
{
    if(tokens.size() != 4) {
        std::cout << "[WARNING] Seuls les meshs construits avec des triangles sont supportés"
            << std::endl;
        std::cout << "[ERREUR] Le fichier .obj est malformé" << std::endl;
        exit(3);
    }

    unsigned int i = 0;
    for(std::string token : tokens) {
        if(i == 0) continue;

        std::vector<std::string> sub_tokens = split(token, IDX_DELIMITER);
        if(sub_tokens.size() <= 0 || sub_tokens.size() > 3) {
            std::cout << "[ERREUR] Erreur rencontrée lors du parsing des index" << std::endl;
            exit(4);
        }

        VertexIndex index {
            std::stof(sub_tokens[0]) - 1,
            sub_tokens.size() < 2 ? -1 : std::stof(sub_tokens[1]) - 1,
            sub_tokens.size() < 3 ? -1 : std::stof(sub_tokens[2]) - 1
        };
    }
}

void Mesh::computeUniques(const vec3Array &positions, const vec3Array &normals,
    const vec2Array &uvs, const std::vector<VertexIndex> &indexes)
{
    /* TODO : Pour chaque élément dans la liste indexes :
        - Si l'élément existe déjà dans la liste de Vertex finale, ajouter son indice dans la liste
        des indexes pour l'EBO
        - Si l'élément n'existe pas déjà dans la liste de Vertex finale, le construire avec les
        éléments des différentes listes en paramètre, puis l'ajouter dans la liste de Vertex finale
        et ajouter son indice dans la liste des indexes pour l'EBO
    */
}
