#pragma once

#include "MeshBase.hpp"

#include <string>

class FileMesh : public MeshBase {
    [[nodiscard]] MeshData ImportMesh(const std::string& fileName);
  public:
    // TODO: copy/move constructors
    [[nodiscard]] FileMesh(const std::string& fileName); // Reads vertex data from file
};
