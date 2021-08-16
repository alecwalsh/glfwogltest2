#pragma once

#include "MeshData.hpp"

#include "GLBuffer.hpp"

#include <cstddef>
#include <vector>

class MeshBase {
    // Generates buffers and uploads data to graphics card
    void UploadToGPU();
  public:
    MeshData meshData;

    struct {
        GLBuffer vbo;
        GLBuffer ebo;
    } buffers;

    [[nodiscard]] MeshBase(MeshData meshData);

#ifdef MESHDATA_USE_CONSTEXPR
    template <std::size_t I, std::size_t J>
    [[nodiscard]] MeshBase(ConstexprMeshData<I, J> meshData) : MeshBase{FromConstexpr(meshData)} {}
#endif

    // The default copy constructor and copy assignment operator are deleted because GLBuffer is uncopyable
    // Defaulting them instead of deleting them will make them automatically copyable if I change GLBuffer's implementation later
    [[nodiscard]] MeshBase(const MeshBase& m) = default;
    MeshBase& operator=(const MeshBase& m) = default;

    [[nodiscard]] MeshBase(MeshBase&& m) = default;
    MeshBase& operator=(MeshBase&& m) = default;

    virtual ~MeshBase() = default;
};