#include "ShaderManager.h"

#include <fstream>
#include <iostream>
#include <sstream>

ShaderProgram::ShaderProgram(const ShaderIdentifier& id)
    : version{id.version}, shaderProgram{ShaderProgramFromFiles(id.vertShader, id.fragShader)} {}

// TODO: Make version_string work with OpenGL below 3.3
GLuint ShaderProgram::ShaderProgramFromFiles(const std::string& vertShaderFile, const std::string& fragShaderFile) {
    using std::get;
    using std::to_string;

    // std::string version = "#version 300 es\n";
    std::string version_string = "#version ";
    version_string += to_string(get<0>(version)) + to_string(get<1>(version)) + '0';
    version_string += get<2>(version) ? " es" : "";
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

    // Don't need these anymore
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);

    printf("Compiled shader %d\n", shaderProgram);
    fflush(stdout);
    return shaderProgram;
}

void ShaderProgram::getCompileErrors(GLuint shader) {
    GLint compiled;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        int logLength;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* infoLog = new GLchar[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, infoLog);

        printf("Compile error: %s\n", infoLog);
        delete[] infoLog;
        exit(EXIT_FAILURE);
    }
}

void ShaderProgram::getLinkErrors(GLuint shaderProgram) {
    GLint linked;

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
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
    std::cout << "Deleting shader program " << shaderProgram << std::endl;
    glDeleteProgram(shaderProgram);
}

ShaderProgram::ShaderProgram(ShaderProgram&& sp) noexcept {
    shaderProgram = std::move(sp.shaderProgram);
    sp.shaderProgram = 0;
    version = std::move(sp.version);
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& sp) noexcept {
    shaderProgram = std::move(sp.shaderProgram);
    sp.shaderProgram = 0;
    version = std::move(sp.version);
    return *this;
}

bool operator==(const ShaderIdentifier& lhs, const ShaderIdentifier& rhs) noexcept {
    return lhs.vertShader == rhs.vertShader && lhs.fragShader == rhs.fragShader && lhs.version == rhs.version;
}

ShaderProgram& ShaderManager::addShader(const ShaderIdentifier& id) {
    auto shaderIter = shaderMap.find(id);
    if (shaderIter == std::end(shaderMap)) {
        // Shader is not already in shaderMap, so add it
        const auto& [newShaderIter, b] = shaderMap.emplace(id, ShaderIdentifier{id});
        return newShaderIter->second;
    }

    return shaderIter->second;
}

ShaderManager& ShaderManager::GetInstance() {
    static ShaderManager sm{};
    return sm;
}