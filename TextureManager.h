#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <GL/glew.h>
#include <SOIL/SOIL.h>

class TextureManager {
public:
    std::unordered_map<std::string, GLuint> textureObjects;

    void AddTexture(const char* id, const char* fileName);
    TextureManager();
    ~TextureManager();
};
