#pragma once

#include <vector>
#include <memory>

#include "Camera.hpp"
#include "CubeObject.hpp"
#include "MeshBase.hpp"
#include "Light.hpp"
#include "TextureManager.hpp"

// TODO: Move render function to a method of this class
class World {
    TextureManager texman;

    std::vector<std::unique_ptr<MeshBase>> meshes;
  public:
    std::vector<std::unique_ptr<RenderableObject>> gameObjects;

    // TODO: Create LightObject class
    // The white cubes that represent lights
    std::vector<std::unique_ptr<RenderableObject>> lightObjects;

    std::vector<std::unique_ptr<Light>> lights;

    static inline std::vector<CubeObject*> physicsObjects;

    Camera camera{{3.0f, 3.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, 2.5f};

    World();

    // Calls Tick on all GameObjects
    void TickAll();
};