#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>

#include "ShaderWithVariables.h"

class Shape
{
public:
	Shape();
	Shape(float scale);
	virtual ~Shape();
	void loadMesh(const std::string &meshName);
	void init(bool textured);
	void draw() const;
	virtual void bindVAO() const { glBindVertexArray(VAO); }
	virtual void unbindVAO() const { glBindVertexArray(0); }

	float getScale() const;
	
	std::vector<unsigned int> eleBuf;
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;
	unsigned eleBufID;
	unsigned posBufID;
	unsigned norBufID;
	unsigned texBufID;

	unsigned VAO;

private:
	float m_nDefaultScale;
};

#endif
