#pragma once
#include "glad/glad.h"

#include <string>
#include <unordered_map>
#include <vector>

class TextureManager {
  public:
    std::unordered_map<std::string, GLuint> textureObjects;

    void AddTextureFromFile(const char *id, const char *fileName);
    void AddTextureFromGLObject(const char *id, GLuint texture);
    TextureManager();
    ~TextureManager();
};
