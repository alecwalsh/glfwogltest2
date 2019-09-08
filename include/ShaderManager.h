#pragma once
#include "glad/glad.h"

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

using gl_version_t = std::tuple<int, int, bool>;

struct ShaderIdentifier {
    const std::string vertShader;
    const std::string fragShader;
    const gl_version_t version;
};

bool operator==(const ShaderIdentifier& lhs, const ShaderIdentifier& rhs) noexcept;

namespace std {
template <> struct hash<ShaderIdentifier> {
	std::size_t operator()(const ShaderIdentifier& id) const {
			std::string concat = id.vertShader + id.fragShader;
            concat += std::to_string(std::get<0>(id.version));
            concat += std::to_string(std::get<1>(id.version));
			concat += std::to_string(std::get<2>(id.version));

			return std::hash<std::string>{}(concat);
	}
};
} // namespace std

class ShaderProgram {
    GLuint ShaderProgramFromFiles(const std::string& vertShaderFile, const std::string& fragShaderFile);
    void getCompileErrors(GLuint shader);
    void getLinkErrors(GLuint shaderProgram);
    gl_version_t version;
  public:
    GLuint shaderProgram;

    ShaderProgram(const ShaderIdentifier& id);
    
	// Copy constructor can be defaulted, move constructor needs to set shaderProgram to 0
	ShaderProgram(const ShaderProgram& sp) = default;
	ShaderProgram(ShaderProgram&& sp) noexcept;
    
	ShaderProgram& operator=(ShaderProgram&& sp) noexcept;
    ShaderProgram& operator=(const ShaderProgram& sp) noexcept;
    
	~ShaderProgram();
};

class ShaderManager {
  public:
    ShaderProgram& addShader(const ShaderIdentifier& id);

    static ShaderManager& GetInstance();
    // Deleted to prevent copies
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

  private:
    ShaderManager() = default;
    std::unordered_map<ShaderIdentifier, ShaderProgram> shaderMap;
};

inline ShaderManager& shaderManager = ShaderManager::GetInstance();