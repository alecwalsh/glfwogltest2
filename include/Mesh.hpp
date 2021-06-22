#pragma once

#include "MeshBase.hpp"

#include <string>

class Mesh : public MeshBase {
    [[nodiscard]] MeshData ImportMesh(const std::string& fileName);
  public:
    // TODO: copy/move constructors
    [[nodiscard]] Mesh(const std::string& fileName); // Reads vertex data from file
};
