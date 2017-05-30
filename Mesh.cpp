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
	std::ifstream infile(fileName);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::vector<float> vertex(8);
		
		iss >> vertex[0] >> vertex[1] >> vertex[2] >> vertex[3] >> vertex[4] >> vertex[5] >> vertex[6] >> vertex[7];

		for (int i = 0; i < 8; i++)
		{
			vertices.push_back(vertex[i]);
		}
	}

#ifdef USE_ASSIMP
    //HACK: Just replaces the positions right now, need to read normals and uv's from blend file too
    ImportMesh("data/cube.blend");
    for(int i = 0; i < vertices.size()/8; i++)
    {
        std::vector<float> vert = {vertices[8*i],vertices[8*i+1],vertices[8*i+2]};
        vertices[8*i] = vertices_assimp[3*i]/2;
        vertices[8*i+1] = vertices_assimp[3*i+1]/2;
        vertices[8*i+2] = vertices_assimp[3*i+2]/2;
        std::vector<float> vert2 = {vertices[8*i],vertices[8*i+1],vertices[8*i+2]};
        std::cout << "" << std::endl;
    }
#endif
	
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

bool Mesh::ImportMesh( const std::string& pFile)
{
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll 
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( pFile, 
        aiProcess_CalcTangentSpace       | 
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    // If the import failed, report it
    if( !scene)
    {
        std::cout << importer.GetErrorString() << std::endl;
        return false;
    }
    
    auto nf = scene->mMeshes[0]->mNumFaces;
    
    auto mesh = scene->mMeshes[0];
    //auto norms = mesh->HasNormals();
    
    for(auto i = 0; i < nf; i++)
    {
        auto face = mesh->mFaces[i];
        switch(face.mNumIndices) {
            case 1: 
            case 2: return false;
            case 3: break;
            default: return false;
        }
        for(auto j = 0; j < face.mNumIndices; j++)
        {
            auto v = mesh->mVertices[face.mIndices[j]];
            vertices_assimp.push_back(v.x);
            vertices_assimp.push_back(v.y);
            vertices_assimp.push_back(v.z);
            std::cout << "a";
        }
    }
    // Now we can access the file's contents. 
    //DoTheSceneProcessing( scene);
    // We're done. Everything will be cleaned up by the importer destructor
    
    std::cout << vertices_assimp.size() << std::endl;
    return true;
}
