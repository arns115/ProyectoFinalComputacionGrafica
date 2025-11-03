#include "MeshManager.h"

// Inicializa el MeshManager
// Se cargan todos los meshes 
MeshManager::MeshManager() 
{
	// Crear todos los meshes al inicializar
	createPiramideMesh();
	createPisoMesh();
	createVegetacionMesh();
}

// Carga un mesh en el manager
void MeshManager::loadMesh(const std::string& meshName, Mesh* mesh)
{
	if (mesh != nullptr) {
		meshes[meshName] = mesh;
	}
}

// Obtiene un mesh por su nombre 
Mesh* MeshManager::getMesh(const std::string& meshName)
{
	auto it = meshes.find(meshName);
	if (it != meshes.end()) {
		return it->second;
	}
	return nullptr;
}

// Crear mesh de pirámide
void MeshManager::createPiramideMesh()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	// Calcular normales
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	// Crear el mesh
	Mesh* piramideMesh = new Mesh();
	piramideMesh->CreateMesh(vertices, indices, 32, 12);
	loadMesh(AssetConstants::MeshNames::PIRAMIDE, piramideMesh);
}

// Crear mesh de piso
void MeshManager::createPisoMesh()
{
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	// Crear el mesh
	Mesh* pisoMesh = new Mesh();
	pisoMesh->CreateMesh(floorVertices, floorIndices, 32, 6);
	loadMesh(AssetConstants::MeshNames::PISO, pisoMesh);
}

// Crear mesh de vegetación
void MeshManager::createVegetacionMesh()
{
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4, 5, 6,
	   4, 6, 7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	// Calcular normales
	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	// Crear el mesh
	Mesh* vegetacionMesh = new Mesh();
	vegetacionMesh->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	loadMesh(AssetConstants::MeshNames::VEGETACION, vegetacionMesh);
}

// Función de cálculo de normales por promedio de vértices
void MeshManager::calcAverageNormals(unsigned int* indices, unsigned int indiceCount, 
                                     GLfloat* vertices, unsigned int verticeCount,
                                     unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

MeshManager::~MeshManager() 
{
}
