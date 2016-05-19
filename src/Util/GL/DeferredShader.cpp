#include "DeferredShader.h"
#include <GL/glew.h>

using namespace glm;
using namespace std;

DeferredShader::DeferredShader() {

}

DeferredShader::DeferredShader(std::string vertShader, std::string fragShader)
	: Shader(vertShader, fragShader), skyShader("simple.vert", "simple.frag"),
	renderer("light.vert", "light.frag"), disp_mode(deferred)
{
	if (DEBUG_MODE)
		check_gl_error("Before all dem inits");

	gbuffer = new GBuffer();
	gbuffer->Init(SCREEN_WIDTH, SCREEN_HEIGHT);

	renderer.setGBuffer(gbuffer);
	
	glBindAttribLocation(prog(), 0, "aPosition");
	glBindAttribLocation(prog(), 1, "aNormal");
	glBindAttribLocation(prog(), 2, "aTexCoord");

	uViewMatrixHandle = GetUniLoc("uViewMatrix");
	uModelMatrixHandle = GetUniLoc("uModelMatrix");
	uProjMatrixHandle = GetUniLoc("uProjMatrix");
	UtexHandle = GetUniLoc("Utex");
	UflagHandle = GetUniLoc("Uflag");
	UdColorHandle = GetUniLoc("UdColor");
	if (DEBUG_MODE)
		check_gl_error("After init prog def shader");

	_skybox = nullptr;

	test = "NEO KROSHNIKAV";
}

void DeferredShader::setSkybox(Entity* skybox) {
	_skybox = skybox;
}

void DeferredShader::geomPass(Camera* camera, std::vector<Entity*> ents) const
{
	//std::cout << test << std::endl;

	if (DEBUG_MODE)
		check_gl_error("Before geom pass");

	gbuffer->BindForGeomPass();

	glDepthMask(GL_TRUE);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);//fixes warnings from next line
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUniformMatrix4fv(uViewMatrixHandle, 1, GL_FALSE, value_ptr(camera->view()));
	glUniformMatrix4fv(uProjMatrixHandle, 1, GL_FALSE, value_ptr(camera->proj()));

	check_gl_error("dfgdhdfghdfgh12");

	for (Entity* entity : ents) {
		const Shape* shape = entity->shape();
		shape->bindVAO();

		if (entity->texture() != nullptr && shape->texBuf.size() > 0) {
			entity->texture()->bind(UtexHandle, 0);
			glUniform1i(UflagHandle, 1);
		} else {
			glUniform1i(UflagHandle, 0);
		}

		//TODO: fix hardcoded diffuse color
		glUniform3fv(UdColorHandle, 1, value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape->eleBufID);
		check_gl_error("Here1111111111112");
		
		glUniformMatrix4fv(uModelMatrixHandle, 1, GL_FALSE, value_ptr(entity->modelMat()));

		if (DEBUG_MODE)
			check_gl_error("Def shader before draw");

		shape->draw();

		if (DEBUG_MODE)
			check_gl_error("Def shader after draw");


		if (entity->texture() != nullptr && shape->texBuf.size() > 0) {
			//glBindTexture(GL_TEXTURE_2D, 0);
			entity->texture()->unbind(0);
		}
	}
	glDepthMask(GL_FALSE);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DeferredShader::draw(Camera* camera, std::vector<Entity*> ents, std::vector<Light*> lights) const
{
	
	gbuffer->StartFrame();
	glUseProgram(prog());
	geomPass(camera, ents);
	glUseProgram(0);

	if (disp_mode == four_screen) {
		//glUseProgram(prog());
		//startLightPasses();
		lightPass();
		//glUseProgram(0);
	} else {
		renderer.draw(camera, lights);

		//glEnable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);

		//glUseProgram(prog());
		finalPass();
		//glUseProgram(0);
	}

	//skyboxPass(camera);
	
}

void DeferredShader::skyboxPass(Camera* camera)
{
	glUseProgram(skyShader.prog());

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (_skybox != nullptr)
		skyShader.draw(camera, _skybox);
}

void DeferredShader::finalPass() const
{
	//glUseProgram(prog());
	gbuffer->BindForFinalPass();
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void DeferredShader::startLightPasses() const
{
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	gbuffer->BindAllForReading();
	glClear(GL_COLOR_BUFFER_BIT);
}

void DeferredShader::lightPass() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);//fixes warnings from next line
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gbuffer->BindForReading();

	GLsizei HalfWidth = (GLsizei)(SCREEN_WIDTH / 2.0f);
	GLsizei HalfHeight = (GLsizei)(SCREEN_HEIGHT / 2.0f);

	gbuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	gbuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		0, HalfHeight, HalfWidth, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	gbuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		HalfWidth, HalfHeight, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	gbuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		HalfWidth, 0, SCREEN_WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

DeferredShader::~DeferredShader() {
	delete gbuffer;
}
