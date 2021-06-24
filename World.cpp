#include "World.hpp"

#include "InputManager.hpp"
#include "UIManager.hpp"
#include "ShaderManager.hpp"
#include "PostProcess.hpp"

#include "Window.hpp"

#include "Mesh.hpp"
#include "ProceduralMesh.hpp"

#include "DirLight.hpp"
#include "Flashlight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <iostream>

using glm::vec3, glm::mat4;

World::World() {
    InputManager& im = InputManager::GetInstance();

    // A fullscreen quad
    // The scene is rendered to a texture and the texture is applied to the quad
    PostProcess& fsq = PostProcess::GetInstance();

    Window& window = Window::GetInstance();

#if __cpp_lib_bind_front >= 201907L
    auto translateCamera = std::bind_front(&Camera::TranslateCamera, &camera);
#else
    auto translateCamera = std::bind(&Camera::TranslateCamera, &camera, std::placeholders::_1);
#endif

    using KeyState = InputManager::KeyState;
    using Direction = Camera::Direction;

#define KEY(k) GLFW_KEY_##k

    // Set key bindings
    // TODO: Set in config file
    im.AddKeyBinding(KEY(W), KeyState::AnyPress, translateCamera(Direction::Forward));
    im.AddKeyBinding(KEY(A), KeyState::AnyPress, translateCamera(Direction::Left));
    im.AddKeyBinding(KEY(S), KeyState::AnyPress, translateCamera(Direction::Backward));
    im.AddKeyBinding(KEY(D), KeyState::AnyPress, translateCamera(Direction::Right));
    im.AddKeyBinding(KEY(SPACE), KeyState::AnyPress, translateCamera(Direction::Up));
    im.AddKeyBinding(KEY(C), KeyState::AnyPress, translateCamera(Direction::Down));
    im.AddKeyBinding(KEY(ESCAPE), KeyState::InitialPress, [&] { window.Close(); });
    im.AddKeyBinding(KEY(R), KeyState::InitialPress, [&] {
        static bool toggled = false;
        if (toggled) {
            fsq.ReloadShader("shaders/vert_postprocess.glsl", "shaders/frag_postprocess_passthrough.glsl");
            toggled = false;
        } else {
            fsq.ReloadShader("shaders/vert_postprocess.glsl", "shaders/frag_postprocess_sobel.glsl");
            toggled = true;
        }
    });

    im.AddKeyBinding(KEY(U), KeyState::InitialPress, UIManager::ToggleUI);

    ShaderProgram* pCubeShader;
    ShaderProgram* pLightShader;

    try {
        pCubeShader = &shaderManager.AddShader({"shaders/vert_cube.glsl", "shaders/frag_cube.glsl", window.glVersion});
        pLightShader =
            &shaderManager.AddShader({"shaders/vert_light.glsl", "shaders/frag_light.glsl", window.glVersion});
    } catch (const ShaderError& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // TODO: Add AssetManager, like TextureManager but for all assets
    // Compile and link shaders
    ShaderProgram& cubeShader = *pCubeShader;
    ShaderProgram& lightShader = *pLightShader;

    cubeShader.SetupTextures();


    meshes.push_back(std::make_unique<PlaneMesh>());
    MeshBase& floorMesh = *meshes.back().get();
    
    // TODO: use different format
    meshes.push_back(std::make_unique<Mesh>("data/cube_irreg.fbx"));
    MeshBase& mesh = *meshes.back().get();
    
    meshes.push_back(std::make_unique<CuboidMesh>());
    MeshBase& lightMesh = *meshes.back().get();

    meshes.push_back(std::make_unique<SphereMesh>());
    MeshBase& sphereMesh = *meshes.back().get();

    try {
        texman.AddTextureFromFile("container", "container2.png");
        texman.AddTextureFromFile("container_specular", "container2_specular.png");
        texman.AddTextureFromFile("normalmaptest1", "normalmaptest1.png");
        texman.AddTextureFromFile("puppy", "sample2.png");
        texman.AddTextureFromFile("gradient", "gradient.png");
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }


    // Creates a CubeObject
    auto go1 = std::make_unique<CubeObject>(mesh, cubeShader, texman);
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

    auto go5 = std::make_unique<CubeObject>(mesh, cubeShader, texman);
    go5->SetPosition({5.0f, 5.0f, 0.0f});
    go5->name = "cube2";
    go5->SetCollider<Physics::SimpleCubeCollider>();

    auto& physicsObjects = World::physicsObjects;

    physicsObjects.push_back(go1.get());
    physicsObjects.push_back(go2.get());
    physicsObjects.push_back(go3.get());
    physicsObjects.push_back(go4.get());
    physicsObjects.push_back(go5.get());

    im.AddKeyBinding(KEY(L), KeyState::InitialPress, [ptr = go5.get()] {
        ptr->ModifyPosition({-1, 0, 0});    
    });

    gameObjects.push_back(std::move(go1));
    gameObjects.push_back(std::move(go2));
    gameObjects.push_back(std::move(go3));
    gameObjects.push_back(std::move(go4));
    gameObjects.push_back(std::move(go5));

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

    physicsObjects.push_back(floor.get());
    physicsObjects.push_back(floor2.get());

    gameObjects.push_back(std::move(floor));
    gameObjects.push_back(std::move(floor2));



    auto pointLight = std::make_unique<PointLight>(vec3{3.0f, 1.0f, 2.0f}, vec3{0.5f}, vec3{1.0f});
    auto pointLight2 = std::make_unique<PointLight>(vec3{-6.0f, 1.0f, -2.0f}, vec3{0.5f}, vec3{1.0f});
    lights.push_back(std::move(pointLight));
    lights.push_back(std::move(pointLight2));

    auto dirLight = std::make_unique<DirLight>(vec3{-1.0f, 0.0f, 0.0f}, vec3{3.0f}, vec3{3.0f});

    lights.push_back(std::move(dirLight));

    auto spotLight = std::make_unique<SpotLight>(vec3{3.0f, 0.75f, 0.0f}, vec3{-1.0f, -0.25f, 0.0f}, vec3{3.0f},
                                                 vec3{3.0f}, glm::cos(glm::radians(15.5f)));
    lights.push_back(std::move(spotLight));

    auto flashlight = std::make_unique<Flashlight>(vec3{-1.0f, -0.25f, 0.0f}, vec3{3.0f}, vec3{3.0f},
                                                   glm::cos(glm::radians(15.5f)), camera);
    flashlight->ToggleActive();
    lights.push_back(std::move(flashlight));

    std::size_t flashlight_idx = lights.size() - 1;

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

    im.AddKeyBinding(KEY(F), KeyState::InitialPress,
                     [&lights = this->lights, flashlight_idx] { lights[flashlight_idx]->ToggleActive(); });
}

void World::TickAll() {
    for (auto& go : gameObjects) {
        go->Tick();
    }
}