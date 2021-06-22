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
    glGenBuffers(1, &buffers.vbo);

    glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(meshData.vertices[0]) * meshData.vertices.size(), meshData.vertices.data(),
                 GL_STATIC_DRAW);

    if (meshData.usesElementArray) {
        if (meshData.elements.size() > std::numeric_limits<GLsizei>::max()) {
            std::cerr << "Number of elements exceeds max GLsizei value" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Create an Element Buffer Object and copy the element data to it
        glGenBuffers(1, &buffers.ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(meshData.elements[0]) * meshData.elements.size(),
                     meshData.elements.data(),
                     GL_STATIC_DRAW);
    }
}

MeshBase::MeshBase(const std::vector<MeshData::Vertex>& vertices) : meshData{vertices} {
    UploadToGPU();
}

MeshBase::MeshBase(const std::vector<MeshData::Vertex>& vertices, const std::vector<std::uint32_t>& elements)
    : meshData{vertices, elements, true} {
    UploadToGPU();
}

MeshBase::~MeshBase() {
    GLuint bufarray[] = {buffers.vbo, buffers.ebo};
    glDeleteBuffers(2, bufarray);
}