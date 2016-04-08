#include "GameObject.h"
#include <SOIL.h>


GameObject::GameObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform) : mesh(_mesh), shaderProgram(_shaderProgram), transform(_transform)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.buffers.vbo);
	if (mesh.usesElementArray)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.buffers.ebo);
	}
	
	glUseProgram(shaderProgram.shaderProgram);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram.shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		VERTEX_SIZE * sizeof(float), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram.shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		VERTEX_SIZE * sizeof(float), (void*)(3 * sizeof(float)));

	GLint texAttrib = glGetAttribLocation(shaderProgram.shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
		VERTEX_SIZE * sizeof(float), (void*)(6 * sizeof(float)));

	// Set up view and projection matrices
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)//y-axis is up
	);
	GLint uniView = glGetUniformLocation(shaderProgram.shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram.shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	
	SetupTextures();
	
	std::cout << "GameObject constructor\n";
}


GameObject::~GameObject()
{
	std::cout << "GameObject destructor\n";
}


GameObject::GameObject(const GameObject& rhs) : mesh(rhs.mesh), shaderProgram(rhs.shaderProgram), transform(rhs.transform)
{
	std::cout << "GameObject copy constructor\n";
}

// Runs every frame
void GameObject::Tick()
{
	Draw();
}


// Renders the object
void GameObject::Draw()
{
	glUseProgram(shaderProgram.shaderProgram);
	GLint uniModel = glGetUniformLocation(shaderProgram.shaderProgram, "model");

	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(transform));

	if (mesh.usesElementArray)
	{
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}


void GameObject::SetupTextures() {
	//texture setup

	GLuint textures[2];
	glGenTextures(2, textures);

	//Load textures from file

	int width, height;
	unsigned char* image;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texKitten"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	image = SOIL_load_image("sample2.png", &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texPuppy"), 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void GameObject::SetTransform(glm::mat4 _transform)
{
	transform = _transform;
}
