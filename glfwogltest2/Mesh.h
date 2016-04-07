#pragma once
#include <GL/glew.h>

#include <vector>

#define VERTEX_SIZE 8

class Mesh
{
	const bool usesElementArray;  //Set to true or false depending on which constructor is called

public:
	std::vector<GLfloat> vertices;
	std::vector<GLuint> elements;

	struct buffers {
		GLuint vbo;
		GLuint ebo;
	} buffers;

	Mesh(std::vector<GLfloat> vertices);
	Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> elements);
	~Mesh();

	// Generates buffers and uploads data to graphics card
	void UploadToGPU();
};

