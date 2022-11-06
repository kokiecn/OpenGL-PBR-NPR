#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include"shader.h"


class Camera
{
public:
	//カメラの姿勢を保持するパラメーター
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool firstClick = true;

	int width;
	int height;

	//入力に関するパラメーター
	float speed = 0.01f;
	float sensitivity = 100.0f;

	//コンストラクタ
	Camera(int width, int height, glm::vec3 position);

	/**
	 * @brief シェーダーを渡して、現在のカメラのViewProjectionを更新する
	 */
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	
	/**
	 * @brief 入力受付
	 */
	void Inputs(GLFWwindow* window);

	/**
	 * @brief カメラのProjection行列のパラメータを設定する
	 * @param fovy fovyの値（radians）
	 * @param aspect aspectの値
	 * @param nearPlane nearの値
	 * @param farPlane farの値
	 */
	glm::mat4 GetViewProjection(float FOVdeg, float nearPlane, float farPlane);
	/**
	 * @brief カメラの位置を取得
	 * @return カメラの位置
	 */
	glm::vec3 GetPosition();
};
#endif