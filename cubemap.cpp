#include "cubemap.h"


CubeMap::CubeMap(const std::vector<std::string> faces, float scale, 
    const char* vert, const char* frag): scale_(scale),shader_(vert,frag){

    float skyboxVertices[] = {         
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    for (int i = 0; i < sizeof(skyboxVertices) / sizeof(skyboxVertices[0]); i++)
        skyboxVertices[i] *= scale;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vertices_vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
        stbi_set_flip_vertically_on_load(true);

    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    shader_.use();
    shader_.setInt("skybox", 0);
}

CubeMap::~CubeMap() { 
    Release();
}

CubeMap::CubeMap(CubeMap&& other) 
    : texture_id_(other.texture_id_), scale_(other.scale_), shader_(other.shader_){
    other.texture_id_ = 0;
    other.vao_ = 0;
    other.vertices_vbo_ = 0;
    other.shader_.ID = 0;
}

CubeMap& CubeMap::operator=(CubeMap&& other) {
    if (this != &other) {
        Release();

        texture_id_ = other.texture_id_;
        vertices_vbo_ = other.vertices_vbo_;
        vao_ = other.vao_;
        shader_ = other.shader_;

        other.texture_id_ = 0;
        other.vao_ = 0;
        other.vertices_vbo_ = 0;
        other.shader_.ID = 0;

        scale_ = other.scale_;
    }

    return *this;
}

void CubeMap::Release(){
    glDeleteTextures(1, &texture_id_);
    glDeleteBuffers(1, &vertices_vbo_);
    glDeleteBuffers(1, &vao_);
    glDeleteProgram(shader_.ID);
}

void CubeMap::Draw(glm::mat4 viewProjection) {

    glDepthFunc(GL_LEQUAL); 

    shader_.use();
    shader_.setMat4("ViewProjection", viewProjection);

    glBindVertexArray(vao_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); 
}