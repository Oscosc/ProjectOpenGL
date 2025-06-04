#include "Mesh.hpp"

Mesh::Mesh(std::string filename)
{
    std::string lineBuffer;
    std::ifstream reader(filename);

    std::vector<unsigned int> counter;
    counter.push_back(0);
    counter.push_back(0);
    counter.push_back(0);

    while(std::getline(reader, lineBuffer)) {
        std::vector<std::string> tokens = split(lineBuffer, STD_DELIMITER);
        Mesh::LineType id = identify(tokens[0]);

        switch (id)
        {
        case LineType::VERTEX:
            this->m_vertices.push_back(glm::vec3(std::stof(tokens[1]),
                                                 std::stof(tokens[2]),
                                                 std::stof(tokens[3])));
            counter[0]++;
            break;

        case LineType::FACET:
            this->m_indexes.push_back(std::stoi(tokens[1]) - 1);
            this->m_indexes.push_back(std::stoi(tokens[2]) - 1);
            this->m_indexes.push_back(std::stoi(tokens[3]) - 1);
            counter[1]++;
            break;
        
        default: // => LineType::NONE || LineType::COMMENT
            counter[2]++;
            break;
        }
    }

    std::cout << "Mesh '" << filename << "' correctly loaded :" << std::endl;
    std::cout << "\t- " << counter[0] << " vertices" << std::endl;
    std::cout << "\t- " << counter[1] << " facets" << std::endl;
    std::cout << "\t- " << counter[2] << " others" << std::endl;

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


Mesh::LineType Mesh::identify(std::string token)
{
    if (token.length() != 1)    return LineType::NONE;
    else if (token[0] == '#')   return LineType::COMMENT;
    else if (token[0] == 'v')   return LineType::VERTEX;
    else if (token[0] == 'f')   return LineType::FACET;
    else                        return LineType::NONE;
}
