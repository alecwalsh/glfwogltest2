#pragma once

#include "GameObject.h"

#include "Camera.h"
#include "MeshBase.h"
#include "ShaderManager.h"
#include "TextureManager.h"

class RenderableObject : public GameObject {
  protected:
    MeshBase& mesh;

    struct {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        GLfloat shininess;
    } material = {};

    GLuint vao;
    TextureManager& texman;

  public:
    using GameObject::GameObject;

    std::string texture_name = "container";
    std::string spec_texture_name = "container_specular";

    RenderableObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman);
    RenderableObject(const RenderableObject&);

    // Renders the object
    virtual void Draw(const Camera& camera) const;

    void SetupTextures() const;
    void BindTextures() const;

    ShaderProgram& shaderProgram;
};