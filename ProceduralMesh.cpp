#include "ProceduralMesh.h"

#include <array>
#include <utility>
#include <optional>

#include <cstdint>

#ifdef __cpp_lib_math_constants
#include <numbers>
using std::numbers::pi;
#else
constexpr double pi = 3.14159265358979323846;
#endif

using vec3 = glm::vec3;

// TODO: Doesn't work right if the vertices aren't in order
constexpr std::array<GLuint, 6> QuadToTrisElements(std::array<vec3, 4> vertices) {
    return {0, 1, 2, 2, 3, 0};
}

//TODO: Doesn't work right if the vertices aren't in order
std::array<vec3, 6> QuadToTris(std::array<vec3, 4> vertices) {
    std::array<vec3, 6> result;

    auto elements = QuadToTrisElements(vertices);

    for (std::size_t i = 0; i < result.size(); i++) {
        result[i] = vertices[elements[i]];
    }

    return result;
}

// Theta is polar angle, measured clockwise from {0, 1, 0}
// Phi is azimuthal angle, measured counterclockwise from {0, 0, 1}
vec3 SphericalToCartesian(double r, double theta, double phi) {
    return static_cast<vec3::value_type>(r) * vec3{sin(theta) * sin(phi), cos(theta), sin(theta) * cos(phi)};
}

std::pair<std::vector<MeshBase::Vertex>, std::vector<GLuint>>
GenerateUVSphereVertices() { // TODO: Generate UV coordinates
    std::vector<MeshBase::Vertex> vertices;
    std::vector<GLuint> elements;

    double radius = 1;

    //Slices must be at least 4
    int slices = 20;

    int latSlices = slices/2;
    int longSlices = slices;


    GLuint capIndex = 0;

    for (int j = 0; j < longSlices; j++) {// Create top and bottom cap
        double theta = pi / latSlices;// Angle for top of second slice
        double theta2 = pi - pi / latSlices; // Angle for bottom of second to last slice

        double phi = j * (2 * pi / longSlices); // Angle from {0, 0, 1}
        double phi2 = phi + 2 * pi / longSlices;

        std::array capVertices = {
            vec3{0, 1, 0},
            SphericalToCartesian(radius, theta, phi2),
            SphericalToCartesian(radius, theta, phi),

            vec3{0, -1, 0},
            SphericalToCartesian(radius, theta2, phi2),
            SphericalToCartesian(radius, theta2, phi),
        };

        for (const vec3& v : capVertices) {
            vertices.push_back({
                v,
                v
            });
            elements.push_back(capIndex++);
        }
    }

    GLuint index = 0;

    for (int i = 1; i < latSlices-1; i++) {
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

            for(const vec3& v : faceVertices) {
                vertices.push_back({
                    v,
                    v
                });
            }
            for(const auto& e : QuadToTrisElements(faceVertices)) {
                elements.push_back(capIndex + e + 4 * index);
            }
            index++;
        }
    }

    return {vertices, elements};
}

// TODO: Support cuboids, subdivision, UV coordinates
std::pair<std::vector<MeshBase::Vertex>, std::vector<GLuint>> GenerateCubeVertices() {
    std::vector<MeshBase::Vertex> vertices;
    std::vector<GLuint> elements;

    vertices.reserve(36);
    elements.reserve(24);

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
        topRightFront,
        topRightFront - vec3{size, 0, 0},
        topRightFront - vec3{size, size, 0},
        topRightFront - vec3{0, size, 0},
    };

    std::array backFace = {
        bottomLeftBack,
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
        {leftFace, vec3{-1, 0, 0}},
    };

    GLuint index = 0;

    for (const auto& [face, normal] : faces) {
        for (const auto& v : face) {
            vertices.push_back({
                v,
                normal
            });
        }
        for (const auto& e : QuadToTrisElements(face)) {
            elements.push_back(e + 4*index);
        }
        index++;
    }

    return {vertices, elements};
}

ProceduralMesh::ProceduralMesh() {

    auto sphere = GenerateUVSphereVertices();
    vertices = sphere.first;
    elements = sphere.second;
    usesElementArray = true;

    //auto cube = GenerateCubeVertices();
    //vertices = cube.first;
    //elements = cube.second;
    //usesElementArray = true;

    UploadToGPU();
}
