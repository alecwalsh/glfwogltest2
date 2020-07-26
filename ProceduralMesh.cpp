#include "ProceduralMesh.h"

std::vector<ProceduralMesh::Vertex> GenerateVertices() { 
    std::vector<ProceduralMesh::Vertex> vertices;

    glm::vec3 normal {1,0,0};

    vertices.push_back({
        {0.5, 0.5, 0.5},
        normal,
        {0, 0}
        });
    vertices.push_back({
        {0.5, -0.5, 0.5},
        normal,
        {0, 1}
    });
    vertices.push_back({
        {-0.5, -0.5, 0.5},
        normal,
        {1, 1}
    });
    vertices.push_back({
        {0.5, 0.5, 0.5},
        normal,
        {1, 1}
    });
    vertices.push_back({
        {-0.5, 0.5, 0.5},
        normal,
        {1, 0}
    });
    vertices.push_back({
        {-0.5, -0.5, 0.5},
        normal,
        {0, 0}
    });

    return vertices;
}

std::vector<GLuint> GenerateElements() {
    return {
        0, 1, 2,
        3, 4, 5,
    };
}

ProceduralMesh::ProceduralMesh() {
    vertices = GenerateVertices();
    //elements = GenerateElements();
    //usesElementArray = true;
    UploadToGPU();
}