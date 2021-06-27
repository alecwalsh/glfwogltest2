#pragma once

#include "MeshBase.hpp"

class SphereMesh : public MeshBase {
  public:
    SphereMesh();
    SphereMesh(double radius);
};

class CuboidMesh : public MeshBase {
  public:
    using MeshBase::MeshBase;

    CuboidMesh();
    CuboidMesh(double size);
    CuboidMesh(double x, double y, double z);
};

class PlaneMesh : public MeshBase {
  public:
    using MeshBase::MeshBase;

    PlaneMesh();
    PlaneMesh(double size);
    PlaneMesh(double x, double y);
};