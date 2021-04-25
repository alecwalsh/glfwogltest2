#include "GameObject.h"

#include <iostream>
#include <limits>
#include <cstdlib>

#include "TimeManager.h"

GameObject::GameObject(glm::mat4 transform)
    : transform{transform}, elapsedTime{timeManager.elapsedTime}, deltaTime{timeManager.deltaTime} {
    std::cout << "GameObject constructor" << std::endl;
}

GameObject::~GameObject() { std::cout << "GameObject destructor" << std::endl; }

// Copy constructor
GameObject::GameObject(const GameObject& rhs)
    : transform{rhs.transform}, elapsedTime{rhs.elapsedTime}, deltaTime{rhs.deltaTime} {
    std::cout << "GameObject copy constructor" << std::endl;
}

// Sets the transform
void GameObject::SetTransform(const glm::mat4& transform) { this->transform = transform; }

// Modifies the transform
void GameObject::ModTransform(const glm::mat4& transform) { this->transform *= transform; }
