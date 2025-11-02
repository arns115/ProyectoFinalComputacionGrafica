#include "MaterialManager.h"

// Carga todos los materiales al inicializar el MaterialManager
MaterialManager::MaterialManager() 
{
	// Cargar materiales predefinidos
	loadMaterial(AssetConstants::MaterialNames::BRILLANTE, 4.0f, 256.0f);
	loadMaterial(AssetConstants::MaterialNames::OPACO, 0.3f, 4.0f);
}

// Obtiene un material por su nombre 
Material* MaterialManager::getMaterial(const std::string& materialName)
{
	auto it = materials.find(materialName);
	if (it != materials.end()) {
		return it->second;
	}
	return nullptr;
}

// Carga un material en el manager
void MaterialManager::loadMaterial(const std::string& materialName, GLfloat specularIntensity, GLfloat shininess)
{
	Material* material = new Material(specularIntensity, shininess);
	materials[materialName] = material;
}

MaterialManager::~MaterialManager() 
{
}
