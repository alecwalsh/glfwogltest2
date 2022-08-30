#pragma once

#include <vector>
#include <memory>

#include "PerspectiveCamera.hpp"
#include "CubeObject.hpp"
#include "MeshManager.hpp"
#include "Light.hpp"
#include "TextureManager.hpp"

namespace GameEngine {

class World {
    TextureManager texman;

    MeshManager meshManager;

  public:
    std::vector<std::unique_ptr<RenderableObject>> gameObjects;
    std::vector<Physics::Collider*> physicsObjects;

    // TODO: Create LightObject class
    // The white cubes that represent lights
    std::vector<std::unique_ptr<RenderableObject>> lightObjects;

    std::vector<std::unique_ptr<Light>> lights;

    PerspectiveCamera camera{{3.0f, 3.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, 2.5f};

    World();

    void PhysicsTick();

    // Calls Tick on all GameObjects, LightObjects, and the camera
    void TickAll();

    void CreateShaders();
    void CreateGameObjects();
    void CreateMeshes();
    void CreateTextures();
    void CreateLights();
    void CreateLightObjects();

    void SetupKeyBindings();

    void AddGameObject(std::unique_ptr<RenderableObject> object);

    void RenderWorld();
};


inline World* world = nullptr;
} // namespace GameEngine