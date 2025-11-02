#include "SkyboxManager.h"

// Carga todos las skyboxes al inicializar el SkyboxManager
SkyboxManager::SkyboxManager()
{
	loadSkybox(AssetConstants::SkyboxNames::DAY);
	loadSkybox(AssetConstants::SkyboxNames::NIGHT);
}


// Carga las texturas del skybox
void SkyboxManager::loadSkybox(const std::string& skyboxName)
{
	std::string basePath = AssetConstants::SkyboxPaths::SKYBOX_PATH + skyboxName + "/";
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back(basePath + AssetConstants::SkyboxPaths::RIGHT);
	skyboxFaces.push_back(basePath + AssetConstants::SkyboxPaths::LEFT);
	skyboxFaces.push_back(basePath + AssetConstants::SkyboxPaths::BOTTOM);
	skyboxFaces.push_back(basePath + AssetConstants::SkyboxPaths::TOP);
	skyboxFaces.push_back(basePath + AssetConstants::SkyboxPaths::BACK);
	skyboxFaces.push_back(basePath + AssetConstants::SkyboxPaths::FRONT);
	skyboxes[skyboxName] = Skybox(skyboxFaces);
}

// Obtiene un skybox por su nombre
Skybox* SkyboxManager::getSkybox(const std::string& skyboxName)
{
	auto it = skyboxes.find(skyboxName);
	if (it != skyboxes.end()) {
		return &(it->second);
	}
	return nullptr;
}

SkyboxManager::~SkyboxManager()
{
}
