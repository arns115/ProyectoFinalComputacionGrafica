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

// Renderiza un skybox por su nombre
void SkyboxManager::renderSkybox(const std::string& skyboxName, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	skyboxes[skyboxName].DrawSkybox(viewMatrix, projectionMatrix);
}

SkyboxManager::~SkyboxManager()
{
}
