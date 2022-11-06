#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


#include <stb_image.h>

using namespace std;

class Model
{
public:
    // モデルのデータ
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    //コンストラタ
    Model(string const& path, const glm::vec3 position, const glm::vec3 rotation,
        const glm::vec3 scale, bool gamma = false) : position_(position),
        rotation_(rotation),
        scale_(scale),
        model_matrix_(),
        gammaCorrection(gamma)
    {
        loadModel(path);
        RecaluculateModelMatrix();
    }

    /**
     * @brief　自分のメッシュを描画する巻数
     * @param shader シェーダー
     */
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    /**
     * @brief　モデルの位置を取得
     * @return モデルのワールド座標
     */
    const glm::vec3 GetPosition() const;
    /**
     * @brief モデルの位置を設定
     * @param position　新しい位置
     */
    void SetPosition(const glm::vec3 position);

    /**
     * @brief　モデルの回転を取得
     * @return モデル回転
     */
    const glm::vec3 GetRotation() const;
    /**
     * @brief モデルの回転を設定
     * @param rotation　新しい回転
     */
    void SetRotation(const glm::vec3 rotation);
    /**
     * @brief　モデルのスケールを取得
     * @return モデルのスケール
     */
    const glm::vec3 GetScale() const;
    /**
     * @brief　モデルのスケールを取得
     * @param scale　新しいスケール
     */
    void SetScale(const glm::vec3 scale);

    /**
     * @brief　モデル行列を取得
     * @return モデル行列
     */
    const glm::mat4 GetModelMatrix() const;

private:
    glm::vec3 position_;
    glm::vec3 rotation_;
    glm::vec3 scale_;
    glm::mat4 model_matrix_;
    void RecaluculateModelMatrix();

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
     /**
     * @brief　assimpによってモデルをロードする
     * @param path パス
     */
    void loadModel(string const& path);

      /**
      * @brief　再帰的にノードを処理。そのノードに位置する個々のメッシュを処理し、その子ノードがある場合はその子ノードに対してこの処理を繰り返す。
      */
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    /**
    * @brief　meshに対応するマテリアルをロードする
    */
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
};



#endif