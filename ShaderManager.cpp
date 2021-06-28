#include "ShaderManager.hpp"

#include "glad/glad.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

ShaderProgram::ShaderProgram(const ShaderIdentifier& id)
    : version{id.version}, shaderProgram{ShaderProgramFromFiles(id.vertShader, id.fragShader)} {}

GLuint ShaderProgram::ShaderProgramFromFiles(const std::string& vertShaderFile, const std::string& fragShaderFile) {
    using std::to_string;

    std::string versionString = "#version ";
    versionString += to_string(version.major) + to_string(version.minor) + '0';
    versionString += version.is_gles ? " es" : " core";
    versionString += '\n';

    auto getSource = [versionString](auto shaderFileName) {
        std::ifstream shaderFile;
        shaderFile.open(shaderFileName);

        std::stringstream buffer;
        buffer << versionString << shaderFile.rdbuf();

        return buffer.str();
    };

    auto v = getSource(vertShaderFile);
    auto f = getSource(fragShaderFile);
    auto vertexSource = v.c_str();
    auto fragmentSource = f.c_str();

    GLuint shaderProgram = glCreateProgram();

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    if (auto compileError = GetCompileErrors(vertexShader)) {
        throw ShaderCompileError{vertShaderFile, *compileError};
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    if (auto compileError = GetCompileErrors(fragmentShader)) {
        throw ShaderCompileError{fragShaderFile, *compileError};
    }

    // Link the vertex and fragment shader into a shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    if (auto linkError = GetLinkErrors(shaderProgram)) {
        throw ShaderLinkError{*linkError};
    }

    // Don't need these anymore
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);

    std::cout << "Compiled shader " << shaderProgram << std::endl;

    return shaderProgram;
}

std::optional<std::string> ShaderProgram::GetCompileErrors(GLuint shader) {
    GLint compiled;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        int logLength;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, infoLog);

        std::string errorText = infoLog;

        delete[] infoLog;

        return errorText;
    }

    return {};
}

std::optional<std::string> ShaderProgram::GetLinkErrors(GLuint shaderProgram) {
    GLint linked;

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
        int logLength;

        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetProgramInfoLog(shaderProgram, logLength, &logLength, infoLog);

        std::string errorText = infoLog;

        delete[] infoLog;
        
        return errorText;
    }

    return {};
}

ShaderProgram::~ShaderProgram() {
    std::cout << "Deleting shader program " << shaderProgram << std::endl;
    glDeleteProgram(shaderProgram);
}

ShaderProgram::ShaderProgram(ShaderProgram&& sp) noexcept {
    *this = std::move(sp);
}

// Move constructor and assignment need to set shaderProgram to 0
ShaderProgram& ShaderProgram::operator=(ShaderProgram&& sp) noexcept {
    shaderProgram = std::move(sp.shaderProgram);
    sp.shaderProgram = 0;
    version = std::move(sp.version);
    
    return *this;
}

// TODO: Don't compile shader until it is used
ShaderProgram& ShaderManager::AddShader(std::string name, const ShaderIdentifier& id) {
    auto nameIter = shaderNameMap.find(name);

    if (nameIter != shaderNameMap.end()) {
        const auto& newID = nameIter->second;

        if (newID != id) {
            throw std::runtime_error{"Attempted to reassign shader " + name};
        }

        return shaderMap.at(newID);
    }

    shaderNameMap.emplace(std::move(name), id);

    auto shaderIter = shaderMap.find(id);

    if (shaderIter == shaderMap.end()) {
        // Shader is not already in shaderMap, so add it
        const auto& newShaderIter = shaderMap.emplace(id, id).first;
        return newShaderIter->second;
    }

    return shaderIter->second;
}

ShaderManager& ShaderManager::GetInstance() {
    thread_local ShaderManager sm{};
    return sm;
}

void ShaderManager::UpdateProjectionMatrix(float width, float height) noexcept {
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), width / height, 1.0f, 100.0f);

    for (const auto& [id, sp] : shaderMap) {
        sp.UseProgram();
        GLint uniProj = glGetUniformLocation(sp.shaderProgram, "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
    }
}

ShaderProgram& ShaderManager::FromName(const std::string& name) { 
    return shaderMap.at(shaderNameMap.at(name));
}

void ShaderProgram::SetupTextures() const noexcept {
    UseProgram();
    // Set sampler uniforms
    glUniform1i(glGetUniformLocation(shaderProgram, "texDiffuseMap"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texSpecMap"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texNormalMap"), 2);
}

void ShaderProgram::UseProgram() const noexcept { glUseProgram(shaderProgram); }