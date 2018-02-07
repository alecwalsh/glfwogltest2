#pragma once
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#include <vector>

#define VERTEX_SIZE 8

class Mesh {
  public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    std::vector<Vertex> vertices;
    std::vector<GLuint> elements;

    const bool usesElementArray; // Set to true or false depending on which constructor is called

    struct buffers {
        GLuint vbo;
        GLuint ebo;
    } buffers;

    // TODO: copy/move constructors
    Mesh(std::vector<Vertex> vertices);
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> elements);
    Mesh(std::string fileName); // Reads vertex data from text file
    ~Mesh();

    // Generates buffers and uploads data to graphics card
    void UploadToGPU();

  private:
    void ImportMesh(const std::string& pFile);
};
