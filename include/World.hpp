#pragma once

#include "MeshBase.hpp"

#include <memory>
#include <string>
#include <unordered_map>

struct MeshManager {
    std::unordered_map<std::string, std::unique_ptr<MeshBase>> meshes;
};

// TODO: Move MeshManager to its own file

#include <vector>
#include <memory>

#include "Camera.hpp"
#include "CubeObject.hpp"
#include "MeshBase.hpp"
#include "Light.hpp"
#include "TextureManager.hpp"

namespace GameEngine {

// TODO: Move render function to a method of this class
class World {
    TextureManager texman;

    MeshManager meshManager;

  public:
    std::vector<std::unique_ptr<RenderableObject>> gameObjects;

    // TODO: Create LightObject class
    // The white cubes that represent lights
    std::vector<std::unique_ptr<RenderableObject>> lightObjects;

    std::vector<std::unique_ptr<Light>> lights;

    // TODO: Make this a non-static member
    static inline std::vector<CubeObject*> physicsObjects;

    Camera camera{{3.0f, 3.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, 2.5f};

    World();

    // Calls Tick on all GameObjects, LightObjects, and the camera
    void TickAll();

    void CreateShaders();
    void CreateGameObjects();
    void CreateMeshes();
    void CreateTextures();
    void CreateLights();
    void CreateLightObjects();

    void SetupKeyBindings();
};

} // namespace GameEngine