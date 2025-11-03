#include "LightManager.h"

// Carga todas las luces predefinidas al inicializar el LightManager
LightManager::LightManager()
{
	// Cargar luces direccionales predefinidas
	loadDirectionalLight(AssetConstants::LightNames::SOL,
		1.0f, 1.0f, 1.0f,           // Color blanco
		0.3f, 0.5f,
		0.0f, -1.0f, 0.0f);



	// Cargar luces puntuales predefinidas
	loadPointLight(AssetConstants::LightNames::PUNTUAL_ROJA,
		1.0f, 0.0f, 0.0f,                 // Rojo
		0.3f, 1.0f,
		-5.0f, 2.0f, 0.0f,
		0.3f, 0.2f, 0.1f);


	// Cargar spotlights predefinidos
	loadSpotLight(AssetConstants::LightNames::LINTERNA,
		1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);

}


// Obtiene una luz direccional por su nombre
DirectionalLight* LightManager::getDirectionalLight(const std::string& lightName)
{
	auto it = directionalLights.find(lightName);
	if (it != directionalLights.end()) {
		return &it->second;
	}
	return nullptr;
}

// Obtiene una luz puntual por su nombre
PointLight* LightManager::getPointLight(const std::string& lightName)
{
	auto it = pointLights.find(lightName);
	if (it != pointLights.end()) {
		return &it->second;
	}
	return nullptr;
}

// Obtiene un spotlight por su nombre
SpotLight* LightManager::getSpotLight(const std::string& lightName)
{
	auto it = spotLights.find(lightName);
	if (it != spotLights.end()) {
		return &it->second;
	}
	return nullptr;
}

// Carga una luz direccional en el manager
void LightManager::loadDirectionalLight(const std::string& lightName, 
                                       GLfloat red, GLfloat green, GLfloat blue,
                                       GLfloat aIntensity, GLfloat dIntensity,
                                       GLfloat xDir, GLfloat yDir, GLfloat zDir)
{
	DirectionalLight light(red, green, blue, aIntensity, dIntensity, xDir, yDir, zDir);
	directionalLights[lightName] = light;
}

// Carga una luz puntual en el manager
void LightManager::loadPointLight(const std::string& lightName, 
                                 GLfloat red, GLfloat green, GLfloat blue,
                                 GLfloat aIntensity, GLfloat dIntensity,
                                 GLfloat xPos, GLfloat yPos, GLfloat zPos,
                                 GLfloat con, GLfloat lin, GLfloat exp)
{
	PointLight light(red, green, blue, aIntensity, dIntensity, xPos, yPos, zPos, con, lin, exp);
	pointLights[lightName] = light;
}

// Carga un spotlight en el manager
void LightManager::loadSpotLight(const std::string& lightName, 
                                GLfloat red, GLfloat green, GLfloat blue,
                                GLfloat aIntensity, GLfloat dIntensity,
                                GLfloat xPos, GLfloat yPos, GLfloat zPos,
                                GLfloat xDir, GLfloat yDir, GLfloat zDir,
                                GLfloat con, GLfloat lin, GLfloat exp,
                                GLfloat edg)
{
	SpotLight light(red, green, blue, aIntensity, dIntensity, 
	               xPos, yPos, zPos, xDir, yDir, zDir, 
	               con, lin, exp, edg);
	spotLights[lightName] = light;
}

LightManager::~LightManager() 
{
}
