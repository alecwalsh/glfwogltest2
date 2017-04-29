#pragma once

#include <vector>

#include <GL/glew.h>
#include <SOIL.h>

class TextureManager {
public:
	std::vector<GLuint> textureObjects;

	void NewTexture(const char* fileName, GLenum textureUnit);
	TextureManager();
	~TextureManager();
};