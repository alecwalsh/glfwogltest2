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
    return (float)r * vec3{sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)};
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

ProceduralMesh::ProceduralMesh() {
    vertices = GenerateSphereVertices();

    UploadToGPU();
}