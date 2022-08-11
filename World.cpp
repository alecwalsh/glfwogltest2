#include "World.hpp"

#include "InputManager.hpp"
#include "UIManager.hpp"
#include "ShaderManager.hpp"
#include "PostProcess.hpp"

#include "Window.hpp"

#include "FileMesh.hpp"
#include "ProceduralMesh.hpp"

#ifdef MESHDATA_USE_CONSTEXPR
#include "ProceduralMeshData.hpp"
#endif

#include "DirLight.hpp"
#include "Flashlight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <iostream>

#define KEY(k) GLFW_KEY_##k

static InputManager& im = InputManager::GetInstance();

using KeyState = InputManager::KeyState;

using glm::vec3, glm::mat4;

namespace GameEngine {

World::World() {
    SetupKeyBindings();

    CreateMeshes();

    try {
        CreateShaders();
    } catch (const ShaderError& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    try {
        CreateTextures();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    CreateGameObjects();

    CreateLights();

    CreateLightObjects();

    auto& window = Window::GetInstance();
    camera.SetWidth(window.width);
    camera.SetHeight(window.height);
}

void World::TickAll() {
    // Call Tick on the camera
    camera.Tick();

    // Call Tick on all GameObjects
    for (auto& go : gameObjects) {
        go->Tick();
    }

    // Call Tick on all of the lights
    for (auto& lo : lightObjects) {
        lo->Tick();
    }
}

void World::SetupKeyBindings() {
    auto translateCamera = std::bind_front(&CameraBase::TranslateCamera, &camera);

    using Direction = CameraBase::Direction;

    // Set key bindings
    // TODO: Get key bindings from config file
    // TODO: Figure out how to use control key
    im.AddKeyBinding(KEY(W), KeyState::AnyPress, translateCamera(Direction::Forward));
    im.AddKeyBinding(KEY(A), KeyState::AnyPress, translateCamera(Direction::Left));
    im.AddKeyBinding(KEY(S), KeyState::AnyPress, translateCamera(Direction::Backward));
    im.AddKeyBinding(KEY(D), KeyState::AnyPress, translateCamera(Direction::Right));
    im.AddKeyBinding(KEY(SPACE), KeyState::AnyPress, translateCamera(Direction::Up));
    im.AddKeyBinding(KEY(C), KeyState::AnyPress, translateCamera(Direction::Down));

    im.AddKeyBinding(KEY(ESCAPE), KeyState::InitialPress, [] { Window::GetInstance().Close(); });
    
    im.AddKeyBinding(KEY(R), KeyState::InitialPress, [&] {
        PostProcess& fsq = PostProcess::GetInstance();

        static bool toggled = false;
        if (toggled) {
            fsq.ReloadShader("postprocess_passthrough");
            toggled = false;
        } else {
            fsq.ReloadShader("postprocess_sobel");
            toggled = true;
        }
    });

    im.hideUIKey = KEY(U);
    im.AddKeyBinding(im.hideUIKey, KeyState::InitialPress, UIManager::ToggleUI);
}

void World::CreateShaders() {
    shaderManager.AddShader("cubeShader", {"shaders/vert_cube.glsl", "shaders/frag_cube.glsl"})
        .SetupTextures();
    shaderManager.AddShader("lightShader", {"shaders/vert_light.glsl", "shaders/frag_light.glsl"});
}

void World::CreateGameObjects() {
    MeshBase& cubeMesh = *meshManager.meshes["cubeMesh"];
    MeshBase& sphereMesh = *meshManager.meshes["sphereMesh"];

    MeshBase& floorMesh = *meshManager.meshes["floorMesh"];

    auto& cubeShader = shaderManager.FromName("cubeShader");

    // Creates a CubeObject
    auto go1 = std::make_unique<CubeObject>(cubeMesh, cubeShader, texman);
    go1->SetPosition({0.0f, 25.0f, 0.0f});
    go1->name = "cube1";
    go1->SetCollider<Physics::SimpleCubeCollider>();

    auto go2 = std::make_unique<CubeObject>(sphereMesh, cubeShader, texman);
    go2->SetPosition({0, 0, 2.0f});
    go2->name = "sphere1";
    go2->texture_name = "gradient";
    go2->SetCollider<Physics::SphereCollider>();

    auto go3 = std::make_unique<CubeObject>(sphereMesh, cubeShader, texman);
    go3->SetPosition({0, 50.0f, 2.0f});
    go3->name = "sphere2";
    go3->texture_name = "gradient";
    go3->SetCollider<Physics::SphereCollider>();

    auto go4 = std::make_unique<CubeObject>(sphereMesh, cubeShader, texman);
    go4->SetPosition({0, 5.0f, 2.0f});
    go4->name = "sphere3";
    go4->texture_name = "gradient";
    go4->SetCollider<Physics::SphereCollider>();
    go4->GetCollider().velocity = {2, 10, 0};

    auto go5 = std::make_unique<CubeObject>(cubeMesh, cubeShader, texman);
    go5->SetPosition({5.0f, 5.0f, 0.0f});
    go5->name = "cube2";
    go5->SetCollider<Physics::SimpleCubeCollider>();

    im.AddKeyBinding(KEY(L), KeyState::InitialPress, [ptr = go5.get()] { ptr->ModifyPosition({-1, 0, 0}); });

    AddGameObject(std::move(go1));
    AddGameObject(std::move(go2));
    AddGameObject(std::move(go3));
    AddGameObject(std::move(go4));
    AddGameObject(std::move(go5));

    mat4 floorRotation = glm::rotate(mat4{1.0f}, glm::radians(90.0f), {-1.0f, 0.0f, 0.0f});
    auto floor = std::make_unique<CubeObject>(floorMesh, cubeShader, texman);
    floor->SetScale({10.0f, 10.0f, 1.0f});
    floor->rotation = floorRotation;
    floor->name = "floor";
    floor->texture_name = "container";
    floor->spec_texture_name = "container_specular";
    floor->SetCollider<Physics::SimplePlaneCollider>();

    auto floor2 = std::make_unique<CubeObject>(floorMesh, cubeShader, texman);
    floor2->SetScale({10.0f, 10.0f, 1.0f});
    floor2->SetPosition({0.0f, -10.0f, 0.0f});
    floor2->rotation = floorRotation;
    floor2->name = "floor";
    floor2->texture_name = "container";
    floor2->spec_texture_name = "container_specular";
    floor2->SetCollider<Physics::SimplePlaneCollider>();

    AddGameObject(std::move(floor));
    AddGameObject(std::move(floor2));
}

void World::CreateMeshes() {
#ifdef MESHDATA_USE_CONSTEXPR
    constexpr auto cubeMeshData = CreateCuboidMeshDataConstexpr();
    constexpr auto planeMeshData = CreatePlaneMeshDataConstexpr();
    
    auto cubeMesh = std::make_unique<CuboidMesh>(cubeMeshData);
    auto planeMesh = std::make_unique<PlaneMesh>(planeMeshData);
#else
    auto cubeMesh = std::make_unique<CuboidMesh>();
    auto planeMesh = std::make_unique<PlaneMesh>();
#endif

    meshManager.meshes.emplace("floorMesh", std::move(planeMesh));
    // TODO: use different format
    meshManager.meshes.emplace("cubeMesh", std::make_unique<FileMesh>("data/cube_irreg.fbx"));
    meshManager.meshes.emplace("lightMesh", std::move(cubeMesh));
    meshManager.meshes.emplace("sphereMesh", std::make_unique<SphereMesh>());
}

void World::CreateTextures() {
    texman.AddTextureFromFile("container", "container2.png");
    texman.AddTextureFromFile("container_specular", "container2_specular.png");
    texman.AddTextureFromFile("normalmaptest1", "normalmaptest1.png");
    texman.AddTextureFromFile("puppy", "sample2.png");
    texman.AddTextureFromFile("gradient", "gradient.png");
}

void World::CreateLights() {
    auto pointLight = std::make_unique<PointLight>(vec3{3.0f, 1.0f, 2.0f}, vec3{0.5f}, vec3{1.0f});
    auto pointLight2 = std::make_unique<PointLight>(vec3{-6.0f, 1.0f, -2.0f}, vec3{0.5f}, vec3{1.0f});

    auto dirLight = std::make_unique<DirLight>(vec3{-1.0f, -0.25f, 0.0f}, vec3{3.0f}, vec3{3.0f});

    auto spotLight = std::make_unique<SpotLight>(vec3{3.0f, 0.75f, 0.0f}, vec3{-1.0f, -0.25f, 0.0f}, vec3{3.0f},
                                                 vec3{3.0f}, glm::cos(glm::radians(15.5f)));
    auto flashlight = std::make_unique<Flashlight>(vec3{-1.0f, -0.25f, 0.0f}, vec3{3.0f}, vec3{3.0f},
                                                   glm::cos(glm::radians(15.5f)), camera, false);

    lights.push_back(std::move(pointLight));
    lights.push_back(std::move(pointLight2));
    lights.push_back(std::move(dirLight));
    lights.push_back(std::move(spotLight));
    lights.push_back(std::move(flashlight));

    im.AddKeyBinding(KEY(F), KeyState::InitialPress, [&fl = lights.back()] { fl->ToggleActive(); });
}

void World::CreateLightObjects() {
    MeshBase& lightMesh = *meshManager.meshes["lightMesh"];

    auto& lightShader = shaderManager.FromName("lightShader");

    // TODO: Light objects appear in the wrong place compared to the location of the light
    for (size_t i = 0; i < lights.size(); i++) {
        if (lights[i]->type == Light::LightType::Point) {
            auto light = static_cast<PointLight*>(lights[i].get());

            auto lo = std::make_unique<CubeObject>(lightMesh, lightShader, texman);
            // Scale by 0.5 then translate to correct position
            lo->SetScale(vec3{0.5f});
            lo->SetPosition({light->position.x, light->position.y, light->position.z});

            lightObjects.push_back(std::move(lo));
        }
    }
}

void World::AddGameObject(std::unique_ptr<RenderableObject> object) {
    gameObjects.push_back(std::move(object));
}

void World::RenderWorld() {
    // Render all of the GameObjects
    for (const auto& go : gameObjects) {
        go->RenderObject(lights, camera);
    }

    // Render all of the lights
    for (const auto& lo : lightObjects) {
        lo->RenderObject(lights, camera);
    }
}

} // namespace GameEngine
