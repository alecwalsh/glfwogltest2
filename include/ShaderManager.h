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

class ShaderProgram {
    GLuint ShaderProgramFromFiles(const std::string& vertShaderFile, const std::string& fragShaderFile);
    void getCompileErrors(GLuint shader);
    void getLinkErrors(GLuint shaderProgram);
    gl_version_t version;
  public:
    GLuint shaderProgram;

    ShaderProgram(const ShaderIdentifier& id);
    
	// Move constructor and assignment need to set shaderProgram to 0	
	ShaderProgram(ShaderProgram&& sp) noexcept;
    ShaderProgram& operator=(ShaderProgram&& sp) noexcept;

	// Copy constructor and assignment are deleted because you can't copy OpenGL objects
    ShaderProgram(const ShaderProgram& sp) noexcept = delete;
	ShaderProgram& operator=(const ShaderProgram& sp) noexcept = delete;
    
	~ShaderProgram();
};

class ShaderManager {
  public:
    ShaderProgram& addShader(const ShaderIdentifier& id);

    static ShaderManager& GetInstance();
    // Deleted to prevent copies
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

	const std::unordered_map<ShaderIdentifier, ShaderProgram>& getMap();
  private:
    std::unordered_map<ShaderIdentifier, ShaderProgram> shaderMap;
    ShaderManager() = default;
};

inline ShaderManager& shaderManager = ShaderManager::GetInstance();