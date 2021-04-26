#include "Collision.h"

#include "TimeManager.h"

namespace Physics {

bool collidesWithFloor(float height, float size, float floorHeight) {
    return (height - size / 2) <= floorHeight && (height + size/2) >= floorHeight;
}

float calculateDistance(float& velocity) {
    float acceleration = earthGravity;

    float time = static_cast<float>(timeManager.deltaTime);

    velocity += acceleration * time;

    float distance = velocity * time + (acceleration * time * time) / 2;

    return distance;
}

glm::vec3 getTranslation(float& velocity, float height, float size, float floorHeight) {
    float distance;

    if (collidesWithFloor(height, size, floorHeight)) {
        // Is currently colliding with the floor
        distance = 0;
        velocity = 0;
    } else {
        distance = calculateDistance(velocity);
        // Is not currently colliding with the floor
        if (collidesWithFloor(height - distance, size, floorHeight)) {
            // The new height will collide with the floor
            // Set distance so that the new height is exactly at the floor
            distance = height - size / 2 - floorHeight;
        }
    }

    return {0.0f, -distance, 0.0f};
}

} // namespace Physics