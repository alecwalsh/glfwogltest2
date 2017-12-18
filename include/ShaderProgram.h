#pragma once
#include "glad/glad.h"

#include <memory>
#include <tuple>
#include <iostream>

class ShaderProgram {
    GLuint ShaderProgramFromFiles(const char *vertShaderFile, const char *fragShaderFile);
    void getCompileErrors(GLuint shader);
    void getLinkErrors(GLuint shaderProgram);
    std::tuple<int, int, bool> version;
  public:
    GLuint shaderProgram;
    ShaderProgram(const char *vertShader, const char *fragShader, std::tuple<int, int, bool> version);
    ShaderProgram& operator=(ShaderProgram&& sp);
    ShaderProgram& operator=(ShaderProgram& sp);
    ~ShaderProgram();
};
