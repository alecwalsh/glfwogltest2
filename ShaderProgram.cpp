#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram(char const* vertShader, char const* fragShader) {
	ShaderProgramFromFiles(vertShader, fragShader);
}

//TODO: add error handling
GLuint ShaderProgram::ShaderProgramFromFiles(char const* vertShaderFile, char const* fragShaderFile) {
	std::ifstream vert_if;
	vert_if.open(vertShaderFile);
	std::ifstream frag_if;
	frag_if.open(fragShaderFile);

	std::stringstream vert_buffer;
	std::stringstream frag_buffer;
	vert_buffer << vert_if.rdbuf();
	frag_buffer << frag_if.rdbuf();
	
	auto v = vert_buffer.str();
	auto f = frag_buffer.str();
	auto vertexSource = v.c_str();
	auto fragmentSource = f.c_str();


	GLuint shaderProgram = glCreateProgram();
	//Bind attrib locations
	glBindAttribLocation(shaderProgram, 0, "position");
	glBindAttribLocation(shaderProgram, 1, "normal");
	glBindAttribLocation(shaderProgram, 2, "texcoord");
	glBindAttribLocation(shaderProgram, 3, "color");

	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

    //Don't need these anymore
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    
    this->shaderProgram = shaderProgram;

	return shaderProgram;
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(shaderProgram);
}
