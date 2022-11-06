#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "cubemap.h"
#include <stb_image.h>
using namespace std;



std::vector<std::string> faces
{
    "skybox/right.jpg",
    "skybox/left.jpg",
    "skybox/top.jpg",
    "skybox/bottom.jpg",
    "skybox/front.jpg",
    "skybox/back.jpg"
};

int main() {
    // GLFW エラーのコールバック
    glfwSetErrorCallback(
        [](auto id, auto description) { std::cerr << description << std::endl; });

    const GLuint width = 800;
    const GLuint height = 600;

    // GLFWの初期化
    if (!glfwInit()) {
        return false;
    }

    // OpenGL Version 4.6 Core Profileを選択する
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // リサイズ不可
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // ウィンドウの作成
    GLFWwindow* window =
        glfwCreateWindow(width, height, "Game", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Can't create GLFW window." << std::endl;
        return false;
    }
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    // GLEWの初期化
    if (glewInit() != GLEW_OK) {
        std::cerr << "Can't initialize GLEW." << std::endl;
        return false;
    }

    // VSyncを待つ
    glfwSwapInterval(1);

    // OpenGL エラーのコールバック
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](auto source, auto type, auto id, auto severity, auto length,
            const auto* message, const void* userParam) {
                auto t = type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "";
                std::cerr << "GL CALLBACK: " << t << " type = " << type
                    << ", severity = " << severity << ", message = " << message
                    << std::endl;
        },
        0);


    //shader生成
    Shader toonShader("./shaders/toon.vert", "shaders/toon.frag");
    Shader pbrShader("./shaders/PBR.vert", "shaders/PBR.frag");
    Shader shadowMapShader("./shaders/shadow.vert", "./shaders/shadow.frag");

    //光源パラメーター
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPosDirectional = glm::vec3(5.0f, 3.0f, -2.0f) * 10.0f;
    glm::vec3 lightPosPoint = glm::vec3(5.0f, 13.0f, -3.0f);

    auto camera_ = std::make_unique<Camera>(
        width, height,glm::vec3(0.0f, 0.0f, 3.0f));


    //toonShader設定
    toonShader.use();
    //shaderのuniform位置取得
    GLuint toon_model_loc_ = glGetUniformLocation(toonShader.ID, "Model");
    GLuint toon_view_projection_loc_ = glGetUniformLocation(toonShader.ID, "ViewProjection");
    GLuint toon_model_it_loc_ = glGetUniformLocation(toonShader.ID, "ModelIT");
    GLuint toon_world_camera_position_loc_ = glGetUniformLocation(toonShader.ID, "worldCameraPosition");
    //光関連のuniformセッティング
    glUniform4f(glGetUniformLocation(toonShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(toonShader.ID, "lightPos1"), lightPosDirectional.x, lightPosDirectional.y, lightPosDirectional.z);
    glUniform3f(glGetUniformLocation(toonShader.ID, "lightPosPoint"), lightPosPoint.x, lightPosPoint.y, lightPosPoint.z);
    
    
    //PBRセッティング
    pbrShader.use();
    //shaderのuniform位置取得
    GLuint pbr_model_loc_ = glGetUniformLocation(pbrShader.ID, "Model");
    GLuint pbr_view_projection_loc_ = glGetUniformLocation(pbrShader.ID, "ViewProjection");
    GLuint pbr_model_it_loc_ = glGetUniformLocation(pbrShader.ID, "ModelIT");
    GLuint pbr_world_camera_position_loc_ = glGetUniformLocation(pbrShader.ID, "worldCameraPosition");
    //光関連のuniformセッティング
    glUniform4f(glGetUniformLocation(pbrShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(pbrShader.ID, "lightPos1"), lightPosDirectional.x, lightPosDirectional.y, lightPosDirectional.z);
    glUniform3f(glGetUniformLocation(pbrShader.ID, "lightPosPoint"), lightPosPoint.x, lightPosPoint.y, lightPosPoint.z);
   
    
    //モデル　インポート
    Model toonModel("./mirai/mirai.obj", glm::vec3(9.0f,8.8f, -7.0f),
      glm::vec3(0.0f), glm::vec3(1.3f));
    Model house("./smallhouse/Smallhouse.obj", glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f), glm::vec3(2.0f));

    //CubeMap用オブジェクト
    CubeMap cubeMap(faces, 500.0, "shaders/skybox.vert", "shaders/skybox.frag");

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);


    //シャドウマップ用FBO
    unsigned int shadowMapFBO;
    glGenFramebuffers(1, &shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    //シャドウマップ用Texture
    unsigned int shadowMapWidth = 2048, shadowMapHeight = 2048;
    unsigned int shadowMap;
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //シャドウマップ用　光源への変換行列
    glm::mat4 orthgonalProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, 0.1f, 75.0f);
    glm::mat4 lightView = glm::lookAt(lightPosDirectional, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = orthgonalProjection * lightView;

    shadowMapShader.use();
    glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    // メインループ
    while (!glfwWindowShouldClose(window)) {


        // シャドウマップ用にViewPortの変更
        glViewport(0, 0, shadowMapWidth, shadowMapHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        auto house_model = house.GetModelMatrix();
        auto house_model_it = glm::inverseTranspose(house_model);
        auto model = toonModel.GetModelMatrix();
        auto model_it = glm::inverseTranspose(model);


        // シャドウマップ描画
        glCullFace(GL_BACK);
        shadowMapShader.use();
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.ID, "model"), 1, GL_FALSE, &house_model[0][0]);
        house.Draw(shadowMapShader);
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.ID, "model"), 1, GL_FALSE, &model[0][0]);
        toonModel.Draw(shadowMapShader);
        glCullFace(GL_FRONT);



        //通常の描画を開始
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //viewportをデフォルトに戻す
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        cubeMap.Draw( camera_->GetViewProjection(45.0f, 0.1f, 100.0f));

        //入力受付
        camera_->Inputs(window);

        auto camera_position = camera_->GetPosition();

        //toon描画開始
        toonShader.use();
        glUniformMatrix4fv(glGetUniformLocation(toonShader.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        camera_->Matrix(45.0f, 0.1f, 100.0f, toonShader, "ViewProjection");
        glUniform3fv(toon_world_camera_position_loc_, 1, &camera_position[0]);
        glUniformMatrix4fv(toon_model_loc_, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(toon_model_it_loc_, 1, GL_FALSE, &model_it[0][0]);
        
        
        //シャドウマップをshaderに渡す
        glActiveTexture(GL_TEXTURE0 + 6);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glUniform1i(glGetUniformLocation(toonShader.ID, "shadowMap"), 6);
        toonModel.Draw(toonShader);
        
        
        //半透明を一度有効化
        glEnable(GL_BLEND);

        //PBR描画開始
        pbrShader.use();
        glUniformMatrix4fv(glGetUniformLocation(pbrShader.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        camera_->Matrix(45.0f, 0.1f, 100.0f, pbrShader, "ViewProjection");
        glUniform3fv(pbr_world_camera_position_loc_, 1, &camera_position[0]);

        glUniformMatrix4fv(pbr_model_loc_, 1, GL_FALSE, &house_model[0][0]);
        glUniformMatrix4fv(pbr_model_it_loc_, 1, GL_FALSE, &house_model_it[0][0]);
        
        //シャドウマップをshaderに渡す
        glActiveTexture(GL_TEXTURE0 + 6);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glUniform1i(glGetUniformLocation(pbrShader.ID, "shadowMap"), 6);
        house.Draw(pbrShader);
        //半透明無効
        glDisable(GL_BLEND);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	return 0;
}
