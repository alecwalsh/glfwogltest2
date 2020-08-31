#include "MeshBase.h"


// Generates buffers and uploads data to graphics card
void MeshBase::UploadToGPU() {
    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &buffers.vbo);

    glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    if (usesElementArray) {
        // Create an Element Buffer Object and copy the element data to it
        glGenBuffers(1, &buffers.ebo);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements[0]) * elements.size(), elements.data(), GL_STATIC_DRAW);
    }
}

MeshBase::MeshBase(const std::vector<Vertex>& vertices) : vertices{vertices} {
    UploadToGPU();
}

MeshBase::MeshBase(const std::vector<Vertex>& vertices, const std::vector<GLuint>& elements)
    : vertices{vertices}, elements{elements}, usesElementArray{true} {
    UploadToGPU();
}

MeshBase::~MeshBase() {
    GLuint bufarray[] = {buffers.vbo, buffers.ebo};
    glDeleteBuffers(2, bufarray);
}