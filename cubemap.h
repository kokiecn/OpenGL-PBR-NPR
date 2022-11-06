#pragma once

//#include "texture.h"
#include "shader.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>


#include <stb_image.h>


class CubeMap {
public:
	CubeMap(const std::vector<std::string> face_path, float scale,
		const char* vert, const char* frag);
	~CubeMap();
	// ÉRÉsÅ[ã÷é~
	CubeMap(const CubeMap&) = delete;
	CubeMap& operator=(const CubeMap&) = delete;
	CubeMap(CubeMap&& other);
	CubeMap& operator=(CubeMap&& other);
	void Draw(glm::mat4 viewprojection);

private:
	float scale_;
	GLuint texture_id_;
	GLuint vertices_vbo_;
	GLuint vao_;
	Shader shader_;
	void Release();
};

