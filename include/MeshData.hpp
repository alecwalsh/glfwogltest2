#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>

#include <glm/glm.hpp>

struct MeshData {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> elements;

    bool usesElementArray = false; // Set to true or false depending on which constructor is called
};

constexpr std::size_t VERTEX_SIZE = sizeof(MeshData::Vertex) / sizeof(float);


#if __cpp_lib_constexpr_vector >= 201907L
#define MESHDATA_USE_CONSTEXPR

template <std::size_t VertexCount, std::size_t ElementCount>
struct ConstexprMeshData {
    using Vertex = MeshData::Vertex;

    std::array<Vertex, VertexCount> vertices;
    std::array<std::uint32_t, ElementCount> elements;

    bool usesElementArray = false; // Set to true or false depending on which constructor is called
};

template <std::size_t I, std::size_t J>
constexpr MeshData FromConstexpr(ConstexprMeshData<I, J> cmd)  {
    return {
        .vertices{cmd.vertices.begin(), cmd.vertices.end()},
        .elements{cmd.elements.begin(), cmd.elements.end()},
        .usesElementArray = cmd.usesElementArray
    };
}
#endif