#include <iostream>
#include <fstream>
#include <sstream>

#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices): usesElementArray(false)
{
	this->vertices = vertices;
	UploadToGPU();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> elements) : usesElementArray(true)
{
	this->vertices = vertices;
	this->elements = elements;
	UploadToGPU();
}

Mesh::Mesh(std::string fileName) : usesElementArray(true)
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
//TODO: Shouldn't return bool
void Mesh::ImportMesh( const std::string& pFile)
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
    }
    
    auto mesh = scene->mMeshes[0];
    
        if(!mesh->HasNormals())
        {
            std::cerr << "Mesh doesn't have normals." << std::endl;
            exit(EXIT_FAILURE);
        }
    
    for(int i = 0; i < mesh->mNumFaces; i++)
    {
        auto face = mesh->mFaces[i];
        //TODO: Add error handling / support more than just triangles
        if(face.mNumIndices != 3) {
                std::cerr << "Wrong number of vertices" << std::endl;
                exit(1);
        }
        
        for(auto j = 0; j < face.mNumIndices; j++)
        {
            elements.push_back(face.mIndices[j]);
        }
    }
    
    for(int i = 0; i < mesh->mNumVertices; i++)
    {
        auto v = mesh->mVertices[i];
        auto n = mesh->mNormals[i];
        
        glm::vec2 texcoords;
        
        if(mesh->mTextureCoords[0])
        {
            texcoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else
        {
            texcoords = {0, 0};
        }
        
        vertices.push_back({
            {v.x/2, v.y/2, v.z/2}, //Position
            {n.x, n.y, n.z},       //Normals
            texcoords              //Texture coordinates
        });
    }
}