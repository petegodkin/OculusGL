#include "DeferredShader.h"
#include <GL/glew.h>

using namespace glm;
using namespace std;

DeferredShader::DeferredShader() {

}

DeferredShader::DeferredShader(std::string vertShader, std::string fragShader, int width, int height)
	: Shader(vertShader, fragShader), skyShader("simple.vert", "simple.frag"),
	renderer("light.vert", "light.frag"), disp_mode(deferred)
{
	if (DEBUG_MODE)
		check_gl_error("Before all dem inits");

	gbuffer = new GBuffer();
	gbuffer->Init(width, height);

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

	std::map<const MeshSet*, std::vector<Entity*>> mapMesh;

	for (int i = 0; i < ents.size(); i++)
	{
		//auto meshItr = mapMesh.find(ents[i]->shape());
		//Do we have to initialize the vector first?
		mapMesh[ents[i]->shape()].push_back(ents[i]);
	}

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

	for (auto meshset : mapMesh) 
	{
		for (Mesh* mesh : meshset.first->getMeshes())
		{
			mesh->bindVAO();

			if (mesh->textures.size() > 0) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mesh->textures.at(0).id);
				glUniform1i(UtexHandle, 0);
				glUniform1i(UflagHandle, 1);
			}
			else {
				glUniform1i(UflagHandle, 0);
			}

			glUniform3fv(UdColorHandle, 1, value_ptr(mesh->diffuse));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IND);
			check_gl_error("Here1111111111112");

			for (auto entity : meshset.second)
			{
				glUniformMatrix4fv(uModelMatrixHandle, 1, GL_FALSE, value_ptr(entity->modelMat()));
				mesh->draw();
			}

			if (mesh->textures.size() > 0) {
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}
	glDepthMask(GL_FALSE);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint DeferredShader::fboID() {
	return gbuffer->fboID();
}

GLuint DeferredShader::finalTexture() {
	return gbuffer->finalTexture();
}

int DeferredShader::width() {
	return gbuffer->width();
}
int DeferredShader::height() {
	return gbuffer->height();
}


void DeferredShader::draw(Camera* camera, std::vector<Entity*> ents, std::vector<Light*> lights) const
{
	
	gbuffer->StartFrame();
	glUseProgram(prog());
	geomPass(camera, ents);
	glUseProgram(0);

	if (disp_mode == four_screen) {
		lightPass();
	} else {
		renderer.draw(camera, lights);
		skyboxPass(camera);
		finalPass();
	}
	
}

void DeferredShader::skyboxPass(Camera* camera) const
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
