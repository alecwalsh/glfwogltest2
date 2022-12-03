#include "GameObject.hpp"

#include "TimeManager.hpp"

#include <spdlog/spdlog.h>

glm::mat4 GameObject::GetTransform() const noexcept {
    glm::mat4 scaleMat = glm::scale(glm::mat4{1.0f}, scale);
    glm::mat4 positionMat = glm::translate(glm::mat4{1.0f}, position);

    return positionMat * rotation * scaleMat;
}

GameObject::GameObject()
    : elapsedTime{timeManager.elapsedTime}, deltaTime{timeManager.deltaTime} {
    spdlog::info("GameObject constructor");
}

GameObject::GameObject(glm::vec3 position) : GameObject{position, {1, 1, 1}} {}

GameObject::GameObject(glm::vec3 position, glm::vec3 scale) : GameObject{} {
    this->position = position;
    this->scale = scale;
}

// Copy constructor
GameObject::GameObject(const GameObject& rhs)
    : position{rhs.position}, scale{rhs.scale}, elapsedTime{rhs.elapsedTime}, deltaTime{rhs.deltaTime} {
    spdlog::info("GameObject copy constructor");
}

GameObject::~GameObject() { spdlog::info("GameObject destructor"); }