#include <GL/glew.h>
//#include <GL/freeglut.h>
#pragma once
#ifndef __GLSL__
#define __GLSL__

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif

#include <string>

namespace GLSL {
	int printError();
	int printError(std::string filename);
	void printProgramInfoLog(GLuint program);
	void printShaderInfoLog(GLuint shader);
	GLint getUniLoc(GLuint program, const GLchar *name);
	void checkVersion();
	int textFileWrite(const char *filename, char *s);
	char *textFileRead(const char *filename);
	GLint getAttribLocation(const GLuint program, const char varname[]);
	GLint getUniformLocation(const GLuint program, const char varname[]);
	void enableVertexAttribArray(const GLint handle);
	void disableVertexAttribArray(const GLint handle);
	void vertexAttribPointer(const GLint handle, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
}

#endif