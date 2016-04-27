#ifndef __Texture__
#define __Texture__

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>

class Texture
{
public:
	Texture();
	virtual ~Texture();
	void setFilename(const std::string &f) { filename = f; }
	void init();
	void bind(GLint handle, GLint unit);
	void unbind(GLint unit);
	
private:
	std::string filename;
	int width;
	int height;
	GLuint tid;
	
};

#endif
