#pragma once

#include <GL/glew.h>

class ShaderProgram
{
	GLuint ShaderProgramFromFiles(const char* vertShaderFile, const char* fragShaderFile);
public:
	GLuint shaderProgram;
	ShaderProgram(char const* vertShader, char const* fragShader);
	~ShaderProgram();
	
};

