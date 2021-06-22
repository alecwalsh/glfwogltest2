#pragma once

#include <cstdint>
#include <vector>

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