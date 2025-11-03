#pragma once
#include "AssetConstants.h"
#include "Skybox.h"
#include <map>


// Clase para gestionar el skybox
class SkyboxManager
{
public:
	SkyboxManager();

	// Método para obtener un skybox por nombre 
	Skybox* getSkybox(const std::string& skyboxName);

	~SkyboxManager();

private:
	// Mapa para almacenar los skyboxes con su nombre como clave
	std::map<std::string, Skybox> skyboxes;

	void loadSkybox(const std::string& skyboxName);
};