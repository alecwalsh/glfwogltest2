#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram(char const* vertShader, char const* fragShader) {
	ShaderProgramFromFiles(vertShader, fragShader);
}

//TODO: add error handling
GLuint ShaderProgram::ShaderProgramFromFiles(const char* vertShaderFile, const char* fragShaderFile) {
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
    getCompileErrors(vertexShader);

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
    getCompileErrors(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
    getLinkErrors(shaderProgram);

    //Don't need these anymore
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    
    this->shaderProgram = shaderProgram;

	return shaderProgram;
}

void ShaderProgram::getCompileErrors(GLuint shader)
{
    GLint compiled;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(compiled == GL_FALSE)
    {
        int logLength;
        
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, infoLog);
        
        printf("Compile error: %s\n", infoLog);
        delete[] infoLog;
        exit(EXIT_FAILURE);
    }
}

void ShaderProgram::getLinkErrors(GLuint shaderProgram)
{
    GLint linked;

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if(linked == GL_FALSE)
    {
        int logLength;
        
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetProgramInfoLog(shaderProgram, logLength, &logLength, infoLog);
        
        printf("Link error: %s\n", infoLog);
        delete[] infoLog;
        exit(EXIT_FAILURE);
    }
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(shaderProgram);
}
