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
    // ���f���̃f�[�^
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    //�R���X�g���^
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
     * @brief�@�����̃��b�V����`�悷�銪��
     * @param shader �V�F�[�_�[
     */
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    /**
     * @brief�@���f���̈ʒu���擾
     * @return ���f���̃��[���h���W
     */
    const glm::vec3 GetPosition() const;
    /**
     * @brief ���f���̈ʒu��ݒ�
     * @param position�@�V�����ʒu
     */
    void SetPosition(const glm::vec3 position);

    /**
     * @brief�@���f���̉�]���擾
     * @return ���f����]
     */
    const glm::vec3 GetRotation() const;
    /**
     * @brief ���f���̉�]��ݒ�
     * @param rotation�@�V������]
     */
    void SetRotation(const glm::vec3 rotation);
    /**
     * @brief�@���f���̃X�P�[�����擾
     * @return ���f���̃X�P�[��
     */
    const glm::vec3 GetScale() const;
    /**
     * @brief�@���f���̃X�P�[�����擾
     * @param scale�@�V�����X�P�[��
     */
    void SetScale(const glm::vec3 scale);

    /**
     * @brief�@���f���s����擾
     * @return ���f���s��
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
     * @brief�@assimp�ɂ���ă��f�������[�h����
     * @param path �p�X
     */
    void loadModel(string const& path);

      /**
      * @brief�@�ċA�I�Ƀm�[�h�������B���̃m�[�h�Ɉʒu����X�̃��b�V�����������A���̎q�m�[�h������ꍇ�͂��̎q�m�[�h�ɑ΂��Ă��̏������J��Ԃ��B
      */
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    /**
    * @brief�@mesh�ɑΉ�����}�e���A�������[�h����
    */
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
};



#endif