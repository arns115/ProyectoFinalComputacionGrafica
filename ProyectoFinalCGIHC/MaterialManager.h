#pragma once
#include "AssetConstants.h"
#include "Material.h"
#include <map>

// Clase para gestionar los materiales
class MaterialManager 
{ 
public:

	MaterialManager();

	// Método para obtener un material 
	Material* getMaterial(const std::string& materialName);
	

	~MaterialManager();

private:	
	// Mapa para almacenar los materiales con su nombre como clave
	std::map<std::string, Material*> materials;

	void loadMaterial(const std::string& materialName, GLfloat specularIntensity, GLfloat shininess);

};
