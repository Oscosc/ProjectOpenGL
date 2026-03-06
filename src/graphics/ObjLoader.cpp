#include <ProjectIGAI/graphics/ObjLoader.hpp>

#include <fstream>
#include <map>

#include <ProjectIGAI/core/utils.hpp>
#include <ProjectIGAI/core/Logger.hpp>

void ObjLoader::loadObject(std::string path, std::vector<Vertex> &vertices, std::vector<unsigned int> &indexes)
{
    std::string lineBuffer;
    std::ifstream reader(path);

    vec3Array positions;
    vec3Array normals;
    vec2Array uvs;
    std::vector<VertexIndex> vIndexes;

    // LECTURE DU FICHIER OBJ //

    while(std::getline(reader, lineBuffer)) {
        std::vector<std::string> tokens = split(lineBuffer, STD_DELIMITER);
        LineType id = identifyObjLineType(tokens[0]);

        switch (id) {
        case LineType::POSITION:
        case LineType::NORMAL:
        case LineType::UV:
            parseAsData(id, tokens, positions, normals, uvs);
            break;

        case LineType::INDEX:
            parseAsIndexes(id, tokens, vIndexes);
            break;
        
        default: // => LineType::NONE || LineType::COMMENT
            break;
        }
    }
    reader.close();

    if(normals.empty()) subComputeNormals(positions, normals, vIndexes);
    computeUniques(positions, normals, uvs, vIndexes, vertices, indexes);
}

LineType ObjLoader::identifyObjLineType(std::string token)
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

void ObjLoader::parseAsData(const LineType id, const std::vector<std::string> tokens,
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

void ObjLoader::parseAsIndexes(const LineType id, const std::vector<std::string> tokens,
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
            std::stoi(subTokens[0]) - 1,                             // [0] Position
            subTokens.size() < 2 ? -1 : std::stoi(subTokens[2]) - 1, // [2] Normal
            subTokens.size() < 3 ? -1 : std::stoi(subTokens[1]) - 1  // [1] UV
        });
    }
}

void ObjLoader::subComputeNormals(const vec3Array &positions, vec3Array &normals, std::vector<VertexIndex> &indexes,
    const bool invertNormals)
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

        normals[i0] += invertNormals ? -faceNormal : faceNormal;
        normals[i1] += invertNormals ? -faceNormal : faceNormal;
        normals[i2] += invertNormals ? -faceNormal : faceNormal;
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

void ObjLoader::computeUniques(
    const vec3Array &positions,
    const vec3Array &normals,
    const vec2Array &uvs,
    const std::vector<VertexIndex> &indexes,
    std::vector<Vertex>& vertexBuffer,
    std::vector<unsigned int>& elementBuffer)
{   
    std::map<std::tuple<int, int, int>, unsigned int> uniqueVertices;

    // Réservation mémoire
    vertexBuffer.reserve(indexes.size());
    elementBuffer.reserve(indexes.size());

    for(VertexIndex index : indexes) {

        // Recherche d'un vertex déjà existant
        std::tuple<int, int, int> key = std::make_tuple(index.position, index.normal, index.uv);
        auto it = uniqueVertices.find(key);

        // Réutilisation si trouvé
        if(it != uniqueVertices.end()) {
            elementBuffer.push_back(it->second);
        }

        // Sinon, construction du vertex
        else {
            Vertex item;
            item.position = (index.position != -1) ? positions[index.position] : glm::vec3(0.0f);
            item.normal = (index.normal != -1) ? normals[index.normal] : glm::vec3(0.0f);
            item.tangent = glm::vec3(0.f); // Fixme
            item.uv = (index.uv != -1) ? uvs[index.uv] : glm::vec2(0.0f);

            unsigned int newIndex = (unsigned int)vertexBuffer.size();
            vertexBuffer.push_back(item);

            uniqueVertices[key] = newIndex;
            elementBuffer.push_back(newIndex);
        }
    }
}