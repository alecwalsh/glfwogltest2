#pragma once

#include "GLVersion.hpp"

#include "CameraBase.hpp"

#include <string>
#include <string_view>
#include <functional>
#include <optional>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>

class ShaderError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class ShaderCompileError : public ShaderError {
  public:
    ShaderCompileError(std::string_view fileName, std::string_view message)
        : ShaderError{"Error compiling shader file \"" + std::string{fileName} + "\"\n" + std::string{message}} {}
};

class ShaderLinkError : public ShaderError {
    using ShaderError::ShaderError;
};

struct ShaderIdentifier {
    const std::string vertShader;
    const std::string fragShader;

    friend bool operator==(const ShaderIdentifier&, const ShaderIdentifier&) noexcept = default;
};

namespace std {
template <> struct hash<ShaderIdentifier> {
    // This technique for combining hashes was taken from Boost.ContainerHash
    template <typename T, typename... Ts>
    static constexpr std::size_t hash_combine(const T& first, const Ts&... rest) noexcept {
        auto seed = std::hash<T>{}(first);

        ((seed ^= std::hash<Ts>{}(rest) + 0x9e3779b9 + (seed << 6) + (seed >> 2)), ...);

        return seed;
    }

    std::size_t operator()(const ShaderIdentifier& id) const {
        return hash_combine(id.vertShader, id.fragShader);
    }
};
} // namespace std

class [[nodiscard]] ShaderProgram final {
    [[nodiscard]] std::uint32_t ShaderProgramFromFiles(std::string_view vertShaderFile, std::string_view fragShaderFile);
    std::optional<std::string> GetCompileErrors(std::uint32_t shader);
    std::optional<std::string> GetLinkErrors(std::uint32_t shaderProgram);

    friend void swap(ShaderProgram& sp1, ShaderProgram& sp2) noexcept;

    [[nodiscard]] ShaderProgram(ShaderProgram&& sp) noexcept;
    ShaderProgram& operator=(ShaderProgram&& sp) noexcept;

    // Calls glDeleteProgram and sets shaderProgram to 0
    void Delete() noexcept;
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
    std::map<std::string, ShaderIdentifier, std::less<>> shaderNameMap;

    ShaderManager() = default;
  public:
    ShaderProgram& FromName(std::string_view name);

    // Adds a shader named name using the files specified in id
    // Calling this function multiple times with the same arguments does nothing
    // Calling this function with an existing name but a different id throws a runtime_exeption
    ShaderProgram& AddShader(std::string_view name, const ShaderIdentifier& id);

    [[nodiscard]] static ShaderManager& GetInstance();
    // Deleted to prevent copies
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;
};

thread_local inline ShaderManager& shaderManager = ShaderManager::GetInstance();

struct VertexShaderAttribute {
    static constexpr std::uint32_t position = 0;
    static constexpr std::uint32_t normal = 1;
    static constexpr std::uint32_t texcoord = 2;
    static constexpr std::uint32_t color = 3;
};
