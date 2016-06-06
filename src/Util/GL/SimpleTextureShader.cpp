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
	//int texture = getAttributeHandle("aTextCoord");

	std::vector<Mesh *> meshes = entity->shape()->getMeshes();

	glUniformMatrix4fv(GetUniLoc("uViewMatrix"), 1, GL_FALSE, value_ptr(camera->view()));
	glUniformMatrix4fv(GetUniLoc("uModelMatrix"), 1, GL_FALSE, value_ptr(entity->modelMat()));
	glUniformMatrix4fv(GetUniLoc("uProjMatrix"), 1, GL_FALSE, value_ptr(camera->proj()));

	for (int i = 0; i < meshes.size(); i++)
	{
		Mesh* mesh = meshes.at(i);
		glBindVertexArray(mesh->VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textures.at(0).id);
		glUniform1i(GetUniLoc("Utex"), 0);
		//glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO3);
		//glEnableVertexAttribArray(texture);//
		//glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE,//
		//	sizeof(vec2), 0);//

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IND);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

		//glDisableVertexAttribArray(texture);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


}