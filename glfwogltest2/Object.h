#include <GL/glew.h>

#define VERTEX_SIZE 5

class Object {
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
	Object(float* vertices, int numVertices, GLuint* elements, int numElements, GLuint shaderProgram);
	~Object();
};