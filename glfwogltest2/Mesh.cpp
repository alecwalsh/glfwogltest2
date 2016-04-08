#include "Mesh.h"
#include <iostream>



Mesh::Mesh(std::vector<GLfloat> vertices): usesElementArray(false)
{
	this->vertices = vertices;
	UploadToGPU();
}

Mesh::Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> elements) : usesElementArray(true)
{
	this->vertices = vertices;
	this->elements = elements;
	UploadToGPU();
}

Mesh::~Mesh()
{
}


// Generates buffers and uploads data to graphics card
void Mesh::UploadToGPU()
{
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	GLuint ebo;

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	buffers.vbo = vbo;
	buffers.ebo = ebo;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*(vertices.size()), vertices.data(), GL_STATIC_DRAW);

	if (usesElementArray)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements[0])*(elements.size()), elements.data(), GL_STATIC_DRAW);
	}
}
