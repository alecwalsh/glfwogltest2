#include "TextureManager.h"

//TODO: Support creating multiple textures at once
//TODO: Retrieve textures by name instead of OpenGL object, probably std::unordered_map
void TextureManager::AddTexture(const char* fileName)
{
	//Add a new value to the vector
	textureObjects.push_back(0);
	//Set the new value to a valid texture object
	glGenTextures(1, &textureObjects[textureObjects.size()-1]);

	//Load texture from file and upload to GPU
	int width, height;
	unsigned char* image;

	//This is always set to GL_TEXTURE0 because we need to bind a texture unit to bind a texture object
	//but we don't care which texture unit it is right now
	//We set the correct texture unit for rendering in the GameObject
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObjects[textureObjects.size()-1]);

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
	glDeleteTextures(textureObjects.size(), textureObjects.data());
}