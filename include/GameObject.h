#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Camera.h"
#include "MeshBase.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "TextureManager.h"

// TODO: Move rendering code from GameObject into RenderableObject subclass;  Make Camera a subclass of GameObject
class GameObject {
  protected:
    MeshBase& mesh;
    glm::mat4 transform{1.0f};

    struct {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        GLfloat shininess;
    } material;

    GLuint vao;
    float& elapsedTime;
    float& deltaTime;
    TextureManager& texman;

  public:
    std::string name = "";
    std::string texture_name = "container";
    std::string spec_texture_name = "container_specular";
    
    GameObject(MeshBase& mesh, ShaderProgram& shaderProgram, glm::mat4 transform, float& elapsedTime, float& deltaTime,
               TextureManager& texman);
    virtual ~GameObject();
    GameObject(const GameObject& go);

    // Runs every frame
    virtual void Tick() = 0;
    // Renders the object
    virtual void Draw(const Camera& camera) const;
    void SetupTextures() const;
    void BindFramebufferTexture() const;
    void BindTextures() const;

    void SetTransform(const glm::mat4& transform);
    void ModTransform(const glm::mat4& transform);

    ShaderProgram& shaderProgram;
};
