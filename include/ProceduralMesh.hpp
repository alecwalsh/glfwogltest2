#pragma once

#include "MeshBase.hpp"

#include <cstdint>

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
    PlaneMesh(double x, double y, std::uint32_t subdivisionLevel);
    PlaneMesh(double x, double y, std::uint32_t xSubdivisionLevel, std::uint32_t ySubdivisionLevel);
};