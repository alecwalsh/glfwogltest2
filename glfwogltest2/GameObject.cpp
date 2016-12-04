#include "GameObject.h"
#include <SOIL.h>


GameObject::GameObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime) : mesh(_mesh), shaderProgram(_shaderProgram), transform(_transform), elapsedTime(_elapsedTime), deltaTime(_deltaTime)
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

	GLint normalAttrib = glGetAttribLocation(shaderProgram.shaderProgram, "normal");
	glEnableVertexAttribArray(normalAttrib);
	glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE,
		VERTEX_SIZE * sizeof(float), (void*)(3 * sizeof(float)));

	GLint normalTexAttrib = glGetAttribLocation(shaderProgram.shaderProgram, "normaltexcoord");
	glEnableVertexAttribArray(normalTexAttrib);
	glVertexAttribPointer(normalTexAttrib, 2, GL_FLOAT, GL_FALSE,
		VERTEX_SIZE * sizeof(float), (void*)(6 * sizeof(float)));

	//GLint texAttrib = glGetAttribLocation(shaderProgram.shaderProgram, "texcoord");
	//glEnableVertexAttribArray(texAttrib);
	//glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
	//	VERTEX_SIZE * sizeof(float), (void*)(6 * sizeof(float)));


	// Set up projection matrix

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

//Copy constructor
GameObject::GameObject(const GameObject& rhs) : mesh(rhs.mesh), shaderProgram(rhs.shaderProgram), transform(rhs.transform), elapsedTime(rhs.elapsedTime), deltaTime(rhs.deltaTime)
{
	std::cout << "GameObject copy constructor\n";
}


// Renders the object
void GameObject::Draw(Camera camera)
{
	glUseProgram(shaderProgram.shaderProgram);
	GLint uniModel = glGetUniformLocation(shaderProgram.shaderProgram, "model");

	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(transform));

	auto view = camera.viewMat;
	GLint uniView = glGetUniformLocation(shaderProgram.shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	if (mesh.usesElementArray)
	{
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}


//TODO: Move texture setup outside of class
void GameObject::SetupTextures() {
	//texture setup

	GLuint textures[4];
	glGenTextures(4, textures);

	//Load textures from file

	int width, height;
	unsigned char* image;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	image = SOIL_load_image("normalmaptest1.png", &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texNormalMap"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	image = SOIL_load_image("container2.png", &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texDiffuseMap"), 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);

	image = SOIL_load_image("sample2.png", &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texPuppy"), 2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[3]);

	image = SOIL_load_image("container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texSpecMap"), 3);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//Sets the transform
void GameObject::SetTransform(glm::mat4 _transform)
{
	transform = _transform;
}

//Modifies the transform
void GameObject::ModTransform(glm::mat4 _transform)
{
	transform *= _transform;
}