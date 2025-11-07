#pragma once
#include "AssetConstants.h"
#include "Mesh.h"
#include "Sphere.h"
#include <map>
#include <glew.h>
#include <glm.hpp>

// Clase para gestionar los meshes
class MeshManager 
{ 
public:

	MeshManager();

	// Método para obtener un mesh 
	Mesh* getMesh(const std::string& meshName);
	

	~MeshManager();

private:	
	// Mapa para almacenar los meshes con su nombre como clave
	std::map<std::string, Mesh*> meshes;

	// Método para cargar un mesh
	void loadMesh(const std::string& meshName, Mesh* mesh);
	
	// Funciones para crear meshes específicos
	void createPiramideMesh();
	void createPisoMesh();
	void createVegetacionMesh();
	void createSphereMesh();
	void createCaminoMesh();
	void createPrismaAguaMesh();
	void createPrismaPequenoMesh();
	void createCanchaParedMesh();
	void createCanchaTechoMesh();
	void createToroideMesh();
	
	// Función auxiliar para calcular normales por promedio de vértices
	void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, 
	                        GLfloat* vertices, unsigned int verticeCount,
	                        unsigned int vLength, unsigned int normalOffset);
};
