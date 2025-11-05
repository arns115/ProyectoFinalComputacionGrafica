#include "MeshManager.h"

// Inicializa el MeshManager
// Se cargan todos los meshes 
MeshManager::MeshManager() 
{
	// Crear todos los meshes al inicializar
	createPiramideMesh();
	createPisoMesh();
	createVegetacionMesh();
	createSphereMesh();
	createCaminoMesh();
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

void MeshManager::createSphereMesh()
{
	// Crear una esfera con radio 1.0, 32 slices y 16 stacks para mejor calidad
	Sphere sphere(1.0f, 32, 16);
	sphere.init(); // Generar vértices e índices
	
	// Obtener los datos de la esfera en formato compatible con Mesh
	std::vector<GLfloat> vertexData = sphere.getVertices();
	std::vector<GLuint> indexData = sphere.getIndices();
	
	// Crear el mesh usando los datos extraídos
	Mesh* sphereMesh = new Mesh();
	sphereMesh->CreateMesh(vertexData.data(), 
	                       const_cast<unsigned int*>(indexData.data()), 
	                       vertexData.size(), 
	                       indexData.size());
	
	// Guardar el mesh en el manager
	loadMesh(AssetConstants::MeshNames::ESFERA, sphereMesh);
}

// Crear mesh de camino empedrado (prisma rectangular)
void MeshManager::createCaminoMesh()
{
	// El camino va desde las cabezas olmecas (z=200) hasta la pirámide (z=-150)
	// Ancho del camino: 5 unidades (de -2.5 a 2.5 en x)
	// Altura muy pequeña: 0.1 unidades
	unsigned int caminoIndices[] = {
		// Cara superior
		0, 1, 2,
		0, 2, 3,
		// Cara inferior
		4, 6, 5,
		4, 7, 6,
		// Cara frontal (hacia la pirámide)
		8, 9, 10,
		8, 10, 11,
		// Cara trasera (hacia las cabezas olmecas)
		12, 14, 13,
		12, 15, 14,
		// Cara lateral izquierda
		16, 17, 18,
		16, 18, 19,
		// Cara lateral derecha
		20, 22, 21,
		20, 23, 22
	};

	GLfloat caminoVertices[] = {
		// Cara superior (y = 0.05) - Normales invertidas
		//    x      y      z          u     v        nx    ny    nz
		-2.5f,  0.05f, -150.0f,   0.0f, 0.0f,    0.0f, -1.0f, 0.0f,  // 0
		 2.5f,  0.05f, -150.0f,   1.0f, 0.0f,    0.0f, -1.0f, 0.0f,  // 1
		 2.5f,  0.05f,  200.0f,   1.0f, 70.0f,   0.0f, -1.0f, 0.0f,  // 2
		-2.5f,  0.05f,  200.0f,   0.0f, 70.0f,   0.0f, -1.0f, 0.0f,  // 3
		
		// Cara inferior (y = -0.05) - Normales invertidas
		-2.5f, -0.05f, -150.0f,   0.0f, 0.0f,    0.0f, 1.0f, 0.0f, // 4
		 2.5f, -0.05f, -150.0f,   1.0f, 0.0f,    0.0f, 1.0f, 0.0f, // 5
		 2.5f, -0.05f,  200.0f,   1.0f, 70.0f,   0.0f, 1.0f, 0.0f, // 6
		-2.5f, -0.05f,  200.0f,   0.0f, 70.0f,   0.0f, 1.0f, 0.0f, // 7
		
		// Cara frontal (z = -150, hacia la pirámide) - Normales invertidas
		-2.5f, -0.05f, -150.0f,   0.0f, 0.0f,    0.0f, 0.0f, 1.0f, // 8
		 2.5f, -0.05f, -150.0f,   1.0f, 0.0f,    0.0f, 0.0f, 1.0f, // 9
		 2.5f,  0.05f, -150.0f,   1.0f, 1.0f,    0.0f, 0.0f, 1.0f, // 10
		-2.5f,  0.05f, -150.0f,   0.0f, 1.0f,    0.0f, 0.0f, 1.0f, // 11
		
		// Cara trasera (z = 200, hacia las cabezas olmecas) - Normales invertidas
		-2.5f, -0.05f,  200.0f,   0.0f, 0.0f,    0.0f, 0.0f, -1.0f,  // 12
		 2.5f, -0.05f,  200.0f,   1.0f, 0.0f,    0.0f, 0.0f, -1.0f,  // 13
		 2.5f,  0.05f,  200.0f,   1.0f, 1.0f,    0.0f, 0.0f, -1.0f,  // 14
		-2.5f,  0.05f,  200.0f,   0.0f, 1.0f,    0.0f, 0.0f, -1.0f,  // 15
		
		// Cara lateral izquierda (x = -2.5) - Normales invertidas
		-2.5f, -0.05f, -150.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,  // 16
		-2.5f, -0.05f,  200.0f,   70.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 17
		-2.5f,  0.05f,  200.0f,   70.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 18
		-2.5f,  0.05f, -150.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,  // 19
		
		// Cara lateral derecha (x = 2.5) - Normales invertidas
		 2.5f, -0.05f, -150.0f,   0.0f, 0.0f,    -1.0f, 0.0f, 0.0f,  // 20
		 2.5f, -0.05f,  200.0f,   70.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  // 21
		 2.5f,  0.05f,  200.0f,   70.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  // 22
		 2.5f,  0.05f, -150.0f,   0.0f, 1.0f,    -1.0f, 0.0f, 0.0f   // 23
	};

	// Crear el mesh
	Mesh* caminoMesh = new Mesh();
	caminoMesh->CreateMesh(caminoVertices, caminoIndices, 192, 36);
	loadMesh(AssetConstants::MeshNames::CAMINO, caminoMesh);
}

MeshManager::~MeshManager() 
{
}
