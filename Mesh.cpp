#include <iostream>
#include <fstream>
#include <sstream>

#include "Mesh.h"

Mesh::Mesh(std::vector<GLfloat> vertices): usesElementArray(false)
{
	this->vertices = vertices;
	UploadToGPU();
}

Mesh::Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> elements) : usesElementArray(true)
{
	this->vertices = vertices;
	this->elements = elements;
	UploadToGPU();
}

Mesh::Mesh(std::string fileName) : usesElementArray(false)
{
    ImportMesh(fileName);
    UploadToGPU();
}

Mesh::~Mesh()
{
}


// Generates buffers and uploads data to graphics card
void Mesh::UploadToGPU()
{
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	GLuint ebo;

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	buffers.vbo = vbo;
	buffers.ebo = ebo;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*(vertices.size()), vertices.data(), GL_STATIC_DRAW);

	if (usesElementArray)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements[0])*(elements.size()), elements.data(), GL_STATIC_DRAW);
	}
}

//TODO: .blend files normals are per vertex, not per face; .fbx works fine
bool Mesh::ImportMesh( const std::string& pFile)
{
    Assimp::Importer importer;
    //TODO: Change/add postprocessing flags
    const aiScene* scene = importer.ReadFile( pFile, 
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices);

    // If the import failed, report it
    if(!scene)
    {
        std::cout << importer.GetErrorString() << std::endl;
        exit(EXIT_FAILURE);
        return false;
    }
    
    auto mesh = scene->mMeshes[0];
    
    for(auto i = 0; i < mesh->mNumFaces; i++)
    {
        auto face = mesh->mFaces[i];
        //TODO: Add error handling / support more than just triangles
        switch(face.mNumIndices) {
            case 1: 
            case 2: return false;
            case 3: break;
            default: return false;
        }
        //TODO: use indices for element buffer
        for(auto j = 0; j < face.mNumIndices; j++)
        {
            auto v = mesh->mVertices[face.mIndices[j]];
            auto n = mesh->mNormals[face.mIndices[j]];
            vertices.push_back(v.x/2);
            vertices.push_back(v.y/2);
            vertices.push_back(v.z/2);
            if(mesh->HasNormals())
            {
                vertices.push_back(n.x);
                vertices.push_back(n.y);
                vertices.push_back(n.z);
            } else
            {
                std::cerr << "Mesh doesn't have normals." << std::endl;
                exit(EXIT_FAILURE);
            }
            
            if(mesh->mTextureCoords[0])
            {
                vertices.push_back(mesh->mTextureCoords[0][face.mIndices[j]].x);
                vertices.push_back(mesh->mTextureCoords[0][face.mIndices[j]].y);
            } else
            {
                vertices.push_back(0);
                vertices.push_back(0);
            }
        }
    }
    return true;
}
