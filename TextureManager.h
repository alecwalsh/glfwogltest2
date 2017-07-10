#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <GL/glew.h>
#include <SOIL/SOIL.h>

//TODO: Move Texture to separate fileName
struct Texture
{
    //OpenGL texture object
    GLuint texObject;
    
    operator GLuint();
    
    ~Texture();
};


class TextureManager {
public:
    std::unordered_map<std::string, Texture> textureObjects;

    void AddTexture(const char* id, const char* fileName);
    TextureManager();
    ~TextureManager();
};
