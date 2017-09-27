#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstring>

// TODO: Support creating multiple textures at once
void TextureManager::AddTexture(const char *id, const char *fileName) {
    //Prepend the directory name to fileName
    const char* directoryName = "data/textures";
    size_t fileNameLength = strlen(fileName) + strlen(directoryName) + 2; //Need room for / and null terminator
    char* finalFileName = (char*)malloc(fileNameLength);
    finalFileName[0] = '\0';
    strcat(finalFileName, directoryName);
    strcat(finalFileName, "/");
    strcat(finalFileName, fileName);
    
    // Add a new value to the vector
    textureObjects.emplace(id, 0);
    // Set the new value to a valid texture object
    glGenTextures(1, &textureObjects[id]);

    glBindTexture(GL_TEXTURE_2D, textureObjects[id]);

    // Load texture from file and upload to GPU
    int width, height;
    unsigned char *image;

    // TODO: check for file existence
    image = stbi_load(finalFileName, &width, &height, 0, 3);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    
    free(finalFileName);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

TextureManager::TextureManager() {}

TextureManager::~TextureManager() {
    for (auto it : textureObjects) {
        glDeleteTextures(1, &it.second);
    }
}
