#pragma once

#include "MeshData.hpp"

#include <cstdint>
#include <vector>

class MeshBase {
    // Generates buffers and uploads data to graphics card
    void UploadToGPU();
  public:
    MeshData meshData;

    struct {
        std::uint32_t vbo;
        std::uint32_t ebo;
    } buffers = {};

    [[nodiscard]] MeshBase(MeshData meshData);

    [[nodiscard]] MeshBase(const MeshBase& m) = default;
    [[nodiscard]] MeshBase(MeshBase&& m) = default;

    MeshBase& operator=(const MeshBase& m) = default;
    MeshBase& operator=(MeshBase&& m) = default;

    virtual ~MeshBase();
};