#include "ShaderManager.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

ShaderProgram::ShaderProgram(const ShaderIdentifier& id)
    : version{id.version}, shaderProgram{ShaderProgramFromFiles(id.vertShader, id.fragShader)} {}

GLuint ShaderProgram::ShaderProgramFromFiles(const std::string& vertShaderFile, const std::string& fragShaderFile) {
    using std::to_string;

    // std::string version = "#version 300 es\n";
    std::string version_string = "#version ";
    version_string += to_string(version.major) + to_string(version.minor) + '0';
    version_string += version.is_gles ? " es" : "";
    version_string += '\n';

    auto getSource = [version_string](auto shaderFile) {
        std::ifstream shader_file;
        shader_file.open(shaderFile);

        std::stringstream buffer;
        buffer << version_string << shader_file.rdbuf();

        return buffer.str();
    };

    auto v = getSource(vertShaderFile);
    auto f = getSource(fragShaderFile);
    auto vertexSource = v.c_str();
    auto fragmentSource = f.c_str();

    GLuint shaderProgram = glCreateProgram();
    // Bind attrib locations
    glBindAttribLocation(shaderProgram, 0, "position");
    glBindAttribLocation(shaderProgram, 1, "normal");
    glBindAttribLocation(shaderProgram, 2, "texcoord");
    glBindAttribLocation(shaderProgram, 3, "color");

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GetCompileErrors(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    GetCompileErrors(fragmentShader);

    // Link the vertex and fragment shader into a shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GetLinkErrors(shaderProgram);

    // Don't need these anymore
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);

    std::cout << "Compiled shader " << shaderProgram << std::endl;

    return shaderProgram;
}

void ShaderProgram::GetCompileErrors(GLuint shader) {
    GLint compiled;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        int logLength;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, infoLog);

        std::string errorText = infoLog;

        delete[] infoLog;

        std::cout << "Shader compile error: " << errorText << std::endl;

        throw ShaderCompileError{errorText};
    }
}

void ShaderProgram::GetLinkErrors(GLuint shaderProgram) {
    GLint linked;

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
        int logLength;

        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetProgramInfoLog(shaderProgram, logLength, &logLength, infoLog);

        std::string errorText = infoLog;

        delete[] infoLog;

        std::cout << "Shader link error: " << errorText << std::endl;
        
        throw ShaderLinkError{errorText};
    }
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

ShaderProgram& ShaderManager::AddShader(const ShaderIdentifier& id) {
    auto shaderIter = shaderMap.find(id);
    if (shaderIter == shaderMap.end()) {
        // Shader is not already in shaderMap, so add it
        const auto& [newShaderIter, b] = shaderMap.emplace(id, id);
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

void ShaderProgram::SetupTextures() const noexcept {
    UseProgram();
    // Set sampler uniforms
    glUniform1i(glGetUniformLocation(shaderProgram, "texDiffuseMap"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texSpecMap"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texNormalMap"), 2);
}

void ShaderProgram::UseProgram() const noexcept { glUseProgram(shaderProgram); }