#pragma once

#include <vector>

#include <GL/glew.h>
#include <SOIL.h>

class TextureManager {
public:
	std::vector<GLuint> textureObjects;

	void AddTexture(const char* fileName);
	TextureManager();
	~TextureManager();
};