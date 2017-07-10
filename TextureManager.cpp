#include "TextureManager.h"

//TODO: Support creating multiple textures at once
void TextureManager::AddTexture(const char* id, const char* fileName)
{
	//Add a new value to the vector
	textureObjects.emplace(id, 0);
	//Set the new value to a valid texture object
	glGenTextures(1, &textureObjects[id]);

    glBindTexture(GL_TEXTURE_2D, textureObjects[id]);
    
	//Load texture from file and upload to GPU
	int width, height;
	unsigned char* image;

	//TODO: check for file existence
	image = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
    
    for(auto it: textureObjects)
    {
        glDeleteTextures(1, &it.second);
    }
}
