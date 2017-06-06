#include "GameObject.h"
#include <SOIL/SOIL.h>

//TODO: lots of parameters and long initializer list, maybe create Time object?
GameObject::GameObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime, TextureManager& _texman) : 
	mesh(_mesh), transform(_transform), elapsedTime(_elapsedTime), deltaTime(_deltaTime), texman(_texman), shaderProgram(_shaderProgram)
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
GameObject::GameObject(const GameObject& rhs) : 
	mesh(rhs.mesh), transform(rhs.transform), elapsedTime(rhs.elapsedTime), deltaTime(rhs.deltaTime), texman(rhs.texman), shaderProgram(rhs.shaderProgram)
{
	std::cout << "GameObject copy constructor\n";
}


// Renders the object
void GameObject::Draw(Camera camera) const
{
    //Make sure the right vertex array is bound
    glBindVertexArray(vao);
	BindTextures();
	glUseProgram(shaderProgram.shaderProgram);
	GLint uniModel = glGetUniformLocation(shaderProgram.shaderProgram, "model");

	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(transform));

	auto view = camera.viewMat;
	GLint uniView = glGetUniformLocation(shaderProgram.shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	if (mesh.usesElementArray)
	{
        //TODO: Use indexes from imported mesh as element buffer
		glDrawElements(GL_TRIANGLES, mesh.elements.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size()/8);
	}
}

void GameObject::SetupTextures() const
{
	//Set sampler uniforms
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texDiffuseMap"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texSpecMap"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texNormalMap"), 2);
	glUniform1i(glGetUniformLocation(shaderProgram.shaderProgram, "texPuppy"), 3);
}

void GameObject::BindTextures() const
{
	//Bind texture objects to texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texman.textureObjects[1]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texman.textureObjects[3]);
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
