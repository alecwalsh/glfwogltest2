#include "MeshBase.h"


// Generates buffers and uploads data to graphics card
void MeshBase::UploadToGPU() {
    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    GLuint ebo;

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    buffers.vbo = vbo;
    buffers.ebo = ebo;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * (vertices.size()), vertices.data(), GL_STATIC_DRAW);

    if (usesElementArray) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements[0]) * (elements.size()), elements.data(), GL_STATIC_DRAW);
    }
}

MeshBase::MeshBase(const std::vector<Vertex>& vertices) {
    this->vertices = vertices;
    UploadToGPU();
}

MeshBase::MeshBase(const std::vector<Vertex>& vertices, const std::vector<GLuint>& elements)
    : vertices{vertices}, elements{elements}, usesElementArray{true} {
    UploadToGPU();
}

MeshBase::~MeshBase() {
    glDeleteBuffers(1, &buffers.vbo);
    glDeleteBuffers(1, &buffers.ebo);
}