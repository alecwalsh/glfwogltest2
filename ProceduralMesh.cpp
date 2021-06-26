#include "ProceduralMesh.hpp"

#include "ProceduralMeshData.hpp"

SphereMesh::SphereMesh(double radius) : MeshBase{CreateSphereMeshData(radius)} {}

SphereMesh::SphereMesh() : SphereMesh{0.5} {}

CuboidMesh::CuboidMesh(double x, double y, double z) : MeshBase{CreateCuboidMeshData(x, y, z)} {}

CuboidMesh::CuboidMesh(double size) : CuboidMesh{size, size, size} {}

CuboidMesh::CuboidMesh() : CuboidMesh{1} {}

PlaneMesh::PlaneMesh(double x, double y) : MeshBase{CreatePlaneMeshData(x, y)} {}

PlaneMesh::PlaneMesh(double size) : PlaneMesh{size, size} {}

PlaneMesh::PlaneMesh() : PlaneMesh{1} {}