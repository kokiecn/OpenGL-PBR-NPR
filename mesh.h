#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Material {
    glm::vec4 Ka;
    glm::vec4 Kd;
    glm::vec4 Ks;
    float alpha;

};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // meshデータ
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    Material mats;
    unsigned int VAO;
    unsigned int uniformBlockIndex;

    //コンストラクタ
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material mat)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->mats = mat;
        setupMesh();
        
    }

    /**
     * @brief　meshを描画する巻数
     * @param shader シェーダー
     */
    void Draw(Shader& shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int metalnessNr = 1;

        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); 
            string number;
            string name = textures[i].type;
            if (name == "albedoMap")
                number = std::to_string(diffuseNr++);
            else if (name == "normalMap")
                number = std::to_string(normalNr++); 
            else if (name == "roughnessMap")
                number = std::to_string(specularNr++); 
            else if (name == "metalnessMap")
                 number = std::to_string(metalnessNr++); 

            glUniform1i(glGetUniformLocation(shader.ID, (name).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }


        //Mapが無い場合はマテリアルの値をそのまま使うように
        if (diffuseNr == 1) {
            glUniform1i(glGetUniformLocation(shader.ID, "albedoFlag"), GL_FALSE);
        }
        else {
            glUniform1i(glGetUniformLocation(shader.ID, "albedoFlag"), GL_TRUE);
        }

        if (normalNr == 1) {
            glUniform1i(glGetUniformLocation(shader.ID, "normalFlag"), GL_FALSE);
        }
        else {
            glUniform1i(glGetUniformLocation(shader.ID, "normalFlag"), GL_TRUE);
        }

        if (specularNr == 1) {
            glUniform1i(glGetUniformLocation(shader.ID, "roughFlag"), GL_FALSE);
        }
        else {
            glUniform1i(glGetUniformLocation(shader.ID, "roughFlag"), GL_TRUE);
        }

        if (metalnessNr == 1) {
            glUniform1i(glGetUniformLocation(shader.ID, "metalFlag"), GL_FALSE);
        }
        else {
            glUniform1i(glGetUniformLocation(shader.ID, "metalFlag"), GL_TRUE);
        }
        

        // メッシュ描画
        glBindVertexArray(VAO);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBlockIndex, 0, sizeof(Material));
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // テクスチャユニットをデフォルトに戻す
        glActiveTexture(GL_TEXTURE0);
    }

private:

    unsigned int VBO, EBO, defaultMetalnessMap;

    /**
     * @brief メッシュデータのバッファーを初期化
     */
    void setupMesh()
    {
        // バッファー生成
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &uniformBlockIndex);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) + sizeof(mats)  , &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockIndex);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(mats), (&mats), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);



        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // 法線
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // uv座標
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
#endif