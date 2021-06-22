#include "ProceduralMesh.hpp"

#include "ProceduralMeshData.hpp"

//using namespace ProceduralMeshData;

SphereMesh::SphereMesh(double radius) {
    meshData = CreateSphereMeshData(radius);

    UploadToGPU();
}

SphereMesh::SphereMesh() : SphereMesh{0.5} {}

CuboidMesh::CuboidMesh(double x, double y, double z) {
    meshData = CreateCuboidMeshData(x, y, z);

    UploadToGPU();
}

CuboidMesh::CuboidMesh(double size) : CuboidMesh{size, size, size} {}

CuboidMesh::CuboidMesh() : CuboidMesh{1} {}

PlaneMesh::PlaneMesh(double x, double y) {
    meshData = CreatePlaneMeshData(x, y);

    UploadToGPU();
}

PlaneMesh::PlaneMesh(double size) : PlaneMesh{size, size} {}

PlaneMesh::PlaneMesh() : PlaneMesh{1} {}