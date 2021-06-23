#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

class TextureManager {
  public:
    std::unordered_map<std::string, std::uint32_t> textureObjects;

    void AddTextureFromFile(const char* id, const char* fileName);
    void AddTextureFromGLObject(const char* id, std::uint32_t texture);
    
    ~TextureManager();
};
