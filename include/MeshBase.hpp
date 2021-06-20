#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

// TODO: Create constexpr MeshData class
class MeshBase {
  public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> elements;

    bool usesElementArray = false; // Set to true or false depending on which constructor is called

    struct {
        std::uint32_t vbo;
        std::uint32_t ebo;
    } buffers = {};

    // TODO: copy/move constructors
    MeshBase() = default;
    [[nodiscard]] MeshBase(const std::vector<Vertex>& vertices);
    [[nodiscard]] MeshBase(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& elements);
    virtual ~MeshBase();

    // Generates buffers and uploads data to graphics card
    void UploadToGPU();
};

constexpr std::size_t VERTEX_SIZE = sizeof(MeshBase::Vertex) / sizeof(float);