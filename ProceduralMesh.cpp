#include "ProceduralMesh.h"

#include <array>
#include <utility>


#include <cstdint>

#ifdef __cpp_lib_math_constants
#include <numbers>
using std::numbers::pi;
#else
constexpr double pi = 3.14159265358979323846;
#endif

#if __cpp_lib_constexpr_vector >= 201907L
    #define VECTOR_CONSTEXPR constexpr
#else
    #define VECTOR_CONSTEXPR
#endif

namespace {

using vec3 = glm::vec3;

// Assumes elements are either clockwise or counterclockwise
constexpr std::array<GLuint, 6> QuadToTrisElements() {
    return {0, 1, 2, 2, 3, 0};
}

// Converts a quad to two triangles. Avoid in favor of using an element array
constexpr std::array<vec3, 6> QuadToTris(std::array<vec3, 4> vertices) {
    std::array<vec3, 6> result = {};

    const auto elements = QuadToTrisElements();

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
GenerateUVSphereVertices(double radius) { // TODO: Generate UV coordinates
    std::vector<MeshBase::Vertex> vertices;
    std::vector<GLuint> elements;

    //Slices must be at least 4
    int slices = 20;

    int latSlices = slices/2;
    int longSlices = slices;

    float xcoord_offset = 0.002f; // TODO: figure out why values of 0 and 1 cause wrong colors

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

        auto xcoord = static_cast<float>(j) / longSlices;

        for (const vec3& v : capVertices) {
            vertices.push_back({
                v,
                v,
                {xcoord + xcoord_offset,0}
            });
            elements.push_back(capIndex++);
        }
    }

    // capIndex is now the index of the first non-cap vertex
    GLuint faceNumber = 0;

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

            auto xcoord = static_cast<float>(j) / longSlices;

            for(const vec3& v : faceVertices) {
                vertices.push_back({
                    v,
                    v,
                    {xcoord + xcoord_offset,0}
                });
            }

            for(const auto& e : QuadToTrisElements()) {
                // 4 * faceNumber vertices for faceNumber faces
                // capIndex + 4 * faceNumber is the index of the first vertex of the current quad
                // e is the offset into the current quad needed to get the current vertex
                elements.push_back(capIndex + 4 * faceNumber + e);
            }
            faceNumber++;
        }
    }

    return {vertices, elements};
}

// TODO: Support subdivision, UV coordinates
VECTOR_CONSTEXPR std::pair<std::vector<MeshBase::Vertex>, std::vector<GLuint>>
GenerateCubeVertices(double xSize, double ySize, double zSize) {
    std::vector<MeshBase::Vertex> vertices;
    std::vector<GLuint> elements;

    vertices.reserve(36);
    elements.reserve(24);

    vec3 topRightFront{xSize/2, ySize/2, zSize/2};

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

    using face_t = std::pair<std::array<vec3, 4>, vec3>;
    std::vector<face_t> faces = {
        {topFace, {0, 1, 0}},
        {bottomFace, {0, -1, 0}},
        {frontFace, {0, 0, 1}},
        {backFace, {0, 0, -1}},
        {rightFace, {1, 0, 0}},
        {leftFace, {-1, 0, 0}},
    };

    GLuint index = 0;

    for (const auto& [face, normal] : faces) {
        for (const auto& v : face) {
            vertices.push_back({
                v,
                normal
            });
        }
        for (const auto& e : QuadToTrisElements()) {
            elements.push_back(e + 4 * index);
        }
        index++;
    }

    return {vertices, elements};
}

// TODO: Support subdivision, UV coordinates
VECTOR_CONSTEXPR std::pair<std::vector<MeshBase::Vertex>, std::vector<GLuint>>
GeneratePlaneVertices(double xSize, double ySize) {
    std::vector<MeshBase::Vertex> vertices;
    
    const auto elementsArray = QuadToTrisElements();
    std::vector<GLuint> elements{elementsArray.begin(), elementsArray.end()};

    vertices.reserve(4);
    
    vec3 topRight{xSize/2, ySize/2, 0};

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
        vertices.push_back({
                .position = verticesArray[i], 
                .normal = normal,
                .texcoord = texCoords[i]
            });
#else
        vertices.push_back({
                verticesArray[i],
                normal,
                texCoords[i]
            });
#endif
    }

   return {vertices, elements};
}

} // namespace

SphereMesh::SphereMesh(double radius) {
    auto mesh = GenerateUVSphereVertices(radius);

    vertices = mesh.first;
    elements = mesh.second;
    usesElementArray = true;

    UploadToGPU();
}

SphereMesh::SphereMesh() : SphereMesh{1} {}

CuboidMesh::CuboidMesh(double x, double y, double z) {
    auto mesh = GenerateCubeVertices(x, y, z);

    vertices = mesh.first;
    elements = mesh.second;
    usesElementArray = true;

    UploadToGPU();
}

CuboidMesh::CuboidMesh(double size) : CuboidMesh{size, size, size} {}

CuboidMesh::CuboidMesh() : CuboidMesh{1} {}

PlaneMesh::PlaneMesh(double x, double y) {
    auto mesh = GeneratePlaneVertices(x, y);

    auto elements_array = QuadToTrisElements();

    vertices = mesh.first;
    elements = mesh.second;
    usesElementArray = true;

    UploadToGPU();
}

PlaneMesh::PlaneMesh(double size) : PlaneMesh{size, size} {}

PlaneMesh::PlaneMesh() : PlaneMesh{1} {}