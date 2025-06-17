#include "Mesh.hpp"

Mesh::Mesh(std::string filename)
{
    std::string lineBuffer;
    std::ifstream reader(filename);
    this->m_filename = filename;

    vec3Array positions;
    vec3Array normals;
    vec2Array uvs;
    std::vector<VertexIndex> indexes;

    // LECTURE DU FICHIER OBJ //

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
            break;
        
        default: // => LineType::NONE || LineType::COMMENT
            break;
        }
    }
    reader.close();

    // INITIALISATION DE L'OBJET //

    this->m_hasNormals = !normals.empty();
    this->m_hasUVs = !uvs.empty();
    computeUniques(positions, normals, uvs, indexes);

    glGenVertexArrays(1, &this->m_VAO);
    glGenBuffers(1, &this->m_VBO);
    glGenBuffers(1, &this->m_EBO);

    glBindVertexArray(this->m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        this->m_vertices.size() * sizeof(Vertex),
        this->m_vertices.data(),
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glEnableVertexAttribArray(0);
    if(this->hasNormals()) glEnableVertexAttribArray(1);
    if(this->hasUVs()) glEnableVertexAttribArray(2);

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

void Mesh::displayInformations()
{
    std::string normals = hasNormals() ? "YES" : "NO";
    std::string uvs = hasUVs() ? "YES" : "NO";

    std::cout << "Object \"" << getName() << "\"" << std::endl;
    std::cout << "\tNormals : " << normals << std::endl;
    std::cout << "\tUVs : " << uvs << std::endl;
    std::cout << this->m_vertices.size() << " vertices computed" << std::endl;
    std::cout << this->m_indexes.size() << " indexes computed" << std::endl;
}

bool Mesh::hasNormals()
{
    return this->m_hasNormals;
}

bool Mesh::hasUVs()
{
    return this->m_hasUVs;
}

std::string Mesh::getName()
{
    return this->m_filename;
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

    bool i = false;
    for(std::string token : tokens) {
        if(!i) {i = true; continue;}

        std::vector<std::string> subTokens = split(token, IDX_DELIMITER);
        if(subTokens.size() <= 0 || subTokens.size() > 3) {
            std::cout << "[ERREUR] Erreur rencontrée lors du parsing des index" << std::endl;
            exit(4);
        }

        indexes.push_back({
            std::stoi(subTokens[0]) - 1,
            subTokens.size() < 2 ? -1 : std::stoi(subTokens[1]) - 1,
            subTokens.size() < 3 ? -1 : std::stoi(subTokens[2]) - 1
        });
    }
}

void Mesh::computeUniques(const vec3Array &positions, const vec3Array &normals,
    const vec2Array &uvs, const std::vector<VertexIndex> &indexes)
{   
    std::vector<Vertex> vertexBuffer;
    std::vector<Vertex>::iterator it;
    std::vector<unsigned int> elementBuffer;
    unsigned int itPos;

    for(VertexIndex index : indexes) {
        // Construction du Vertex
        Vertex item {
            (index.position != -1) ? positions[index.position] : glm::vec3(0.0f),
            (index.normal != -1) ? normals[index.normal] : glm::vec3(0.0f),
            (index.uv != -1) ? uvs[index.uv] : glm::vec2(0.0f)
        };

        // Recher du Vertex
        it = std::find(vertexBuffer.begin(), vertexBuffer.end(), item);
        if(it != vertexBuffer.end()) {
            itPos = std::distance(vertexBuffer.begin(), it);
        }
        else {
            itPos = vertexBuffer.size();
            vertexBuffer.push_back(item);
        }
        elementBuffer.push_back(itPos);
    }

    // Mise à jour des attributs
    this->m_vertices = vertexBuffer;
    this->m_indexes = elementBuffer;
}
