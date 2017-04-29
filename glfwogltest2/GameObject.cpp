#include "GameObject.h"
#include <SOIL.h>


GameObject::GameObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime) : mesh(_mesh), transform(_transform), elapsedTime(_elapsedTime), deltaTime(_deltaTime), shaderProgram(_shaderProgram)
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
	//Use attrib location from glBindAttribLocation
	GLint posAttrib = 0;
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		VERTEX_SIZE * sizeof(float), 0);

	GLint normalAttrib = 1;
	glEnableVertexAttribArray(normalAttrib);
	glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE,
		VERTEX_SIZE * sizeof(float), (void*)(3 * sizeof(float)));

	GLint texAttrib = 2;
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
		VERTEX_SIZE * sizeof(float), (void*)(6 * sizeof(float)));

	// Set up projection matrix

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram.shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	std::cout << "GameObject constructor\n";
}


GameObject::~GameObject()
{
	std::cout << "GameObject destructor\n";
}

//Copy constructor
GameObject::GameObject(const GameObject& rhs) : mesh(rhs.mesh), transform(rhs.transform), elapsedTime(rhs.elapsedTime), deltaTime(rhs.deltaTime), shaderProgram(rhs.shaderProgram)
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

//TODO: Move texture setup outside of class, reuse textures between objects
void GameObject::SetupTextures(TextureManager* texman)
{
	texman->NewTexture("normalmaptest1.png", GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texNormalMap"), 0);
	
	texman->NewTexture("container2.png", GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texDiffuseMap"), 1);

	texman->NewTexture("sample2.png", GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texPuppy"), 2);

	texman->NewTexture("container2_specular.png", GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texSpecMap"), 3);
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