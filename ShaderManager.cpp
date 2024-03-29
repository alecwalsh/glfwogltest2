#include "ShaderManager.hpp"

#include "glad/glad.h"

#include <fstream>
#include <sstream>
#include <utility>
#include <filesystem>

#include <cassert>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include "Window.hpp"

namespace {
[[nodiscard]] std::optional<std::string> GetCompileErrors(GLuint shader) {
    GLint compiled;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        GLint logLength;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        auto* infoLog = new GLchar[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, infoLog);

        std::string errorText = infoLog;

        delete[] infoLog;

        return errorText;
    }

    return {};
}

[[nodiscard]] std::optional<std::string> GetLinkErrors(GLuint shaderProgram) {
    GLint linked;

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
        GLint logLength;

        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        auto* infoLog = new GLchar[logLength];
        glGetProgramInfoLog(shaderProgram, logLength, &logLength, infoLog);

        std::string errorText = infoLog;

        delete[] infoLog;

        return errorText;
    }

    return {};
}

[[nodiscard]] std::string getSourceFromPath(const std::filesystem::path& shaderFileName) {
    std::ifstream shaderFile;
    shaderFile.open(shaderFileName);

    std::stringstream buffer;
    buffer << Window::VersionString() << shaderFile.rdbuf();

    return buffer.str();
}

[[nodiscard]] std::uint32_t ShaderProgramFromFiles(std::string_view vertShaderFile, std::string_view fragShaderFile) {
    auto v = getSourceFromPath(vertShaderFile);
    auto f = getSourceFromPath(fragShaderFile);
    auto vertexSource = v.c_str();
    auto fragmentSource = f.c_str();

    GLuint shaderProgram = glCreateProgram();

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    if (auto compileError = GetCompileErrors(vertexShader)) {
        throw ShaderCompileError{vertShaderFile, *compileError};
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
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

    spdlog::info("Compiled shader program {}", shaderProgram);

    return shaderProgram;
}
}

ShaderCompileError::ShaderCompileError(std::string_view fileName, std::string_view message)
    : ShaderError{fmt::format("Error compiling shader file \"{}\"\n{}", fileName, message)} {}

ShaderProgram::ShaderProgram(const ShaderIdentifier& id)
    : shaderProgram{ShaderProgramFromFiles(id.vertShader, id.fragShader)} {}

void ShaderProgram::Delete() noexcept {
    if (shaderProgram != 0) {
        spdlog::info("Deleting shader program {}", shaderProgram);

        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

ShaderProgram::~ShaderProgram() { Delete(); }

void swap(ShaderProgram& sp1, ShaderProgram& sp2) noexcept {
    using std::swap;

    swap(sp1.shaderProgram, sp2.shaderProgram);
}

ShaderProgram::ShaderProgram(ShaderProgram&& sp) noexcept {
    *this = std::move(sp);
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& sp) noexcept {
    if (this != &sp) {
        Delete();

        using std::swap;
        swap(*this, sp);

        assert(sp.shaderProgram == 0);
    }
    
    return *this;
}

// TODO: Don't compile shader until it is used
ShaderProgram& ShaderManager::AddShader(std::string_view name, const ShaderIdentifier& id) {
    auto nameIter = shaderNameMap.find(name);

    if (nameIter != shaderNameMap.end()) {
        const auto& newID = nameIter->second;

        if (newID != id) {
            throw std::runtime_error{"Attempted to reassign shader " + std::string{name}};
        }

        return shaderMap.at(newID);
    }

    shaderNameMap.emplace(name, id);

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

ShaderProgram& ShaderManager::FromName(std::string_view name) { 
    auto it = shaderNameMap.find(name);
    if (it == shaderNameMap.end()) throw std::out_of_range{"Shader not found"};

    return shaderMap.at(it->second);
}

void ShaderProgram::SetupTextures() const noexcept {
    UseProgram();
    // Set sampler uniforms
    glUniform1i(glGetUniformLocation(shaderProgram, "texDiffuseMap"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texSpecMap"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texNormalMap"), 2);
}

void ShaderProgram::UseProgram() const noexcept { glUseProgram(shaderProgram); }
