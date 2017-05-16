#pragma once
#include <GL/glew.h>

#include <vector>

#define VERTEX_SIZE 8

class Mesh
{
public:
	std::vector<GLfloat> vertices;
	std::vector<GLuint> elements;

	const bool usesElementArray;  //Set to true or false depending on which constructor is called

	struct buffers {
		GLuint vbo;
		GLuint ebo;
	} buffers;

	Mesh(std::vector<GLfloat> vertices);
	Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> elements);
	Mesh(std::string fileName); //Reads vertex data from text file
	~Mesh();

	// Generates buffers and uploads data to graphics card
	void UploadToGPU();
};

