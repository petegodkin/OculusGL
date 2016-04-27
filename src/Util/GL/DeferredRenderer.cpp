#include "DeferredRenderer.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

DeferredRenderer::DeferredRenderer() {

}

DeferredRenderer::DeferredRenderer(std::string vertShader, std::string fragShader)
	: Shader(vertShader, fragShader), gbuffer(gbuffer), stencilShader("stencil.vert", "stencil.frag")
{
	glBindAttribLocation(prog(), 0, "aPosition");

	model_handle = GetUniLoc("uModelMatrix");
	view_handle = GetUniLoc("uViewMatrix");
	proj_handle = GetUniLoc("uProjMatrix");
	pos_map_handle = GetUniLoc("posMap");
	color_map_handle = GetUniLoc("colMap");
	normal_map_handle = GetUniLoc("norMap");
	eye_handle = GetUniLoc("uEye");
	size_handle = GetUniLoc("uSize");
	position_handle = GetAttrLoc("aPosition");
	light_pos_handle = GetUniLoc("uLightPos");
	light_color_handle = GetUniLoc("uLightColor");
}

DeferredRenderer::~DeferredRenderer()
{

}

void DeferredRenderer::setGBuffer(GBuffer* gbuffer) {
	this->gbuffer = gbuffer;
}

void DeferredRenderer::pointLightPass(Camera* camera, Light* light)
{
	gbuffer->BindForLightPass();

	glUseProgram(prog());

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	light->shape->bindVAO();

	glUniform1i(pos_map_handle, GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
	glUniform1i(color_map_handle, GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glUniform1i(normal_map_handle, GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);

	vec3 eye = camera->position();
	vec3 light_pos = light->pos;
	vec3 light_color = light->color;
	glUniform3f(eye_handle, eye.x, eye.y, eye.z);
	glUniform3f(light_pos_handle, light_pos.x, light_pos.y, light_pos.z);
	glUniform3f(light_color_handle, light_color.x, light_color.y, light_color.z);
	glUniform2f(size_handle, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);

	glUniformMatrix4fv(model_handle, 1, GL_FALSE, value_ptr(light->transform()));
	glUniformMatrix4fv(view_handle, 1, GL_FALSE, glm::value_ptr(camera->view()));
	glUniformMatrix4fv(proj_handle, 1, GL_FALSE, glm::value_ptr(camera->proj()));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light->shape->eleBufID);

	if (DEBUG_MODE)
		check_gl_error("rend before");

	light->shape->draw();

	if (DEBUG_MODE)
		check_gl_error("rend after");


	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}


void DeferredRenderer::draw(Camera* camera, std::vector<Light*> lights)
{
	glEnable(GL_STENCIL_TEST);
	for (int i = 0; i < lights.size(); i++) {
		stencilShader.stencilPass(camera, gbuffer, lights[i]);
		pointLightPass(camera, lights[i]);
	}
	glDisable(GL_STENCIL_TEST);
}