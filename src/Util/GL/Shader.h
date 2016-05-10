#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include "Entity.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include "Light.h"

class Shader
{
	GLuint program;
	GLuint vertexShader;
	GLuint fragmentShader;
	std::map<std::string, GLint> attributes;
	std::map<std::string, GLint> uniforms;
	std::map<std::string, GLint> uniformBlocks;
public:
	Shader();
	Shader(std::string vertShader, std::string fragShader);
	virtual ~Shader(); // important
	void setupHandles();
	GLint GetAttrLoc(std::string name);
	GLint GetUniLoc(std::string name);
	GLint getUniformBlockHandle(std::string name);
	GLuint prog() const;


	bool check_gl_error(std::string msg) const;
};