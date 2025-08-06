#include "Mesh.hpp"
#include "Logger.hpp"

Mesh::Mesh(std::string file, Transform transformation, Material material) :
    m_filename(file), Object(transformation, material)
{
    loadInitMesh(file);
}

void Mesh::draw(Scene* scene) const
{
    Shader* shader = this->getMaterial().shader;
    if(shader == nullptr) {
        Logger::logError("No shader instanciated for this object");
    }
    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(glm::mat4(1.0f), this->m_transform.position);
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.x), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.y), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(this->m_transform.rotation.z), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, this->m_transform.scale);

    shader->setMat4("model", model);
    shader->setMat4("view", scene->getActiveCameraPV().view);
    shader->setMat4("projection", scene->getActiveCameraPV().projection);

    updateMaterial(shader);
    
    scene->updateLigth(shader);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(this->m_VAO);
    glDrawElements(GL_TRIANGLES, this->m_indexes.size(), GL_UNSIGNED_INT, (void*)0);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        Logger::logError("in Mesh drawing : GLError "  + std::to_string(err));
    }
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
    std::cout << "  |- Position :" << glm::to_string(this->getTransform().position) << std::endl;
    std::cout << "  |- Rotation :" << glm::to_string(this->getTransform().rotation) << std::endl;
    std::cout << "  |- Scale :" << glm::to_string(this->getTransform().scale) << std::endl;

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

Mesh::LineType Mesh::identify(std::string token) const
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

void Mesh::loadInitMesh(std::string filename)
{
    std::string lineBuffer;
    std::ifstream reader(filename);

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
    
    if(!hasNormals()) subComputeNormals(positions, normals, indexes);
    this->m_hasNormals = true;

    computeUniques(positions, normals, uvs, indexes);

    initGLObject();

    // DEBUG
    // displayInformations();
}

void Mesh::parseAsData(const Mesh::LineType id, const std::vector<std::string> tokens,
    vec3Array &positions, vec3Array &normals, vec2Array &uvs)
{
    if(tokens.size() != 3 && tokens.size() != 4) {
        Logger::logError("Le fichier .obj est malformé ou corrompu");
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
        Logger::logError("Erreur rencontrée lors du parsing de l'id");
        exit(2);
        break;
    }
}

void Mesh::parseAsIndexes(const LineType id, const std::vector<std::string> tokens,
    std::vector<VertexIndex> &indexes)
{
    if(tokens.size() != 4) {
        Logger::logWarning("Seuls les meshs construits avec des triangles sont supportés");
        Logger::logError("Le fichier .obj est malformé");
        exit(3);
    }

    bool i = false;
    for(std::string token : tokens) {
        if(!i) {i = true; continue;}

        std::vector<std::string> subTokens = split(token, IDX_DELIMITER);
        if(subTokens.size() <= 0 || subTokens.size() > 3) {
            Logger::logError("Erreur rencontrée lors du parsing des index");
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
    std::unordered_map<Vertex, unsigned int> vertexToIndex;
    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> elementBuffer;

    // Creation des normales si nécessaire

    for(VertexIndex index : indexes) {
        // Construction du Vertex
        Vertex item {
            (index.position != -1) ? positions[index.position] : glm::vec3(0.0f),
            (index.normal != -1) ? normals[index.normal] : glm::vec3(0.0f),
            (index.uv != -1) ? uvs[index.uv] : glm::vec2(0.0f)
        };

        // Recherche du Vertex
        auto it = vertexToIndex.find(item);
        if(it != vertexToIndex.end()) {
            elementBuffer.push_back(it->second); // (key, -> value <-)
        }
        else {
            unsigned int newIndex = vertexBuffer.size();
            vertexBuffer.push_back(item);
            vertexToIndex[item] = newIndex;
            elementBuffer.push_back(newIndex);
        }
    }

    // Mise à jour des attributs
    this->m_vertices = std::move(vertexBuffer);
    this->m_indexes = std::move(elementBuffer);
}

void Mesh::subComputeNormals(const vec3Array &positions, vec3Array &normals, std::vector<VertexIndex> &indexes)
{
    normals.resize(positions.size(), glm::vec3(0.0f)); // Une normale par sommet

    // Accumulation des normales par sommet
    for (unsigned int i = 0; i + 2 < indexes.size(); i += 3) {
        int i0 = indexes[i].position;
        int i1 = indexes[i + 1].position;
        int i2 = indexes[i + 2].position;

        const glm::vec3 &v0 = positions[i0];
        const glm::vec3 &v1 = positions[i1];
        const glm::vec3 &v2 = positions[i2];

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        normals[i0] += faceNormal;
        normals[i1] += faceNormal;
        normals[i2] += faceNormal;
    }

    // Normalisation des normales par sommet
    for (glm::vec3 &n : normals) {
        if (glm::length(n) > 0.0f)
            n = glm::normalize(n);
        else
            n = glm::vec3(0.0f, 1.0f, 0.0f); // Normale par défaut si nécessaire
    }

    // Mise à jour des indexes pour pointer vers la normale du sommet (identique à l’index de position)
    for (VertexIndex &vi : indexes) {
        vi.normal = vi.position;
    }
}