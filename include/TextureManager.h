#pragma once
#include "glad/glad.h"

#include <string>
#include <unordered_map>
#include <vector>

class TextureManager {
  public:
    std::unordered_map<std::string, GLuint> textureObjects;

    void AddTexture(const char *id, const char *fileName);
    TextureManager();
    ~TextureManager();
};
