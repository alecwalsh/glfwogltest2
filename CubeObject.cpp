#include "CubeObject.h"

#include <sstream>

// Runs every frame
void CubeObject::Tick() {
    glm::mat4 rotation{1.0f}, translation{1.0f}, scaling{1.0f};

    rotation = glm::rotate(rotation, deltaTime * RotSpeed * glm::radians(180.0f), glm::vec3{0.0f, 1.0f, 0.0f});
    
    
    for(auto& v : mesh.vertices) {
        for(uint32_t i = 0; i < mesh.num_bones; i++) {
            auto weight = v.weights[i];
//             printf("%f\n", weight);
            auto transformed_pos = mesh.bone_matrices[i] * glm::vec4{v.position, 1.0f};
            auto m = glm::rotate(glm::mat4{1.0f}, glm::radians(0.5f*elapsedTime*weight), glm::vec3{0.0f, 1.0f, 0.0f});
            transformed_pos = m * transformed_pos;
//             v.position = glm::inverse(mesh.bone_matrices[i]) * transformed_pos;
        }
    }
    
//     for(uint32_t i = 0; i < mesh.num_bones; i++) {
//         std::stringstream ss;
//         ss << "bone_matrices[" << i << ']';
//         std::cout << ss.str().c_str() << std::endl;
//         GLint bone_matrix_loc = glGetUniformLocation(shaderProgram.shaderProgram, ss.str().c_str());
//         assert(bone_matrix_loc != -1);
//         glUniformMatrix4fv(bone_matrix_loc, 1, GL_FALSE, glm::value_ptr(mesh.bone_matrices[i]));
//     }
    

    this->ModTransform(translation * rotation * scaling);
}

void CubeObject::Draw(const Camera& camera) {
    // Set material properties
    GLint matAmbientLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.ambient");
    GLint matDiffuseLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.diffuse");
    GLint matSpecularLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.specular");
    GLint matShineLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.shininess");

    glUniform3f(matAmbientLoc, material.ambient.r, material.ambient.g, material.ambient.b);
    glUniform3f(matDiffuseLoc, material.diffuse.r, material.diffuse.g, material.diffuse.b);
    glUniform3f(matSpecularLoc, material.specular.r, material.specular.g, material.specular.b);
    glUniform1f(matShineLoc, material.shininess);

    GameObject::Draw(camera);
}

CubeObject::CubeObject(Mesh& mesh, ShaderProgram& shaderProgram, glm::mat4 transform, float& elapsedTime,
                       float& deltaTime, TextureManager& texman)
    : GameObject(mesh, shaderProgram, transform, elapsedTime, deltaTime, texman) {
    
    for(uint32_t i = 0; i < mesh.num_bones; i++) {
        std::stringstream ss;
        ss << "bone_matrices[" << i << ']';
        std::cout << ss.str().c_str() << std::endl;
        GLint bone_matrix_loc = glGetUniformLocation(shaderProgram.shaderProgram, ss.str().c_str());
        assert(bone_matrix_loc != -1);
        glUniformMatrix4fv(bone_matrix_loc, 1, GL_FALSE, glm::value_ptr(mesh.bone_matrices[i]));
        
    }


    for(uint32_t i = 0; i < mesh.num_bones; i++) {
        std::stringstream ss;
        ss << "bone_transforms[" << i << ']';
        std::cout << ss.str().c_str() << std::endl;
        GLint bone_transform_loc = glGetUniformLocation(shaderProgram.shaderProgram, ss.str().c_str());
//         assert(bone_transform_loc != -1);
        glUniformMatrix4fv(bone_transform_loc, 1, GL_FALSE, glm::value_ptr(mesh.bone_transforms[i]));
    }
    
    
    // Sets up material properties for the cube
    material.ambient = glm::vec3{1.0f, 0.5f, 0.31f};
    material.diffuse = glm::vec3{1.0f, 0.5f, 0.31f};
    material.specular = glm::vec3{1.0f, 1.0f, 1.0f};
    material.shininess = 32.0f;
}

CubeObject::~CubeObject() {}
