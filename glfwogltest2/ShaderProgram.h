#pragma once

#include <GL/glew.h>

class ShaderProgram
{
	GLuint fragShader;
	GLuint vertShader;

	GLuint ShaderProgramFromFiles(char const* vertShaderFile, char const* fragShaderFile);
public:
	GLuint shaderProgram;
	ShaderProgram(char const* vertShader, char const* fragShader);
	~ShaderProgram();
};

