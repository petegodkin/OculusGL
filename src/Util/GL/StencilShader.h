#pragma once

#include "Shader.h"
#include "GBuffer.h"
#include "Entity.h"
#include "Light.h"
#include "Camera.h"

class StencilShader : public Shader
{
public:
	StencilShader();
	StencilShader(std::string vertShader, std::string fragShader);
	void stencilPass(Camera* camera, const GBuffer* gbuffer, Light* light) const;
private:
	GLuint model_handle;
	GLuint view_handle;
	GLuint proj_handle;
};