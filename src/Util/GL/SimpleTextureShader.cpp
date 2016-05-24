#include <vector>
#include "SimpleTextureShader.h"
#include "Shape.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

SimpleTextureShader::SimpleTextureShader() {

}

SimpleTextureShader::SimpleTextureShader(std::string vertShader, std::string fragShader) : Shader(vertShader, fragShader)
{
}

void SimpleTextureShader::draw(Camera* camera, Entity* entity)
{
	const Shape* shape = entity->shape();

	glUseProgram(prog());
	{
		glUniformMatrix4fv(GetUniLoc("uViewMatrix"), 1, GL_FALSE, value_ptr(camera->view()));
		glUniformMatrix4fv(GetUniLoc("uModelMatrix"), 1, GL_FALSE, value_ptr(entity->modelMat()));
		glUniformMatrix4fv(GetUniLoc("uProjMatrix"), 1, GL_FALSE, value_ptr(camera->proj()));

		shape->bindVAO();
		shape->texture()->bind(GetUniLoc("Utex"), 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape->eleBufID);

		shape->draw();
		//glBindTexture(GL_TEXTURE_2D, 0);
		shape->texture()->unbind(0);
	}

	glUseProgram(0);

}