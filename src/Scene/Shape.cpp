#include "Shape.h"
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

Shape::Shape() :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0)
{
}

Shape::~Shape()
{
}

void Shape::loadMesh(const string &meshName)
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		posBuf = shapes[0].mesh.positions;
		norBuf = shapes[0].mesh.normals;
		texBuf = shapes[0].mesh.texcoords;
		eleBuf = shapes[0].mesh.indices;
	}
}

void Shape::init(ShaderWithVariables &shader)
{
	GLuint vertVbo = 0;
	glGenBuffers(1, &vertVbo);
	shader.AddVbo("vPosition", vertVbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertVbo);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	glVertexAttribPointer(shader.GetAttrLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint colVbo = 0;
	glGenBuffers(1, &colVbo);
	shader.AddVbo("vColor", colVbo);
	glBindBuffer(GL_ARRAY_BUFFER, colVbo);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	glVertexAttribPointer(shader.GetAttrLoc("vColor"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(shader.GetAttrLoc("vPosition"));
	glEnableVertexAttribArray(shader.GetAttrLoc("vColor"));

	GLuint quadVbo = 0;
	glGenBuffers(1, &quadVbo);
	shader.AddVbo("elements", quadVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);
}

void Shape::draw(const ShaderWithVariables &shader) const
{
	shader.bindVAO();
	glDrawElements(GL_TRIANGLES,
		eleBuf.size(),
		GL_UNSIGNED_INT,
		0);
	glBindVertexArray(0);
}
