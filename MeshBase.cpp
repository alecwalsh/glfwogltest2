#include "MeshBase.hpp"

#include "glad/glad.h"

#include <iostream>
#include <limits>
#include <cstdlib>

// Generates buffers and uploads data to graphics card
void MeshBase::UploadToGPU() {
    if (meshData.vertices.size() > std::numeric_limits<GLsizei>::max()) {
        std::cerr << "Number of vertices exceeds max GLsizei value" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Create a Vertex Buffer Object and copy the vertex data to it
    buffers.vbo.GenBuffer();

    buffers.vbo.Bind(GL_ARRAY_BUFFER);

    glBufferData(GL_ARRAY_BUFFER, sizeof(meshData.vertices[0]) * meshData.vertices.size(), meshData.vertices.data(),
                 GL_STATIC_DRAW);

    if (meshData.usesElementArray) {
        if (meshData.elements.size() > std::numeric_limits<GLsizei>::max()) {
            std::cerr << "Number of elements exceeds max GLsizei value" << std::endl;
            std::exit(EXIT_FAILURE);
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
