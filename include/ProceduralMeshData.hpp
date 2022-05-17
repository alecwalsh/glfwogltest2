#pragma once

#include "MeshData.hpp"

#include <array>
#include <algorithm>
#include <utility>
#include <numbers>

#ifdef MESHDATA_USE_CONSTEXPR
#include <ranges>

#define MESHDATA_VECTOR_CONSTEXPR constexpr
#else
#define MESHDATA_VECTOR_CONSTEXPR inline
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
    using std::numbers::pi;

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

// TODO: Support tiling texture
MESHDATA_VECTOR_CONSTEXPR MeshData CreatePlaneMeshData(double xSize, double ySize, std::uint32_t xSubdivisionLevel, std::uint32_t ySubdivisionLevel) noexcept {
    using glm::vec2, glm::vec3;

    xSubdivisionLevel++; // xSubdivisionLevel is now equal to the number of edges along the x axis
    ySubdivisionLevel++; // ySubdivisionLevel is now equal to the number of edges along the y axis

    std::vector<MeshData::Vertex> vertices;
    std::vector<std::uint32_t> elements;

    vertices.reserve((xSubdivisionLevel + 1) * (ySubdivisionLevel + 1));
    elements.reserve(xSubdivisionLevel * ySubdivisionLevel * 6);

    vec3 topRight{xSize / 2, ySize / 2, 0};

    vec3 normal{0, 0, 1};

    for (std::uint32_t i = 0; i <= ySubdivisionLevel; i++) {
        for (std::uint32_t j = 0; j <= xSubdivisionLevel; j++) {
            double xSubDivFraction = 1.0 / xSubdivisionLevel;
            double ySubDivFraction = 1.0 / ySubdivisionLevel;

            double xSizeSubDivFraction = xSize / xSubdivisionLevel;
            double ySizeSubDivFraction = ySize / ySubdivisionLevel;

            vertices.push_back({
                .position = topRight - vec3{xSizeSubDivFraction * j, ySizeSubDivFraction * i, 0},
                .normal = normal,
                .texcoord = vec2{1 - xSubDivFraction * j, 1 - ySubDivFraction * i}
            });
        }
    }

    for (std::uint32_t i = 0; i < ySubdivisionLevel; i++) {
        for (std::uint32_t j = 0; j < xSubdivisionLevel; j++) {
            elements.push_back((xSubdivisionLevel + 1) * i + j);
            elements.push_back((xSubdivisionLevel + 1) * i + j + 1);
            elements.push_back((xSubdivisionLevel + 1) * (i + 1) + j);

            elements.push_back((xSubdivisionLevel + 1) * (i + 1) + j);
            elements.push_back((xSubdivisionLevel + 1) * (i + 1) + j + 1);
            elements.push_back((xSubdivisionLevel + 1) * i + j + 1);
        }
    }

    return { vertices, elements, true };
}

MESHDATA_VECTOR_CONSTEXPR MeshData CreatePlaneMeshData(double xSize, double ySize, std::uint32_t subdivisionLevel) noexcept {
    return CreatePlaneMeshData(xSize, ySize, subdivisionLevel, subdivisionLevel);
}

MESHDATA_VECTOR_CONSTEXPR MeshData CreatePlaneMeshData(double xSize, double ySize) noexcept {
    return CreatePlaneMeshData(xSize, ySize, 0);
}

MESHDATA_VECTOR_CONSTEXPR MeshData CreatePlaneMeshData(double size) noexcept { return CreatePlaneMeshData(size, size); }

MESHDATA_VECTOR_CONSTEXPR MeshData CreatePlaneMeshData() noexcept { return CreatePlaneMeshData(1); }

#ifdef MESHDATA_USE_CONSTEXPR

template <std::size_t SubdivisionLevel = 0>
constexpr auto CreateCuboidMeshDataConstexpr(double xSize, double ySize, double zSize) noexcept {
    auto meshData = CreateCuboidMeshData(xSize, ySize, zSize);

    constexpr std::size_t VertexCount = 24;
    constexpr std::size_t ElementCount = 36;

    ConstexprMeshData<VertexCount, ElementCount> result = {
        .usesElementArray = meshData.usesElementArray
    };

    std::ranges::copy(meshData.vertices, result.vertices.begin());
    std::ranges::copy(meshData.elements, result.elements.begin());

    return result;
}

template <std::size_t SubdivisionLevel = 0>
constexpr auto CreateCuboidMeshDataConstexpr(double size) noexcept {
    return CreateCuboidMeshDataConstexpr<SubdivisionLevel>(size, size, size);
}

template <std::size_t SubdivisionLevel = 0>
constexpr auto CreateCuboidMeshDataConstexpr() noexcept {
    return CreateCuboidMeshDataConstexpr<SubdivisionLevel>(1);
}


template <std::uint32_t XSubdivisionLevel = 0, std::uint32_t YSubdivisionLevel = XSubdivisionLevel>
constexpr auto CreatePlaneMeshDataConstexpr(double xSize, double ySize) noexcept {
    auto meshData = CreatePlaneMeshData(xSize, ySize, XSubdivisionLevel, YSubdivisionLevel);

    constexpr std::size_t VertexCount = (XSubdivisionLevel + 2) * (YSubdivisionLevel + 2);
    constexpr std::size_t ElementCount = (XSubdivisionLevel + 1) * (YSubdivisionLevel + 1) * 6;

    ConstexprMeshData<VertexCount, ElementCount> result = {
        .usesElementArray = meshData.usesElementArray
    };

    std::ranges::copy(meshData.vertices, result.vertices.begin());
    std::ranges::copy(meshData.elements, result.elements.begin());

    return result;
}

template <std::uint32_t XSubdivisionLevel = 0, std::uint32_t YSubdivisionLevel = XSubdivisionLevel>
constexpr auto CreatePlaneMeshDataConstexpr(double size) noexcept {
    return CreatePlaneMeshDataConstexpr<XSubdivisionLevel, YSubdivisionLevel>(size, size);
}

template <std::uint32_t XSubdivisionLevel = 0, std::uint32_t YSubdivisionLevel = XSubdivisionLevel>
constexpr auto CreatePlaneMeshDataConstexpr() noexcept {
    return CreatePlaneMeshDataConstexpr<XSubdivisionLevel, YSubdivisionLevel>(1);
}

#endif
