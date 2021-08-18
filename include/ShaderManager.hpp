#pragma once

#include "GLVersion.hpp"

#include <string>
#include <functional>
#include <optional>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>

class ShaderError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class ShaderCompileError : public ShaderError {
  public:
    ShaderCompileError(const std::string& fileName, const std::string& message)
        : ShaderError{"Error compiling shader file \"" + fileName + "\"\n" + message} {}
};

class ShaderLinkError : public ShaderError {
    using ShaderError::ShaderError;
};

struct ShaderIdentifier {
    const std::string vertShader;
    const std::string fragShader;
    const GameEngine::GLVersion version;

    friend bool operator==(const ShaderIdentifier&, const ShaderIdentifier&) noexcept = default;
};

// TODO: Find a better way to combine hashes
namespace std {
template <> struct hash<ShaderIdentifier> {
    std::size_t operator()(const ShaderIdentifier& id) const {
        using std::to_string;

        std::string concat = id.vertShader + id.fragShader;
            
        auto [major, minor, es] = id.version;
            
        concat += to_string(major);
        concat += to_string(minor);
        concat += to_string(es);

        return std::hash<std::string>{}(concat);
    }
};
} // namespace std

class [[nodiscard]] ShaderProgram {
    [[nodiscard]] std::uint32_t ShaderProgramFromFiles(const std::string& vertShaderFile, const std::string& fragShaderFile);
    std::optional<std::string> GetCompileErrors(std::uint32_t shader);
    std::optional<std::string> GetLinkErrors(std::uint32_t shaderProgram);

    GameEngine::GLVersion version = {};

    friend void swap(ShaderProgram& sp1, ShaderProgram& sp2) noexcept;

    [[nodiscard]] ShaderProgram(ShaderProgram&& sp) noexcept;
    ShaderProgram& operator=(ShaderProgram&& sp) noexcept;
  public:
    std::uint32_t shaderProgram = 0;

    void SetupTextures() const noexcept;
    // Calls glUseProgram
    void UseProgram() const noexcept;
    
    [[nodiscard]] ShaderProgram(const ShaderIdentifier& id);
    
    // Copy constructor and assignment are deleted because you can't copy OpenGL objects
    ShaderProgram(const ShaderProgram& sp) = delete;
    ShaderProgram& operator=(const ShaderProgram& sp) = delete;

    ~ShaderProgram();
};

class ShaderManager {
    std::unordered_map<ShaderIdentifier, ShaderProgram> shaderMap;
    std::unordered_map<std::string, ShaderIdentifier> shaderNameMap;

    ShaderManager() = default;
  public:
    ShaderProgram& FromName(const std::string& name);

    // Adds a shader named name using the files specified in id
    // Calling this function multiple times with the same arguments does nothing
    // Calling this function with an existing name but a different id throws a runtime_exeption
    ShaderProgram& AddShader(std::string name, const ShaderIdentifier& id);

    [[nodiscard]] static ShaderManager& GetInstance();
    // Deleted to prevent copies
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    // Calculates a new projection matrix and updates the shaders' uniforms
    void UpdateProjectionMatrix(float width, float height) noexcept;
};

thread_local inline ShaderManager& shaderManager = ShaderManager::GetInstance();

struct VertexShaderAttribute {
    static constexpr int position = 0;
    static constexpr int normal = 1;
    static constexpr int texcoord = 2;
    static constexpr int color = 3;
};
