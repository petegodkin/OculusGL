#include "StencilShader.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

StencilShader::StencilShader() {

}

StencilShader::StencilShader(std::string vertShader, std::string fragShader)
	: Shader(vertShader, fragShader)
{
	glBindAttribLocation(prog(), 0, "aPosition");
	model_handle = GetUniLoc("uModelMatrix");
	view_handle = GetUniLoc("uViewMatrix");
	proj_handle = GetUniLoc("uProjMatrix");
}

void StencilShader::stencilPass(Camera* camera, const GBuffer* gbuffer, Light* light) const
{
	glUseProgram(prog());

	// Disable color/depth write and enable stencil
	gbuffer->BindForStencilPass();

	assert(light->shape->getMeshes().size() == 1);
	Mesh* mesh = light->shape->getMeshes()[0];
	mesh->bindVAO();

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	glClear(GL_STENCIL_BUFFER_BIT);

	// We need the stencil test to be enabled but we want it
	// to succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	glUniformMatrix4fv(model_handle, 1, GL_FALSE, glm::value_ptr(light->transform()));
	glUniformMatrix4fv(view_handle, 1, GL_FALSE, glm::value_ptr(camera->view()));
	glUniformMatrix4fv(proj_handle, 1, GL_FALSE, glm::value_ptr(camera->proj()));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IND);

	if (DEBUG_MODE)
		check_gl_error("stencil before");

	mesh->draw();

	if (DEBUG_MODE)
		check_gl_error("stencil after");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}