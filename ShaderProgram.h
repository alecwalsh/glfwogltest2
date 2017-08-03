#pragma once

#include <GL/glew.h>
#include <tuple>

class ShaderProgram
{
    GLuint ShaderProgramFromFiles(const char* vertShaderFile, const char* fragShaderFile);
    void getCompileErrors(GLuint shader);
    void getLinkErrors(GLuint shaderProgram);
    std::tuple<int, int, bool> version;
public:
    GLuint shaderProgram;
    ShaderProgram(char const* vertShader, char const* fragShader, std::tuple<int, int, bool> version);
    ~ShaderProgram();
};

