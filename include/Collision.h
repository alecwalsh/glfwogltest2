#pragma once

#include <glm/vec3.hpp>

namespace Physics {

struct SphereCollider {
    glm::vec3 position;
    float radius;
};

struct SimpleCubeCollider {
    glm::vec3 position;
    float size;
};

constexpr float earthGravity = 9.81f;

// Applies gravity and collision detection and return a translation to be applied to the object
glm::vec3 getTranslation(glm::vec3& velocityVector, const SimpleCubeCollider& collider, float floorHeight = 0);

glm::vec3 getTranslationSphere(glm::vec3& velocity, const SphereCollider& thisSphere, const SphereCollider& otherSphere);

}