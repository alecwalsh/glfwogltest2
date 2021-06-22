#include "Mesh.hpp"

#include <iostream>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

Mesh::Mesh(const std::string& fileName) {
    meshData = ImportMesh(fileName);

    UploadToGPU();
}

// TODO: .blend files normals are per vertex, not per face; .fbx works fine
MeshData Mesh::ImportMesh(const std::string& fileName) {
    std::vector<MeshData::Vertex> vertices;
    std::vector<std::uint32_t> elements;
    
    Assimp::Importer importer;
    // TODO: Change/add postprocessing flags
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    // If the import failed, report it
    if (!scene) {
        std::cout << importer.GetErrorString() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto mesh = scene->mMeshes[0];

    vertices.reserve(mesh->mNumVertices);
    elements.reserve(mesh->mNumFaces * 3); // Each face has 3 vertices

    if (!mesh->HasNormals()) {
        std::cerr << "Mesh doesn't have normals." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const auto& face = mesh->mFaces[i];
        // TODO: Add error handling / support more than just triangles
        if (face.mNumIndices != 3) {
            std::cerr << "Wrong number of vertices" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            elements.push_back(face.mIndices[j]);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        const auto& v = mesh->mVertices[i];
        const auto& n = mesh->mNormals[i];

        glm::vec2 texcoords;

        if (mesh->mTextureCoords[0]) {
            texcoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            texcoords = {0, 0};
        }

        vertices.push_back({
            {v.x / 2, v.y / 2, v.z / 2}, // Position
            {n.x, n.y, n.z},             // Normals
            texcoords                    // Texture coordinates
        });
    }

    return {vertices, elements, true};
}
