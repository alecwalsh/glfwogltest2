#pragma once

#include "MeshData.hpp"

#include <array>
#include <utility>

#if __cpp_lib_constexpr_vector >= 201907L
#define MESHDATA_VECTOR_CONSTEXPR constexpr
#else
#define MESHDATA_VECTOR_CONSTEXPR inline
#endif

#ifdef __cpp_lib_math_constants
#include <numbers>
using std::numbers::pi;
#else
constexpr double pi = 3.14159265358979323846;
#endif


// Assumes elements are either clockwise or counterclockwise
constexpr std::array<std::uint32_t, 6> QuadToTrisElements() noexcept { return {0, 1, 2, 2, 3, 0}; }

// Converts a quad to two triangles. Avoid in favor of using an element array
constexpr std::array<glm::vec3, 6> QuadToTris(std::array<glm::vec3, 4> vertices) noexcept {
    std::array<glm::vec3, 6> result = {};

    const auto elements = QuadToTrisElements();

    for (std::size_t i = 0; i < result.size(); i++) {
        result[i] = vertices[elements[i]];
    }

    return result;
}

// This can't be constexpr because trig functions aren't constexpr
// Theta is polar angle, measured clockwise from {0, 1, 0}
// Phi is azimuthal angle, measured counterclockwise from {0, 0, 1}
inline glm::vec3 SphericalToCartesian(double r, double theta, double phi) noexcept {
    return static_cast<float>(r) * glm::vec3{sin(theta) * sin(phi), cos(theta), sin(theta) * cos(phi)};
}

inline MeshData CreateSphereMeshData(double radius) noexcept { // TODO: Generate UV coordinates
    using glm::vec3;

    std::vector<MeshData::Vertex> vertices;
    std::vector<std::uint32_t> elements;

    // Slices must be at least 4
    int slices = 20;

    int latSlices = slices / 2;
    int longSlices = slices;

    unsigned int totalVertices = longSlices * 6                      // Cap vertices
                                 + (latSlices - 2) * longSlices * 4; // Vertices in the rest of the sphere

    unsigned int totalElements = longSlices * 6                      // Cap elements
                                 + (latSlices - 2) * longSlices * 6; // Elements for the rest of the sphere

    vertices.reserve(totalVertices);
    elements.reserve(totalElements);

    float xcoord_offset = 0.002f; // TODO: figure out why values of 0 and 1 cause wrong colors

    std::uint32_t capIndex = 0;

    for (int j = 0; j < longSlices; j++) {   // Create top and bottom cap
        double theta = pi / latSlices;       // Angle for top of second slice
        double theta2 = pi - pi / latSlices; // Angle for bottom of second to last slice

        double phi = j * (2 * pi / longSlices); // Angle from {0, 0, 1}
        double phi2 = phi + 2 * pi / longSlices;

        std::array capVertices = {
            vec3{0, radius, 0},  SphericalToCartesian(radius, theta, phi2),  SphericalToCartesian(radius, theta, phi),

            vec3{0, -radius, 0}, SphericalToCartesian(radius, theta2, phi2), SphericalToCartesian(radius, theta2, phi),
        };

        auto xcoord = static_cast<float>(j) / longSlices;

        for (const vec3& v : capVertices) {
            vertices.push_back({v, v, {xcoord + xcoord_offset, 0}});
            elements.push_back(capIndex++);
        }
    }

    // capIndex is now the index of the first non-cap vertex
    std::uint32_t faceNumber = 0;

    for (int i = 1; i < latSlices - 1; i++) {
        double theta = i * pi / latSlices; // Angle from top ({0, 1, 0})
        double theta2 = theta + pi / latSlices;

        for (int j = 0; j < longSlices; j++) {
            double phi = j * (2 * pi / longSlices); // Angle from {0, 0, 1}
            double phi2 = phi + 2 * pi / longSlices;

            std::array faceVertices = {
                SphericalToCartesian(radius, theta, phi),
                SphericalToCartesian(radius, theta, phi2),
                SphericalToCartesian(radius, theta2, phi2),
                SphericalToCartesian(radius, theta2, phi),
            };

            auto xcoord = static_cast<float>(j) / longSlices;

            for (const vec3& v : faceVertices) {
                vertices.push_back({v, v, {xcoord + xcoord_offset, 0}});
            }

            for (const auto& e : QuadToTrisElements()) {
                // 4 * faceNumber vertices for faceNumber faces
                // capIndex + 4 * faceNumber is the index of the first vertex of the current quad
                // e is the offset into the current quad needed to get the current vertex
                elements.push_back(capIndex + 4 * faceNumber + e);
            }
            faceNumber++;
        }
    }

    return {vertices, elements, true};
}

inline MeshData CreateSphereMeshData() noexcept { return CreateSphereMeshData(0.5); }

// TODO: Support subdivision, UV coordinates
MESHDATA_VECTOR_CONSTEXPR MeshData CreateCuboidMeshData(double xSize, double ySize, double zSize) noexcept {
    using glm::vec3;

    std::vector<MeshData::Vertex> vertices;
    std::vector<std::uint32_t> elements;

    vertices.reserve(24);
    elements.reserve(36);

    vec3 topRightFront{xSize / 2, ySize / 2, zSize / 2};

    vec3 bottomLeftBack = topRightFront - vec3{xSize, ySize, zSize};

    std::array topFace = {
        topRightFront,
        topRightFront - vec3{xSize, 0, 0},
        topRightFront - vec3{xSize, 0, zSize},
        topRightFront - vec3{0, 0, zSize},
    };

    std::array bottomFace = {
        bottomLeftBack,
        bottomLeftBack + vec3{xSize, 0, 0},
        bottomLeftBack + vec3{xSize, 0, zSize},
        bottomLeftBack + vec3{0, 0, zSize},
    };

    std::array frontFace = {
        topRightFront,
        topRightFront - vec3{xSize, 0, 0},
        topRightFront - vec3{xSize, ySize, 0},
        topRightFront - vec3{0, ySize, 0},
    };

    std::array backFace = {
        bottomLeftBack,
        bottomLeftBack + vec3{xSize, 0, 0},
        bottomLeftBack + vec3{xSize, ySize, 0},
        bottomLeftBack + vec3{0, ySize, 0},
    };

    std::array rightFace = {
        topRightFront,
        topRightFront - vec3{0, 0, zSize},
        topRightFront - vec3{0, ySize, zSize},
        topRightFront - vec3{0, ySize, 0},
    };

    std::array leftFace = {
        bottomLeftBack,
        bottomLeftBack + vec3{0, 0, zSize},
        bottomLeftBack + vec3{0, ySize, zSize},
        bottomLeftBack + vec3{0, ySize, 0},
    };

    struct FaceType {
        std::array<vec3, 4> vertices;
        vec3 normal;
    };

    std::vector<FaceType> faces = {
        {topFace, {0, 1, 0}},   {bottomFace, {0, -1, 0}}, {frontFace, {0, 0, 1}},
        {backFace, {0, 0, -1}}, {rightFace, {1, 0, 0}},   {leftFace, {-1, 0, 0}},
    };

    std::uint32_t index = 0;

    for (const auto& [face, normal] : faces) {
        for (const auto& v : face) {
            vertices.push_back({v, normal});
        }
        for (const auto& e : QuadToTrisElements()) {
            elements.push_back(e + 4 * index);
        }
        index++;
    }

    return {vertices, elements, true};
}

MESHDATA_VECTOR_CONSTEXPR MeshData CreateCuboidMeshData(double size) noexcept {
    return CreateCuboidMeshData(size, size, size);
}

MESHDATA_VECTOR_CONSTEXPR MeshData CreateCuboidMeshData() noexcept { return CreateCuboidMeshData(1); }

// TODO: Support subdivision, UV coordinates
MESHDATA_VECTOR_CONSTEXPR MeshData CreatePlaneMeshData(double xSize, double ySize) noexcept {
    using glm::vec3;

    std::vector<MeshData::Vertex> vertices;

    const auto elementsArray = QuadToTrisElements();
    std::vector<std::uint32_t> elements{elementsArray.begin(), elementsArray.end()};

    vertices.reserve(4);

    vec3 topRight{xSize / 2, ySize / 2, 0};

    vec3 normal{0, 0, 1};

    std::array verticesArray = {
        topRight,
        topRight - vec3{xSize, 0, 0},
        topRight - vec3{xSize, ySize, 0},
        topRight - vec3{0, ySize, 0},
    };

    using glm::vec2;

    std::array texCoords = {
        vec2{1, 1},
        vec2{0, 1},
        vec2{0, 0},
        vec2{1, 0},
    };

    for (int i = 0; i < 4; i++) {
#if __cpp_designated_initializers >= 201707L
        vertices.push_back({.position = verticesArray[i], .normal = normal, .texcoord = texCoords[i]});
#else
        vertices.push_back({verticesArray[i], normal, texCoords[i]});
#endif
    }

    return {vertices, elements, true};
}

MESHDATA_VECTOR_CONSTEXPR MeshData CreatePlaneMeshData(double size) noexcept { return CreatePlaneMeshData(size, size); }

MESHDATA_VECTOR_CONSTEXPR MeshData CreatePlaneMeshData() noexcept { return CreatePlaneMeshData(1); }