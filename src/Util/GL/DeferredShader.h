#pragma once

#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include "Entity.h"
#include "GBuffer.h"
#include "Light.h"
#include "DeferredRenderer.h"
#include "SimpleTextureShader.h"
#include "Camera.h"


enum DisplayMode
{
	deferred,
	four_screen
};

class DeferredShader : public Shader
{
public:

	DeferredShader();
	DeferredShader(std::string vertShader, std::string fragShader, int width, int height);
	~DeferredShader();
	virtual void draw(Camera* camera, std::vector<Entity*> ents, std::vector<Light*> lights) const;
	void setSkybox(Entity* skybox);

	GLuint fboID();
	GLuint finalTexture();
	int width();
	int height();

private:
	void geomPass(Camera* camera, std::vector<Entity*> ents) const;
	void lightPass() const;
	GBuffer *gbuffer;
	void startLightPasses() const;
	void finalPass() const;
	void skyboxPass(Camera* camera);
	SimpleTextureShader skyShader;

	Entity* _skybox;

	DeferredRenderer renderer;
	DisplayMode disp_mode;

	GLint uViewMatrixHandle;
	GLint uModelMatrixHandle;
	GLint uProjMatrixHandle;
	GLint UtexHandle;
	GLint UflagHandle;
	GLint UdColorHandle;

	std::string test;
};