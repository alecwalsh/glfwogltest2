#pragma once

#include "MeshBase.hpp"

#include <string>

class Mesh : public MeshBase {
  public:
    // TODO: copy/move constructors
    [[nodiscard]] Mesh(const std::string& fileName); // Reads vertex data from text file

  private:
    void ImportMesh(const std::string& fileName);
};
