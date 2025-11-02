#pragma once
#include "AssetConstants.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <map>

// Clase para gestionar todas las luces posibles
class LightManager 
{ 
public:
	LightManager();

	// Métodos para obtener luces por nombre
	DirectionalLight* getDirectionalLight(const std::string& lightName);
	PointLight* getPointLight(const std::string& lightName);
	SpotLight* getSpotLight(const std::string& lightName);

	~LightManager();

private:	
	// Mapas para almacenar todas las luces posibles
	std::map<std::string, DirectionalLight> directionalLights;
	std::map<std::string, PointLight> pointLights;
	std::map<std::string, SpotLight> spotLights;

	// Funciones para cargar luces predefinidas
	void loadDirectionalLight(const std::string& lightName, GLfloat red, GLfloat green, GLfloat blue,
	                          GLfloat aIntensity, GLfloat dIntensity,
	                          GLfloat xDir, GLfloat yDir, GLfloat zDir);
	
	void loadPointLight(const std::string& lightName, GLfloat red, GLfloat green, GLfloat blue,
	                   GLfloat aIntensity, GLfloat dIntensity,
	                   GLfloat xPos, GLfloat yPos, GLfloat zPos,
	                   GLfloat con, GLfloat lin, GLfloat exp);
	
	void loadSpotLight(const std::string& lightName, GLfloat red, GLfloat green, GLfloat blue,
	                  GLfloat aIntensity, GLfloat dIntensity,
	                  GLfloat xPos, GLfloat yPos, GLfloat zPos,
	                  GLfloat xDir, GLfloat yDir, GLfloat zDir,
	                  GLfloat con, GLfloat lin, GLfloat exp,
	                  GLfloat edg);
};
