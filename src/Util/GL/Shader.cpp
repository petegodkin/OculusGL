#include "Shader.h"

#include "GLSL.h"

using namespace std;
using namespace GLSL;

Shader::Shader()
{
}

Shader::Shader(string vertShader, string fragShader)
{
	GLint rc;

	// Create shader handles
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shader sources
	string path = "../shaders/";
	string vshader_name = path + vertShader;
	string fshader_name = path + fragShader;
	const char *vshader = textFileRead(vshader_name.c_str());
	const char *fshader = textFileRead(fshader_name.c_str());
	glShaderSource(vertexShader, 1, &vshader, NULL);
	glShaderSource(fragmentShader, 1, &fshader, NULL);

	// Compile vertex shader
	glCompileShader(vertexShader);
	printError(vshader_name);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &rc);
	printShaderInfoLog(vertexShader);
	if (!rc)
	{
		printf("Error compiling vertex shader %s\n", vertShader.c_str());
	}

	// Compile fragment shader
	glCompileShader(fragmentShader);
	printError(fshader_name);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &rc);
	printShaderInfoLog(fragmentShader);
	if (!rc)
	{
		printf("Error compiling fragment shader %s\n", fragShader.c_str());
	}

	// Create the program and link
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	printError("both?");
	glGetProgramiv(program, GL_LINK_STATUS, &rc);
	printProgramInfoLog(program);
	if (!rc)
	{
		printf("Error linking shaders %s and %s\n", vertShader.c_str(), fragShader.c_str());
	}

	setupHandles();

	assert(printError("both? again") == GL_NO_ERROR);
}

Shader::~Shader() {

}

void Shader::setupHandles()
{
	int total = -1;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &total);
	for (int i = 0; i<total; ++i)
	{
		int name_len = -1, num = -1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveUniform(program, GLuint(i), sizeof(name) - 1,
			&name_len, &num, &type, name);
		name[name_len] = 0;
		GLuint location = glGetUniformLocation(program, name);
		uniforms.insert(pair<string, GLint>(name, location));
	}

	total = -1;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &total);
	for (int i = 0; i<total; ++i)
	{
		int name_len = -1, num = -1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveAttrib(program, GLuint(i), sizeof(name) - 1,
			&name_len, &num, &type, name);
		name[name_len] = 0;
		GLuint location = glGetAttribLocation(program, name);
		attributes.insert(pair<string, GLint>(name, location));
	}

	total = -1;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &total);
	for (int i = 0; i<total; ++i)
	{
		int name_len;
		glGetActiveUniformBlockiv(program, GLuint(i), GL_UNIFORM_BLOCK_NAME_LENGTH, &name_len);
		std::vector<GLchar> name;
		glGetActiveUniformBlockName(program, GLuint(i), name_len, NULL, &name[0]);

		GLuint location = glGetUniformBlockIndex(program, &name[0]);
		uniformBlocks.insert(pair<string, GLint>(&name[0], location));
	}
}

GLint Shader::GetAttrLoc(string name)
{
	GLint handle;
	try
	{
		handle = attributes.at(name);
	}
	catch (exception& e)
	{
		cout << "The attribute name '" << name << "' is not bound to this shader." << endl;
		assert(false);
	}
	return handle;
}

GLint Shader::GetUniLoc(string name)
{
	GLint handle;
	try
	{
		handle = uniforms.at(name);
	}
	catch (exception& e)
	{
		cout << "The uniform name '" << name << "' is not bound to this shader." << endl;
		assert(false);
	}
	return handle;
}

GLint Shader::getUniformBlockHandle(string name)
{
	GLint handle;
	try
	{
		handle = uniformBlocks.at(name);
	}
	catch (exception& e)
	{
		cout << "The uniform block name '" << name << "' is not bound to this shader." << endl;
		assert(false);
	}
	return handle;
}

GLuint Shader::prog() const
{
	return program;
}


bool Shader::check_gl_error(std::string msg) const {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << msg << ": OpenGL Error: " << error << " English: " << glewGetErrorString(error)
			<< " Or: " << gluErrorString(error) << std::endl;

		system("PAUSE");
		return true;
	}

	return false;
}