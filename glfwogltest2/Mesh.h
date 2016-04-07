#pragma once
#include <GL/glew.h>

#include <vector>

class Mesh
{
	const bool usesElementArray;  //Set to true or false depending on which constructor is called
public:
	std::vector<GLfloat> vertices;
	std::vector<GLuint> elements;

	Mesh(std::vector<GLfloat> vertices);
	Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> elements);
	~Mesh();
};

