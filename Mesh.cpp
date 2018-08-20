#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"

void print_mat4(const glm::mat4& m) {
    printf("[ \n");
    printf("%f, %f, %f, %f,\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    printf("%f, %f, %f, %f,\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    printf("%f, %f, %f, %f,\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    printf("%f, %f, %f, %f\n", m[3][0], m[3][1], m[3][2], m[3][3]);
    printf("] \n");
}

void Mesh::print_bone_transforms() {
    for(uint32_t j = 0; j < num_bones; j++) {
        auto transformed_pos = bone_matrices[j] * glm::vec4{0.0f, 0.0f, 1.0f, 1.0f} * root_transform;
        transformed_pos *= 2;
        printf("%f, %f, %f in bone space for bone %d\n", transformed_pos.x, transformed_pos.y, transformed_pos.z, j);
    }
    for(unsigned int i = 0; i < vertices.size(); i++) {
        auto pos = glm::vec4{vertices[i].position, 1.0f};
//         printf("Vert %d: %f, %f, %f\n", i, pos.x, pos.y, pos.z);
        for(uint32_t j = 0; j < 3; j++) {
            auto transformed_pos = bone_matrices[j] * pos;
//             printf("Vert %d: %f, %f, %f in bone space for bone %d\n", i, transformed_pos.x, transformed_pos.y, transformed_pos.z, j);
        }
//         printf("\n");
    }
    
}

glm::mat4 assimp_to_glm(const aiMatrix4x4& m) {
    glm::mat4 glm_mat{
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3]
    };
    return glm::transpose(glm_mat);
// //     return glm_mat;
}

Mesh::Mesh(const std::vector<Vertex>& vertices) : vertices(vertices), usesElementArray(false) {
    UploadToGPU();
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& elements) : vertices(vertices), elements(elements), usesElementArray(true) {
    UploadToGPU();
}

Mesh::Mesh(const std::string& fileName) : usesElementArray(true) {
    ImportMesh(fileName);
    UploadToGPU();
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &buffers.vbo);
    glDeleteBuffers(1, &buffers.ebo);
}

// Generates buffers and uploads data to graphics card
void Mesh::UploadToGPU() {
    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    GLuint ebo;

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    buffers.vbo = vbo;
    buffers.ebo = ebo;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * (vertices.size()), vertices.data(), GL_STATIC_DRAW);

    if (usesElementArray) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements[0]) * (elements.size()), elements.data(), GL_STATIC_DRAW);
    }
}

void Mesh::ImportMesh(const std::string& fileName) {
    // TODO: Change/add postprocessing flags
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    
    auto root_node = scene->mRootNode;
    root_transform = assimp_to_glm(root_node->mTransformation.Inverse());
    printf("%s has %d children\n", root_node->mName.C_Str(), root_node->mNumChildren);
    
    if(root_node->mChildren[0]->mNumChildren > 0) {
        const auto& node = root_node->mChildren[0];
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            const auto& node2 = node->mChildren[i];
            printf("Node %s has %d children\n", node2->mName.C_Str(), node2->mNumChildren);
        }
    }

    // If the import failed, report it
    if (!scene) {
        std::cerr << importer.GetErrorString() << std::endl;
        exit(EXIT_FAILURE);
    }

    const auto& mesh = scene->mMeshes[0];

    if (!mesh->HasNormals()) {
        std::cerr << "Mesh doesn't have normals." << std::endl;
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        // TODO: Add error handling / support more than just triangles
        if (face.mNumIndices != 3) {
            std::cerr << "Wrong number of vertices" << std::endl;
            exit(1);
        }

        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            elements.push_back(face.mIndices[j]);
        }
    }

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        auto v = mesh->mVertices[i];
        auto n = mesh->mNormals[i];

        glm::vec2 texcoords{0, 0};

        if (mesh->mTextureCoords[0]) {
            texcoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        }

        //TODO: Don't divide by 2
        vertices.push_back({
            {v.x / 2, v.y / 2, v.z / 2}, // Position
            {n.x, n.y, n.z},             // Normals
            texcoords,                    // Texture coordinates,
            {1.0f, 1.0f, 1.0f}
        });
    }
    
    std::cout << "vertices.size() = " << vertices.size() << std::endl;
    std::cout << "elements.size() = " << elements.size() << std::endl;
    
    GetBoneWeights();
}

void Mesh::GetBoneWeights() {
    const auto& mesh = importer.GetScene()->mMeshes[0];
    
    if(!mesh->HasBones()) {
        num_bones = 0;
        return;
    }
    
    num_bones = mesh->mNumBones;
    assert(num_bones <= MAX_BONES);
    auto bones = mesh->mBones;
    printf("%d bones\n", num_bones);
    for(uint32_t i = 0; i < num_bones; i++) {
        auto bone = bones[i];
        bone_names.insert({bone->mName.C_Str(), i});
        
        bone_matrices[i] = assimp_to_glm(bone->mOffsetMatrix);
        for(uint32_t j = 0; j < bone->mNumWeights; j++) {
            auto weight = bone->mWeights[j];
            vertices[weight.mVertexId].weights[i] = weight.mWeight;
        }
        
//         auto node = importer.GetScene()->mRootNode->FindNode(bone->mName);
//         const aiNode* tempNode = node;
//         while(tempNode) {
// //             aiMatrix4x4 m = tempNode->mTransformation;
//             bone_matrices[i] = assimp_to_glm(tempNode->mTransformation) * bone_matrices[i];
//             tempNode = tempNode->mParent;
//         }
//         bone_matrices[i] = assimp_to_glm(importer.GetScene()->mRootNode->mTransformation) * bone_matrices[i];
    }
    
    print_bone_transforms();
    
    for(auto& v : vertices) {
        for(uint32_t i = 0; i < 1; i++) {
            auto weight = v.weights[i];
//             printf("%f\n", weight);
            auto transformed_pos = bone_matrices[i] * glm::vec4{v.position, 1.0f};
            auto m = glm::rotate(glm::mat4{1.0f}, glm::radians(45.0f*weight), glm::vec3{0.0f, 1.0f, 0.0f});
            transformed_pos = m * transformed_pos;
//             v.position = glm::inverse(bone_matrices[i]) * transformed_pos;
        }
    }
}
