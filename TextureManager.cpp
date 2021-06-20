#include "TextureManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>
#include <filesystem>

// TODO: Support creating multiple textures at once
void TextureManager::AddTextureFromFile(const char* id, const char* fileName) {
    // Prepend the directory name to fileName
    std::filesystem::path finalFileName = "data/textures";
    finalFileName /= fileName;

    if (!std::filesystem::exists(finalFileName)) {
        throw std::runtime_error{"Texture file does not exist"};
    }

    // Add a new value to the map
    textureObjects.emplace(id, 0);
    // Set the new value to a valid texture object
    glGenTextures(1, &textureObjects[id]);

    glBindTexture(GL_TEXTURE_2D, textureObjects[id]);

    // Load texture from file and upload to GPU
    int width, height;
    unsigned char* image;

    image = stbi_load(finalFileName.string().c_str(), &width, &height, 0, 3);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TextureManager::AddTextureFromGLObject(const char* id, GLuint texture) {
    // Add the texture to the map
    textureObjects.emplace(id, texture);
}

TextureManager::~TextureManager() {
    for (auto& it : textureObjects) {
        glDeleteTextures(1, &it.second);
    }
}
