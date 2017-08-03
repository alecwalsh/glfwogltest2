#pragma once

#include <GL/glew.h>

class ShaderProgram
{
	GLuint ShaderProgramFromFiles(const char* vertShaderFile, const char* fragShaderFile);
    void getCompileErrors(GLuint shader);
    void getLinkErrors(GLuint shaderProgram);
public:
	GLuint shaderProgram;
	ShaderProgram(char const* vertShader, char const* fragShader);
	~ShaderProgram();
	
};

