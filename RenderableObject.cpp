#include "RenderableObject.hpp"

#include "glad/glad.h"

#include "Window.hpp"

#include <iostream>
#include <limits>

namespace GameEngine {

RenderableObject::RenderableObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman)
    : mesh{mesh}, texman{texman}, shaderProgram{shaderProgram} {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    mesh.buffers.vbo.Bind(GL_ARRAY_BUFFER);
    
    if (mesh.meshData.usesElementArray) {
        mesh.buffers.ebo.Bind(GL_ELEMENT_ARRAY_BUFFER);
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

    std::cout << "RenderableObject constructor\n";
}

// Copy constructor
RenderableObject::RenderableObject(const RenderableObject& rhs)
    : GameObject{rhs}, mesh{rhs.mesh}, texman{rhs.texman}, shaderProgram{rhs.shaderProgram} {
    std::cout << "RenderableObject copy constructor" << std::endl;
}

// Renders the object
void RenderableObject::Draw(const CameraBase& camera) const {
    // Make sure the right vertex array is bound
    glBindVertexArray(vao);
    BindTextures();
    shaderProgram.UseProgram();

    // Set model, view, and projection matrices
    GLint uniModel = glGetUniformLocation(shaderProgram.shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(GetTransform()));

    GLint uniView = glGetUniformLocation(shaderProgram.shaderProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

    GLint uniProj = glGetUniformLocation(shaderProgram.shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));

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


// Sets up the lights, then calls RenderableObject::Draw
void RenderableObject::RenderObject(std::span<const std::unique_ptr<Light>> lights,
                                    const CameraBase& camera) {
    const auto& sp = shaderProgram;

    sp.UseProgram();

    GLint numLightsUniform = glGetUniformLocation(sp.shaderProgram, "numLights");

    std::size_t numLights = lights.size();

    if (numLights < 0 || numLights > std::numeric_limits<GLint>::max()) {
        std::cerr << "Invalid number of lights" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    glUniform1i(numLightsUniform, static_cast<GLint>(numLights));

    for (std::size_t i = 0; i < lights.size(); i++) {
        // TODO: don't do this every frame
        lights[i]->SetUniforms(sp.shaderProgram, i);
    }

    GLint ambientLoc = glGetUniformLocation(sp.shaderProgram, "uniAmbient");

    float ambient = 0.5f;
    // TODO: Don't hardcode ambient value
    glUniform3f(ambientLoc, ambient, ambient, ambient);

    Draw(camera);
}

} // namespace GameEngine