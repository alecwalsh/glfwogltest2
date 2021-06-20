#pragma once

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

struct gl_version_t {
    std::uint8_t major;
    std::uint8_t minor;
    bool is_gles; // OpenGL or OpenGL ES

#ifdef __cpp_impl_three_way_comparison
    friend constexpr bool operator==(const gl_version_t& lhs, const gl_version_t& rhs) noexcept = default;
#else
    friend constexpr bool operator==(const gl_version_t& lhs, const gl_version_t& rhs) noexcept {
        return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.is_gles == rhs.is_gles;
    }
#endif // __cpp_impl_three_way_comparison
};

struct ShaderIdentifier {
    const std::string vertShader;
    const std::string fragShader;
    const gl_version_t version;

#ifdef __cpp_impl_three_way_comparison
    #ifdef __cpp_lib_constexpr_string
        #define STRING_CONSTEXPR constexpr
    #else
        #define STRING_CONSTEXPR
    #endif // __cpp_lib_constexpr_string
    
    friend STRING_CONSTEXPR inline bool operator==(const ShaderIdentifier& lhs, const ShaderIdentifier& rhs) noexcept = default;
    #undef STRING_CONSTEXPR
#else
    friend inline bool operator==(const ShaderIdentifier& lhs, const ShaderIdentifier& rhs) noexcept {
        return lhs.vertShader == rhs.vertShader && lhs.fragShader == rhs.fragShader && lhs.version == rhs.version;
    }
#endif // __cpp_impl_three_way_comparison
};

#ifndef __cpp_impl_three_way_comparison

#endif // !__cpp_impl_three_way_comparison

//TODO: Find a better way to combine hashes
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
    gl_version_t version;

    // Move constructor and assignment need to set shaderProgram to 0
    [[nodiscard]] ShaderProgram(ShaderProgram&& sp) noexcept;
    ShaderProgram& operator=(ShaderProgram&& sp) noexcept;
  public:
    std::uint32_t shaderProgram;

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
    ShaderManager() = default;
  public:
    [[nodiscard]] ShaderProgram& AddShader(const ShaderIdentifier& id);

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
