#pragma once
#include "Light.h"

// Clase para representar una luz direccional
class DirectionalLight :
	public Light // Hereda de la clase Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
					GLfloat aIntensity, GLfloat dIntensity,
					GLfloat xDir, GLfloat yDir, GLfloat zDir);

	// Método para usar la luz direccional en shaders
	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	~DirectionalLight();

private:
	glm::vec3 direction; // Dirección de la luz
};

