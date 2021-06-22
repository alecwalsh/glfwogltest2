#pragma once

#include "MeshData.hpp"

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

class MeshBase {
  public:
    MeshData meshData;

    struct {
        std::uint32_t vbo;
        std::uint32_t ebo;
    } buffers = {};

    // TODO: copy/move constructors
    MeshBase() = default;
    [[nodiscard]] MeshBase(const std::vector<MeshData::Vertex>& vertices);
    [[nodiscard]] MeshBase(const std::vector<MeshData::Vertex>& vertices, const std::vector<std::uint32_t>& elements);
    virtual ~MeshBase();

    // Generates buffers and uploads data to graphics card
    void UploadToGPU();
};