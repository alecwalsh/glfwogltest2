#include "RenderableObject.hpp"

#include "glad/glad.h"

#include "Window.hpp"

#include <iostream>

RenderableObject::RenderableObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman)
    : mesh{mesh}, texman{texman}, shaderProgram{shaderProgram} {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.buffers.vbo);
    if (mesh.meshData.usesElementArray) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.buffers.ebo);
    }

    shaderProgram.UseProgram();

    GLint posAttrib = VertexShaderAttribute::position;
    GLint normalAttrib = VertexShaderAttribute::normal;
    GLint texAttrib = VertexShaderAttribute::texcoord;

    // Specify the layout of the vertex data
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), 0);

    glEnableVertexAttribArray(normalAttrib);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), (void*)(6 * sizeof(float)));

    Window& window = Window::GetInstance();

    // Set up projection matrix
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)window.width / window.height, 1.0f, 100.0f);
    GLint uniProj = glGetUniformLocation(shaderProgram.shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    std::cout << "RenderableObject constructor\n";
}

// Copy constructor
RenderableObject::RenderableObject(const RenderableObject& rhs)
    : GameObject{rhs}, mesh{rhs.mesh}, texman{rhs.texman}, shaderProgram{rhs.shaderProgram} {
    std::cout << "RenderableObject copy constructor" << std::endl;
}

// Renders the object
void RenderableObject::Draw(const Camera& camera) const {
    // Make sure the right vertex array is bound
    glBindVertexArray(vao);
    BindTextures();
    shaderProgram.UseProgram();

    GLint uniModel = glGetUniformLocation(shaderProgram.shaderProgram, "model");

    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(GetTransform()));

    GLint uniView = glGetUniformLocation(shaderProgram.shaderProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

    if (mesh.meshData.vertices.size() > std::numeric_limits<GLsizei>::max()) {
        std::cerr << "Too many vertices" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (mesh.meshData.usesElementArray) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.meshData.elements.size()), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.meshData.vertices.size()));
    }
}

void RenderableObject::BindTextures() const {
    // Bind texture objects to texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texman.textureObjects[texture_name]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texman.textureObjects[spec_texture_name]);
}