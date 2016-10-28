#include "Light.h"



Light::Light(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime) : GameObject(_mesh, _shaderProgram, _transform, _elapsedTime, _deltaTime)
{
}


Light::~Light()
{
}

void Light::Tick()
{

}