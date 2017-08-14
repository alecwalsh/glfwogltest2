#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Camera.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "TextureManager.h"

// TODO: Move rendering code from GameObject into RenderableObject subclass;  Make Camera a subclass of GameObject
class GameObject {
  protected:
    Mesh &mesh;
    glm::mat4 transform;

    struct {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        GLfloat shininess;
    } material;

    GLuint vao;
    float &elapsedTime;
    float &deltaTime;
    TextureManager &texman;

  public:
    GameObject(Mesh &_mesh, ShaderProgram &_shaderProgram, glm::mat4 _transform, float &_elapsedTime, float &_deltaTime,
               TextureManager &_texman);
    virtual ~GameObject();
    GameObject(const GameObject &go);

    // Runs every frame
    virtual void Tick() = 0;
    // Renders the object
    virtual void Draw(Camera camera) const;
    void SetupTextures() const;
    void BindTextures() const;

    void SetTransform(glm::mat4 _transform);
    void ModTransform(glm::mat4 _transform);

    ShaderProgram &shaderProgram;
};
