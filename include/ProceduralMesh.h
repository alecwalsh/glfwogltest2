#pragma once
#include "MeshBase.h"

class SphereMesh : public MeshBase {
  public:
    SphereMesh();
    SphereMesh(double radius);
};

class CuboidMesh : public MeshBase {
  public:
    CuboidMesh();
    CuboidMesh(double size);
    CuboidMesh(double x, double y, double z);
};