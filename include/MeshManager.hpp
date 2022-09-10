#pragma once

#include "MeshBase.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <map>

namespace GameEngine {

class MeshManager {
    std::map<std::string, std::unique_ptr<MeshBase>, std::less<>> meshes;
  public:
    MeshBase& FromName(std::string_view name);
    MeshBase& AddMesh(std::string_view name, std::unique_ptr<MeshBase> mesh);
};

} // namespace GameEngine
