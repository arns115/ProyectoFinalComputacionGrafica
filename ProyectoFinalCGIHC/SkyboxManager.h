#pragma once
#include "AssetConstants.h"
#include "Skybox.h"
#include <map>


// Clase para gestionar el skybox
class SkyboxManager
{
public:
	SkyboxManager();

	void renderSkybox(const std::string& skyboxName, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	~SkyboxManager();

private:
	std::map<std::string, Skybox> skyboxes;

	void loadSkybox(const std::string& skyboxName);
};