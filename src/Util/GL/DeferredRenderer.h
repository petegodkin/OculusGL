#pragma once

#include "StencilShader.h"
#include "Shader.h"
#include "Camera.h"

class DeferredRenderer : public Shader
{
public:
	DeferredRenderer();
	DeferredRenderer(std::string vertShader, std::string fragShader);

	~DeferredRenderer();

	void setGBuffer(GBuffer* gbuffer);

	virtual void draw(Camera* camera, std::vector<Light*> lights) const;
private:
	void pointLightPass(Camera* camera, Light* light) const;

	glm::vec3 lightDir;

	const GBuffer* gbuffer;

	StencilShader stencilShader;

	GLuint model_handle;
	GLuint view_handle;
	GLuint proj_handle;

	GLuint pos_map_handle;
	GLuint color_map_handle;
	GLuint normal_map_handle;

	GLuint eye_handle;
	GLuint size_handle;
	GLuint position_handle;
	GLuint light_pos_handle;
	GLuint light_color_handle;

	GLuint shiny_handle;

	std::string toast;
};