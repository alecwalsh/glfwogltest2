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

#ifdef MESHDATA_USE_CONSTEXPR
    template <std::size_t I, std::size_t J>
    [[nodiscard]] MeshBase(ConstexprMeshData<I, J> meshData) : MeshBase{FromConstexpr(meshData)} {}
#endif

    [[nodiscard]] MeshBase(const MeshBase& m) = default;
    [[nodiscard]] MeshBase(MeshBase&& m) = default;

    MeshBase& operator=(const MeshBase& m) = default;
    MeshBase& operator=(MeshBase&& m) = default;

    virtual ~MeshBase();
};