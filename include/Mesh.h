#pragma once
#include "MeshBase.h"

#include "glad/glad.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#include <vector>

class Mesh : public MeshBase {
  public:

    // TODO: copy/move constructors
    Mesh(const std::string& fileName); // Reads vertex data from text file

  private:
    void ImportMesh(const std::string& pFile);
};
