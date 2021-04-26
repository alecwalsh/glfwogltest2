#pragma once

#include <glm/vec3.hpp>

namespace Physics {
    constexpr float earthGravity = 9.81f;

    bool collidesWithFloor(float height, float size, float floorHeight);

    float calculateDistance(float& velocity);

    // Applies gravity and collision detection and return a translation to be applied to the object
    glm::vec3 getTranslation(float& velocity, float height, float size, float floorHeight = 0);
}