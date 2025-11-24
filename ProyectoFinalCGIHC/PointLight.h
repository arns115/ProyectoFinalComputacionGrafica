#pragma once
#include "Light.h"

// Clase para representar una luz puntual
class PointLight :
	public Light // Hereda de la clase Light
{
public:
	PointLight();
	PointLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat con, GLfloat lin, GLfloat exp);

	// Método para usar la luz puntual en shaders
	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
		GLfloat diffuseIntensityLocation, GLfloat positionLocation,
		GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation);

	// Obtener la posición de la luz
	glm::vec3 GetPosition() const { return position; }

	// Settear posicion de la luz
	void setPosition(const glm::vec3& pos) { position = pos; }
	~PointLight();

protected:
	glm::vec3 position;

	GLfloat constant, linear, exponent;
};

