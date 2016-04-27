#pragma once

#include "Shader.h"
#include "Entity.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

class SimpleTextureShader : public Shader
{
public:
	SimpleTextureShader();
	SimpleTextureShader(std::string vertShader, std::string fragShader);
	void draw(Camera* camera, Entity* entity);
};