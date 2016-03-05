#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define VERTEX_SIZE 7

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
	void SetupTextures();
	Object(float* vertices, int numVertices, GLuint* elements, int numElements, GLuint shaderProgram);
	~Object();
};