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
	//�J�����̎p����ێ�����p�����[�^�[
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool firstClick = true;

	int width;
	int height;

	//���͂Ɋւ���p�����[�^�[
	float speed = 0.01f;
	float sensitivity = 100.0f;

	//�R���X�g���N�^
	Camera(int width, int height, glm::vec3 position);

	/**
	 * @brief �V�F�[�_�[��n���āA���݂̃J������ViewProjection���X�V����
	 */
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	
	/**
	 * @brief ���͎�t
	 */
	void Inputs(GLFWwindow* window);

	/**
	 * @brief �J������Projection�s��̃p�����[�^��ݒ肷��
	 * @param fovy fovy�̒l�iradians�j
	 * @param aspect aspect�̒l
	 * @param nearPlane near�̒l
	 * @param farPlane far�̒l
	 */
	glm::mat4 GetViewProjection(float FOVdeg, float nearPlane, float farPlane);
	/**
	 * @brief �J�����̈ʒu���擾
	 * @return �J�����̈ʒu
	 */
	glm::vec3 GetPosition();
};
#endif