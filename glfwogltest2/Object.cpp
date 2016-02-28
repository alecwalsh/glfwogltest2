#include "Object.h"

void Object::Draw() {
	// Bind vertex array object
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
}

Object::Object(float* vertices, int numVertices, GLuint* elements, int numElements, GLuint shaderProgram) : vertices(vertices), elements(elements), numElements(numElements), shaderProgram(shaderProgram) {
	// Create Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	GLuint ebo;

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	buffers.vbo = vbo;
	buffers.ebo = ebo;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*VERTEX_SIZE*numVertices, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements[0])*numElements, elements, GL_STATIC_DRAW);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
		5 * sizeof(float), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		5 * sizeof(float), (void*)(2 * sizeof(float)));

	glUseProgram(shaderProgram);

	glBindVertexArray(0);

	this->vao = vao;

}

Object::~Object() {
	glDeleteBuffers(1, &buffers.vbo);
	glDeleteBuffers(1, &buffers.ebo);
	glDeleteVertexArrays(1, &vao);
}