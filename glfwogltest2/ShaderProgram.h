#pragma once

#include <gl\glew.h>

class ShaderProgram
{
	GLuint fragShader;
	GLuint vertShader;

	GLuint ShaderProgramFromFiles(char* vertShaderFile, char* fragShaderFile);
public:
	GLuint shaderProgram;
	ShaderProgram(char* vertShader, char* fragShader);
	~ShaderProgram();
};

