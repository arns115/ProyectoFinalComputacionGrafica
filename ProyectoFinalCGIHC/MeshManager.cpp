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
	createPrismaAguaMesh();
	createPrismaPequenoMesh();
	createCanchaParedMesh();
	createCanchaTechoMesh();
	createToroideMesh();
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

// Crear mesh de prisma cuadrado con textura de agua
void MeshManager::createPrismaAguaMesh()
{
	// Prisma cuadrado: 10x10 unidades en x y z
	// Altura: 0.1 unidades (igual que el camino)
	unsigned int prismaIndices[] = {
		// Cara superior
		0, 1, 2,
		0, 2, 3,
		// Cara inferior
		4, 6, 5,
		4, 7, 6,
		// Cara frontal
		8, 9, 10,
		8, 10, 11,
		// Cara trasera
		12, 14, 13,
		12, 15, 14,
		// Cara lateral izquierda
		16, 17, 18,
		16, 18, 19,
		// Cara lateral derecha
		20, 22, 21,
		20, 23, 22
	};

	GLfloat prismaVertices[] = {
		// Cara superior (y = 0.05)
		//    x      y      z        u     v      nx    ny    nz
		-5.0f,  0.05f, -5.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  // 0
		 5.0f,  0.05f, -5.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  // 1
		 5.0f,  0.05f,  5.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,  // 2
		-5.0f,  0.05f,  5.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  // 3
		
		// Cara inferior (y = -0.05)
		-5.0f, -0.05f, -5.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 4
		 5.0f, -0.05f, -5.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 5
		 5.0f, -0.05f,  5.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f, // 6
		-5.0f, -0.05f,  5.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f, // 7
		
		// Cara frontal (z = -5.0)
		-5.0f, -0.05f, -5.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 8
		 5.0f, -0.05f, -5.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 9
		 5.0f,  0.05f, -5.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 10
		-5.0f,  0.05f, -5.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 11
		
		// Cara trasera (z = 5.0)
		-5.0f, -0.05f,  5.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,  // 12
		 5.0f, -0.05f,  5.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,  // 13
		 5.0f,  0.05f,  5.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  // 14
		-5.0f,  0.05f,  5.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,  // 15
		
		// Cara lateral izquierda (x = -5.0)
		-5.0f, -0.05f, -5.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 16
		-5.0f, -0.05f,  5.0f,   1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 17
		-5.0f,  0.05f,  5.0f,   1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 18
		-5.0f,  0.05f, -5.0f,   0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 19
		
		// Cara lateral derecha (x = 5.0)
		 5.0f, -0.05f, -5.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  // 20
		 5.0f, -0.05f,  5.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  // 21
		 5.0f,  0.05f,  5.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  // 22
		 5.0f,  0.05f, -5.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f   // 23
	};

	// Crear el mesh
	Mesh* prismaMesh = new Mesh();
	prismaMesh->CreateMesh(prismaVertices, prismaIndices, 192, 36);
	loadMesh(AssetConstants::MeshNames::CHINAMPA_AGUA, prismaMesh);
}

// Crear mesh de prisma pequeño para colocar sobre el prisma de agua
void MeshManager::createPrismaPequenoMesh()
{
	// Prisma pequeño: 2x2 unidades en x y z
	// Altura: 0.1 unidades (igual que los otros prismas)
	unsigned int prismaIndices[] = {
		// Cara superior
		0, 1, 2,
		0, 2, 3,
		// Cara inferior
		4, 6, 5,
		4, 7, 6,
		// Cara frontal
		8, 9, 10,
		8, 10, 11,
		// Cara trasera
		12, 14, 13,
		12, 15, 14,
		// Cara lateral izquierda
		16, 17, 18,
		16, 18, 19,
		// Cara lateral derecha
		20, 22, 21,
		20, 23, 22
	};

	GLfloat prismaVertices[] = {
		// Cara superior (y = 0.05)
		//    x      y      z        u     v      nx    ny    nz
		-1.0f,  0.05f, -1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  // 0
		 1.0f,  0.05f, -1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  // 1
		 1.0f,  0.05f,  1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,  // 2
		-1.0f,  0.05f,  1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  // 3
		
		// Cara inferior (y = -0.05)
		-1.0f, -0.05f, -1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 4
		 1.0f, -0.05f, -1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 5
		 1.0f, -0.05f,  1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f, // 6
		-1.0f, -0.05f,  1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f, // 7
		
		// Cara frontal (z = -1.0)
		-1.0f, -0.05f, -1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 8
		 1.0f, -0.05f, -1.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, // 9
		 1.0f,  0.05f, -1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 10
		-1.0f,  0.05f, -1.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 11
		
		// Cara trasera (z = 1.0)
		-1.0f, -0.05f,  1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,  // 12
		 1.0f, -0.05f,  1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,  // 13
		 1.0f,  0.05f,  1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  // 14
		-1.0f,  0.05f,  1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,  // 15
		
		// Cara lateral izquierda (x = -1.0)
		-1.0f, -0.05f, -1.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 16
		-1.0f, -0.05f,  1.0f,   1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 17
		-1.0f,  0.05f,  1.0f,   1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 18
		-1.0f,  0.05f, -1.0f,   0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 19
		
		// Cara lateral derecha (x = 1.0)
		 1.0f, -0.05f, -1.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  // 20
		 1.0f, -0.05f,  1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  // 21
		 1.0f,  0.05f,  1.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  // 22
		 1.0f,  0.05f, -1.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f   // 23
	};

	// Crear el mesh
	Mesh* prismaMesh = new Mesh();
	prismaMesh->CreateMesh(prismaVertices, prismaIndices, 192, 36);
	loadMesh(AssetConstants::MeshNames::CHINAMPA_ISLA, prismaMesh);
}

// Crear mesh de pared rectangular para la cancha de pelota maya
void MeshManager::createCanchaParedMesh()
{
	// Prisma rectangular: 30 unidades de largo (Z), 5 de alto (Y), 3 de ancho (X)
	unsigned int paredIndices[] = {
		// Cara superior
		0, 1, 2,
		0, 2, 3,
		// Cara inferior
		4, 6, 5,
		4, 7, 6,
		// Cara frontal (hacia el centro de la cancha)
		8, 9, 10,
		8, 10, 11,
		// Cara trasera (exterior)
		12, 14, 13,
		12, 15, 14,
		// Cara lateral frontal (Z-)
		16, 17, 18,
		16, 18, 19,
		// Cara lateral trasera (Z+)
		20, 22, 21,
		20, 23, 22
	};

	GLfloat paredVertices[] = {
		// Cara superior (y = 5.0)
		//    x      y      z        u     v      nx    ny    nz
		-1.5f,  5.0f, -15.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  // 0
		 1.5f,  5.0f, -15.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  // 1
		 1.5f,  5.0f,  15.0f,   1.0f, 10.0f,  0.0f, -1.0f, 0.0f,  // 2
		-1.5f,  5.0f,  15.0f,   0.0f, 10.0f,  0.0f, -1.0f, 0.0f,  // 3
		
		// Cara inferior (y = 0.0)
		-1.5f,  0.0f, -15.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 4
		 1.5f,  0.0f, -15.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 5
		 1.5f,  0.0f,  15.0f,   1.0f, 10.0f,  0.0f, 1.0f, 0.0f, // 6
		-1.5f,  0.0f,  15.0f,   0.0f, 10.0f,  0.0f, 1.0f, 0.0f, // 7
		
		// Cara frontal (x = -1.5, hacia el centro)
		-1.5f,  0.0f, -15.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f, // 8
		-1.5f,  0.0f,  15.0f,   10.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 9
		-1.5f,  5.0f,  15.0f,   10.0f, 2.0f,  1.0f, 0.0f, 0.0f, // 10
		-1.5f,  5.0f, -15.0f,   0.0f, 2.0f,   1.0f, 0.0f, 0.0f, // 11
		
		// Cara trasera (x = 1.5, exterior)
		 1.5f,  0.0f, -15.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  // 12
		 1.5f,  0.0f,  15.0f,   10.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  // 13
		 1.5f,  5.0f,  15.0f,   10.0f, 2.0f,  -1.0f, 0.0f, 0.0f,  // 14
		 1.5f,  5.0f, -15.0f,   0.0f, 2.0f,   -1.0f, 0.0f, 0.0f,  // 15
		
		// Cara lateral frontal (z = -15.0)
		-1.5f,  0.0f, -15.0f,   0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 16
		 1.5f,  0.0f, -15.0f,   1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 17
		 1.5f,  5.0f, -15.0f,   1.0f, 2.0f,  0.0f, 0.0f, 1.0f,  // 18
		-1.5f,  5.0f, -15.0f,   0.0f, 2.0f,  0.0f, 0.0f, 1.0f,  // 19
		
		// Cara lateral trasera (z = 15.0)
		-1.5f,  0.0f,  15.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,  // 20
		 1.5f,  0.0f,  15.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,  // 21
		 1.5f,  5.0f,  15.0f,   1.0f, 2.0f,   0.0f, 0.0f, -1.0f,  // 22
		-1.5f,  5.0f,  15.0f,   0.0f, 2.0f,   0.0f, 0.0f, -1.0f   // 23
	};

	// Crear el mesh
	Mesh* paredMesh = new Mesh();
	paredMesh->CreateMesh(paredVertices, paredIndices, 192, 36);
	loadMesh(AssetConstants::MeshNames::CANCHA_PARED, paredMesh);
}

// Crear mesh de techo triangular (prisma con base de triángulo rectángulo)
void MeshManager::createCanchaTechoMesh()
{
	// Prisma triangular: 30 unidades de largo (Z), base de 3 unidades, altura de 3 unidades
	// Triángulo rectángulo: un lado vertical y uno horizontal
	// CORRECCIÓN: Hipotenusa va de (0, 8) a (3, 5), con tapas triangulares y normales invertidas
	unsigned int techoIndices[] = {
		// Cara inclinada (hipotenusa)
		0, 2, 1,
		0, 3, 2,
		// Cara vertical (cateto vertical)
		4, 6, 5,
		4, 7, 6,
		// Cara horizontal (cateto horizontal - base)
		8, 10, 9,
		8, 11, 10,
		// Tapa triangular frontal (Z-)
		12, 13, 14,
		// Tapa triangular trasera (Z+)
		15, 16, 17
	};

	GLfloat techoVertices[] = {
		// Cara inclinada (hipotenusa) 
		//    x      y      z        u     v      nx      ny      nz
		 0.0f,  8.0f, -15.0f,   0.0f, 0.0f,  0.707f, -0.707f, 0.0f,  // 0
		 0.0f,  8.0f,  15.0f,   10.0f, 0.0f, 0.707f, -0.707f, 0.0f,  // 1
		 3.0f,  5.0f,  15.0f,   10.0f, 1.5f, 0.707f, -0.707f, 0.0f,  // 2
		 3.0f,  5.0f, -15.0f,   0.0f, 1.5f,  0.707f, -0.707f, 0.0f,  // 3
		
		// Cara vertical 
		 0.0f,  5.0f, -15.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 4
		 0.0f,  5.0f,  15.0f,   10.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 5
		 0.0f,  8.0f,  15.0f,   10.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // 6
		 0.0f,  8.0f, -15.0f,   0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 7
		
		// Cara horizontal 
		 0.0f,  5.0f, -15.0f,   0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // 8
		 0.0f,  5.0f,  15.0f,   10.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // 9
		 3.0f,  5.0f,  15.0f,   10.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 10
		 3.0f,  5.0f, -15.0f,   0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // 11
		
		// Tapa triangular frontal 
		 0.0f,  5.0f, -15.0f,   0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 12
		 3.0f,  5.0f, -15.0f,   1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 13
		 0.0f,  8.0f, -15.0f,   0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // 14
		
		// Tapa triangular trasera
		 0.0f,  5.0f,  15.0f,   0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  // 15
		 3.0f,  5.0f,  15.0f,   1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  // 16
		 0.0f,  8.0f,  15.0f,   0.0f, 1.0f,  0.0f, 0.0f, -1.0f   // 17
	};

	// Crear el mesh
	Mesh* techoMesh = new Mesh();
	techoMesh->CreateMesh(techoVertices, techoIndices, 144, 24);
	loadMesh(AssetConstants::MeshNames::CANCHA_TECHO, techoMesh);
}

// Crear mesh de toroide para el aro de la cancha de pelota maya
void MeshManager::createToroideMesh()
{
	// Parámetros del toroide
	float mainRadius = 1.0f;    // Radio del círculo principal
	float tubeRadius = 0.2f;    // Radio del tubo
	int mainSegments = 32;      // Segmentos del círculo principal
	int tubeSegments = 16;      // Segmentos del tubo
	
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	
	// Por cada segmento del círculo principal
	for (int i = 0; i <= mainSegments; i++)
	{
		// Ángulo alrededor del círculo principal
		float theta = (float)i / mainSegments * 2.0f * 3.14159265f;
		float cosTheta = cos(theta); // Calculamos el coseno de theta antes de el bucle interno
		float sinTheta = sin(theta); // Calculamos el seno de theta antes de el bucle interno

		// Por cada segmento del tubo
		for (int j = 0; j <= tubeSegments; j++)
		{
			// Obtenemos el ángulo alrededor del tubo
			float phi = (float)j / tubeSegments * 2.0f * 3.14159265f;
			float cosPhi = cos(phi); // Coseno de phi
			float sinPhi = sin(phi); // Seno de phi

			// Centro del tubo en el círculo principal (calculado respecto al ángulo del circulo principal actual)
			float cx = mainRadius * cosTheta;
			float cz = mainRadius * sinTheta;

			// Posición del vértice (usando las fórmulas paramétricas del toroide)
			float x = (mainRadius + tubeRadius * cosPhi) * cosTheta;
			float y = tubeRadius * sinPhi;
			float z = (mainRadius + tubeRadius * cosPhi) * sinTheta;

			// La coordenada de textura u,v actual se calcula normalizando 
			// el índice del segmento actual tanto para el círculo principal como para el tubo
			float u = (float)i / mainSegments;
			float v = (float)j / tubeSegments;

			// Cálculo de las normales del vértice (del centro a la posición de x calculada en la iteración actual.
			// C = (cx, 0, cz) y P = (x, y, z)
			float nx = x - cx; // Componente en x
			float ny = y; // Componente en y
			float nz = z - cz; // Componente en z

			// Normalizar el vector normal 
			float length = sqrt(nx * nx + ny * ny + nz * nz);
			if (length > 0.0f) {
				nx /= length;
				ny /= length;
				nz /= length;
			}

			// Agregar vértice: x, y, z, u, v, nx, ny, nz
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			vertices.push_back(u);
			vertices.push_back(v);
			vertices.push_back(nx);
			vertices.push_back(ny);
			vertices.push_back(nz);
		}
	}

	// Generar índices
	for (int i = 0; i < mainSegments; i++)
	{
		for (int j = 0; j < tubeSegments; j++)
		{
			int first = i * (tubeSegments + 1) + j;
			int second = first + tubeSegments + 1;

			// Primer triángulo
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			// Segundo triángulo
			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}
	
	// Crear el mesh
	Mesh* toroideMesh = new Mesh();
	toroideMesh->CreateMesh(vertices.data(), indices.data(), vertices.size(), indices.size());
	loadMesh(AssetConstants::MeshNames::TOROIDE, toroideMesh);
}

MeshManager::~MeshManager() 
{
}
