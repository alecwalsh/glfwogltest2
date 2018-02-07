#pragma once
#include "glad/glad.h"

#include <iostream>
#include <memory>
#include <tuple>

using gl_version_t = std::tuple<int, int, bool>;

class ShaderProgram {
    GLuint ShaderProgramFromFiles(const char* vertShaderFile, const char* fragShaderFile);
    void getCompileErrors(GLuint shader);
    void getLinkErrors(GLuint shaderProgram);
    gl_version_t version;

  public:
    GLuint shaderProgram;
    ShaderProgram(const char* vertShader, const char* fragShader, std::tuple<int, int, bool> version);
    ShaderProgram& operator=(ShaderProgram&& sp);
    ShaderProgram& operator=(ShaderProgram& sp);
    ~ShaderProgram();
};
