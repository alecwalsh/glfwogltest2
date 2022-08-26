#include "MeshManager.hpp"

#include <cassert>
#include <stdexcept>

namespace GameEngine {
MeshBase& MeshManager::FromName(std::string_view name) {
    auto it = meshes.find(name);
    if (it == meshes.end()) throw std::out_of_range{"Mesh not found"};

    return *it->second;
}
MeshBase& MeshManager::AddMesh(std::string_view name, std::unique_ptr<MeshBase> mesh) {
    auto nameIter = meshes.find(name);

    if (nameIter != meshes.end()) {
        const auto& newMesh = nameIter->second;

        if (newMesh != mesh) {
            throw std::runtime_error{"Attempted to reassign mesh " + std::string{name}};
        }

        return *newMesh;
    }

    auto [iter, inserted] = meshes.emplace(name, std::move(mesh));

    assert(inserted);

    return *iter->second;
}
} // namespace GameEngine