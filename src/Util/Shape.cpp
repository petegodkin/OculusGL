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
	texBufID(0),
	VAO(0),
	m_nDefaultScale(1.0f),
	m_diffuse(1.0f, 0.0f, 0.0f)
{
}

Shape::Shape(float scale) :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0),
	VAO(0),
	m_diffuse(1.0f, 0.0f, 0.0f)
{
	m_nDefaultScale = scale;
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

void Shape::init(bool textured)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	if (norBuf.size() > 0) {
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	if (textured && texBuf.size() > 0)
	{
		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float), &texBuf[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	glGenBuffers(1, &eleBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Shape::draw() const
{
	glDrawElements(GL_TRIANGLES,
		eleBuf.size(),
		GL_UNSIGNED_INT,
		0);
}

float Shape::getScale() const
{
	return m_nDefaultScale;
}

Texture* Shape::texture() const{
	return _texture;
}

void Shape::setTexture(Texture* texture) {
	_texture = texture;
}

void Shape::setDiffuse(glm::vec3 diffuse)
{
	m_diffuse = diffuse;
}

glm::vec3 Shape::getDiffuse() const
{
	return m_diffuse;
}
