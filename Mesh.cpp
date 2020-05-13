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

void Mesh::print_bone_transforms() const {
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
//     return glm_mat;
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
    scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    
	// If the import failed, report it
    if (!scene) {
        std::cerr << importer.GetErrorString() << std::endl;
        exit(EXIT_FAILURE);
    }

    auto root_node = scene->mRootNode;
    root_transform = assimp_to_glm(root_node->mTransformation.Inverse());

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
            texcoords,                   // Texture coordinates,
            {0.0f, 0.0f, 0.0f}
        });
    }
    
    std::cout << "vertices.size() = " << vertices.size() << std::endl;
    std::cout << "elements.size() = " << elements.size() << std::endl;
    
    GetBoneWeights();
}

void Mesh::ReadNodes(aiNode* node, glm::mat4 parent_transform, float time) {
    const char* name = node->mName.C_Str();

    glm::mat4 transform{1.0f};


    auto keyFrame = static_cast<int>(time * 25) % 250;
    //printf("%d\n", keyFrame);


    if (bone_names.find(name) != bone_names.end()) {
        uint32_t idx = bone_names[name];

        //printf("found bone with index %u\n", idx);

        aiBone* bone = bones[idx];

        //printf("Bone name: %s\n", bone->mName.C_Str());
        //print_mat4(assimp_to_glm(bone->mOffsetMatrix));

        //auto res = assimp_to_glm(bone->mOffsetMatrix) * glm::vec4{1.0f, 2.0f, 3, 4};
        //printf("%f %f %f %f\n", res.x, res.y, res.z, res.w);

        //print_mat4(glm::translate(glm::mat4{1.0f}, glm::vec3{1.0f, 2.0f, 3.0f}));

        auto v = key_frames[keyFrame].pos;
        auto posVec = glm::vec3{v.x, v.y, v.z};

        printf("%f %f %f\n", v.x, v.y, v.z);

        //transform = glm::translate(transform, posVec);
        
        //transform = parent_transform * transform;

        bone_transforms[idx] = transform;
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) {

        ReadNodes(node->mChildren[i], transform, time);
    }
}

void Mesh::UploadFrameTransforms(float time) { ReadNodes(scene->mRootNode, glm::mat4{1.0f}, time); }

void Mesh::GetBoneWeights() {
    const auto& mesh = importer.GetScene()->mMeshes[0];
    
    if(!mesh->HasBones()) {
        num_bones = 0;
        return;
    }
    
    num_bones = mesh->mNumBones;
    assert(num_bones <= MAX_BONES);
    bones = mesh->mBones;

    printf("%d bones\n", num_bones);

    for(uint32_t i = 0; i < num_bones; i++) {
        auto* bone = bones[i];
        bone_names.insert({bone->mName.C_Str(), i});
        
        bone_matrices[i] = assimp_to_glm(bone->mOffsetMatrix);
        for(uint32_t j = 0; j < bone->mNumWeights; j++) {
            auto weight = bone->mWeights[j];
            vertices[weight.mVertexId].weights[i] = weight.mWeight;
        }
    }
    
    //print_bone_transforms();

    if (scene->HasAnimations()) {
        printf("%d animations\n", scene->mNumAnimations);
        aiAnimation* const anim = scene->mAnimations[0];

        printf("%d animation channels\n", anim->mNumChannels);

        for (int i = 0; i < anim->mNumChannels; i++) {
            aiNodeAnim* const animChannel = anim->mChannels[i];
            printf("animChannel %s\n", animChannel->mNodeName.C_Str());

            printf("mNumPositionKeys %u\n", animChannel->mNumPositionKeys);

            for (int j = 0; j < animChannel->mNumPositionKeys; j++) {

                auto posKey = animChannel->mPositionKeys[j];
                auto scaleKey = animChannel->mScalingKeys[j];
                auto rotKey = animChannel->mRotationKeys[j];

                key_frames.push_back({posKey.mTime, posKey.mValue, scaleKey.mValue, rotKey.mValue});
            }
        }
    }

    UploadFrameTransforms(0);
    
    for (auto& bt : bone_transforms) {
        //bt = glm::mat4{1.0f};
    }
    
    //bone_transforms[0] = glm::rotate(glm::mat4{1}, glm::radians(45.0f), glm::vec3{1.0f, 0.0f, 0.0f});
    //bone_transforms[1] = glm::rotate(glm::mat4{1}, glm::radians(45.0f), glm::vec3{1.0f, 0.0f, 0.0f}); // glm::mat4{1.0f};
    //bone_transforms[2] = glm::rotate(glm::mat4{1}, glm::radians(45.0f), glm::vec3{1.0f, 0.0f, 0.0f}); // glm::mat4{1.0f};
    
    for(int i = 0; i < bone_transforms.size(); i++) {
        //bone_transforms[i] = glm::inverse(bone_matrices[i]) * bone_transforms[i] * bone_matrices[i];
    }
}
