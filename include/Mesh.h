#pragma once
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#include <vector>
#include <array>
#include <utility>
#include <unordered_map>

#define MAX_BONES 10

class Mesh {
  public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
        std::array<float, MAX_BONES> weights;
    };
    
    struct Bone {
        
    };

    std::vector<Vertex> vertices;
    std::vector<GLuint> elements;
    
    std::array<glm::mat4, MAX_BONES> bone_matrices;
    std::unordered_map<std::string, uint32_t> bone_names;
    
    uint32_t num_bones;

    const bool usesElementArray; // Set to true or false depending on which constructor is called

    struct buffers {
        GLuint vbo;
        GLuint ebo;
    } buffers;

    // TODO: copy/move constructors
    Mesh(const std::vector<Vertex>& vertices);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& elements);
    Mesh(const std::string& fileName); // Reads vertex data from text file
    ~Mesh();

    // Generates buffers and uploads data to graphics card
    void UploadToGPU();
    
    void print_bone_transforms();

  private:
    Assimp::Importer importer;
    glm::mat4 root_transform;
    
    void ImportMesh(const std::string& fileName);
    void GetBoneWeights();
};

constexpr size_t VERTEX_SIZE  = sizeof(Mesh::Vertex)/sizeof(float);
