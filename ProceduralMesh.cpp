#include "ProceduralMesh.h"

#include <array>
#include <utility>
#include <optional>

#include <cstdint>

using vec3 = glm::vec3;

//TODO: Doesn't work right if the vertices aren't in order
std::array<vec3, 6> QuadToTris(std::array<vec3, 4> vertices) {
    return {
        vertices[0], 
        vertices[1],
        vertices[2],

        vertices[2],
        vertices[3],
        vertices[0],
    };
}

vec3 SphericalToCartesian(double r, double theta, double phi) {
    return static_cast<vec3::value_type>(r) * vec3{sin(theta) * sin(phi), cos(theta), sin(theta) * cos(phi)};
}

std::vector<MeshBase::Vertex> GenerateSphereVertices() {
    std::vector<MeshBase::Vertex> vertices;

    int slices = 20;

    double radius = 1;

    double M_PI = 3.14159265358979323846;

    double theta = 1 / M_PI;
    double phi = 2 / (2 * M_PI);

    for (int i = 1; i < slices; i++) { //TODO: handle top and bottom slices separately
        double theta = i * 2 * M_PI / slices; //Angle from top
        double theta2 = theta + 2 * M_PI / slices;

        for (int j = 0; j < slices; j++) {
            double phi = j * 2 * M_PI / slices; // Angle from top
            double phi2 = phi + 2 * M_PI / slices;

            std::array faceVertices = {
                SphericalToCartesian(radius, theta, phi),
                SphericalToCartesian(radius, theta, phi2),
                SphericalToCartesian(radius, theta2, phi2),
                SphericalToCartesian(radius, theta2, phi),
            };

            for(const vec3& v : QuadToTris(faceVertices)) {
                vertices.push_back({
                    v,
                    v,
                });
            }
        }
    }

    return vertices;
}

//TODO: Support cuboids
std::vector<MeshBase::Vertex> GenerateCubeVertices() {
    std::vector<MeshBase::Vertex> vertices;

    float size = 1.0f;

    vec3 topRightFront{1.0f, 1.0f, 1.0f};

    vec3 bottomLeftBack = topRightFront - vec3{size};

    std::array topFace = {
        topRightFront,
        topRightFront - vec3{size, 0, 0},
        topRightFront - vec3{size, 0, size},
        topRightFront - vec3{0, 0, size},
    };

    std::array bottomFace = {
        bottomLeftBack,
        bottomLeftBack + vec3{size, 0, 0},
        bottomLeftBack + vec3{size, 0, size},
        bottomLeftBack + vec3{0, 0, size},
    };

    std::array frontFace = {
        topRightFront - vec3{0, 0, 0},
        topRightFront - vec3{size, 0, 0},
        topRightFront - vec3{size, size, 0},
        topRightFront - vec3{0, size, 0},
    };

    std::array backFace = {
        bottomLeftBack + vec3{0, 0, 0},
        bottomLeftBack + vec3{size, 0, 0},
        bottomLeftBack + vec3{size, size, 0},
        bottomLeftBack + vec3{0, size, 0},
    };

    std::array rightFace = {
        topRightFront,
        topRightFront - vec3{0, 0, size},
        topRightFront - vec3{0, size, size},
        topRightFront - vec3{0, size, 0},
    };

    std::array leftFace = {
        bottomLeftBack,
        bottomLeftBack + vec3{0, 0, size},
        bottomLeftBack + vec3{0, size, size},
        bottomLeftBack + vec3{0, size, 0},
    };

    using T = std::pair<std::array<vec3, 4>, vec3>;
    std::vector<T> faces = {
        {topFace, vec3{0, 1, 0}},
        {bottomFace, vec3{0, -1, 0}},
        {frontFace, vec3{0, 0, 1}},
        {backFace, vec3{0, 0, -1}},
        {rightFace, vec3{1, 0, 0}},
        {leftFace, vec3{-1, 0, 0}}
    };

    for (const auto& [face, normal] : faces) {
        for (const vec3& v : QuadToTris(face)) {
            vertices.push_back({v, normal});
        }
    }

    return vertices;
}

ProceduralMesh::ProceduralMesh() {
    // vertices = GenerateSphereVertices();
    vertices = GenerateCubeVertices();

    UploadToGPU();
}