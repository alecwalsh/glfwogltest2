#pragma once
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define VERTEX_SIZE 8

class RenderObject {
	float *vertices;
	GLuint* elements;
	int numElements;
	GLuint shaderProgram;

	struct buffers {
		GLuint vbo;
		GLuint ebo;
	} buffers;


public:
	GLuint vao;

	void Draw();
	void SetupTextures();
	RenderObject(float* vertices, int numVertices, GLuint* elements, int numElements, GLuint shaderProgram);
	~RenderObject();
};