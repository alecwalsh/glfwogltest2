#pragma once
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <vector>

class MeshBase {
  public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    std::vector<Vertex> vertices;
    std::vector<GLuint> elements;

    bool usesElementArray = false; // Set to true or false depending on which constructor is called

    struct {
        GLuint vbo;
        GLuint ebo;
    } buffers = {};

    // TODO: copy/move constructors
    MeshBase() = default;
    [[nodiscard]] MeshBase(const std::vector<Vertex>& vertices);
    [[nodiscard]] MeshBase(const std::vector<Vertex>& vertices, const std::vector<GLuint>& elements);
    virtual ~MeshBase();

    // Generates buffers and uploads data to graphics card
    void UploadToGPU();
};

constexpr std::size_t VERTEX_SIZE = sizeof(MeshBase::Vertex) / sizeof(float);