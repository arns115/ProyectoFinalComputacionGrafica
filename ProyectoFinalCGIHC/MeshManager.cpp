#include "MeshManager.h"

// Inicializa el MeshManager
// Se cargan todos los meshes 
MeshManager::MeshManager() 
{

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

MeshManager::~MeshManager() 
{
}
