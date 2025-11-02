#pragma once
#include "AssetConstants.h"
#include "Mesh.h"
#include <map>

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



};
