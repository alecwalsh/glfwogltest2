#include "MeshBase.hpp"

#include "glad/glad.h"

#include <limits>
#include <stdexcept>

// Generates buffers and uploads data to graphics card
void MeshBase::UploadToGPU() {
    if (meshData.vertices.size() > std::numeric_limits<GLsizei>::max()) {
        throw std::runtime_error{"Number of vertices exceeds max GLsizei value"};
    }

    // Create a Vertex Buffer Object and copy the vertex data to it
    buffers.vbo.GenBuffer();

    buffers.vbo.Bind(GL_ARRAY_BUFFER);

    glBufferData(GL_ARRAY_BUFFER, sizeof(meshData.vertices[0]) * meshData.vertices.size(), meshData.vertices.data(),
                 GL_STATIC_DRAW);

    if (meshData.usesElementArray) {
        if (meshData.elements.size() > std::numeric_limits<GLsizei>::max()) {
            throw std::runtime_error{"Number of elements exceeds max GLsizei value"};
        }

        // Create an Element Buffer Object and copy the element data to it
        buffers.ebo.GenBuffer();

        buffers.ebo.Bind(GL_ELEMENT_ARRAY_BUFFER);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(meshData.elements[0]) * meshData.elements.size(),
                     meshData.elements.data(),
                     GL_STATIC_DRAW);
    }
}

MeshBase::MeshBase(MeshData meshData) : meshData{std::move(meshData)} {
        UploadToGPU();
}
