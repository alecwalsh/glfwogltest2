#include "Mesh.h"



Mesh::Mesh(std::vector<GLfloat> vertices): usesElementArray(false)
{
	this->vertices = vertices;
}

Mesh::Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> elements) : usesElementArray(true)
{
	this->vertices = vertices;
	this->elements = elements;
}

Mesh::~Mesh()
{
}
