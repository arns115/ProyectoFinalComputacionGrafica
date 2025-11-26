#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

// Forward declaration
class Entidad;

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);
	void mouseScrollControl(GLfloat yOffset);  // Nuevo método para zoom con scroll

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();

	// Métodos para tercera persona
	void setThirdPersonMode(bool enable);
	void setThirdPersonTarget(Entidad* target);
	void setThirdPersonDistance(float distance);
	void setThirdPersonHeight(float height);
	void setThirdPersonMoveSpeed(float speed);
	bool isThirdPersonMode() const;

	// Métodos para vista aérea
	void setAerialViewMode(bool enable);
	void setAerialViewHeight(float height);
	void setAerialViewCenter(glm::vec3 center);
	bool isAerialViewMode() const;
	
	// Método para establecer modo de cámara libre
	void setFreeCameraMode(bool enable);
	bool isFreeCameraMode() const;

	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	// Variables para tercera persona
	bool thirdPersonMode;
	Entidad* thirdPersonTarget;
	float thirdPersonDistance;
	float thirdPersonHeight;
	float thirdPersonMoveSpeed;

	// Variable para detectar pulsación de tecla Q
	bool qKeyPressed;

	// Nivel del suelo para física
	float groundLevel;
	
	// Variables para detección de teclas de cambio de modo
	bool key8Pressed;  // Tecla 8 para cámara libre
	bool key9Pressed;  // Tecla 9 para tercera persona
	bool key0Pressed;  // Tecla 0 para vista aérea (renombrado)

	// Variables para detección de teclas de teletransportación
	bool keyHPressed;  // Tecla H para teletransporte a pirámide
	bool keyJPressed;  // Tecla J para teletransporte a cancha
	bool keyKPressed;  // Tecla K para teletransporte a chinampas
	bool keyLPressed;  // Tecla L para teletransporte a boss room

	// Variables para vista aérea
	bool aerialViewMode;
	float aerialViewHeight;
	glm::vec3 aerialViewCenter;

	// Variables para movimiento de cámara aérea
	GLfloat aerialYaw;
	GLfloat aerialPitch;
	GLfloat aerialMoveSpeed;
	
	// Variables para guardar estado anterior
	glm::vec3 savedPosition;
	glm::vec3 savedFront;
	glm::vec3 savedUp;
	glm::vec3 savedRight;
	GLfloat savedYaw;
	GLfloat savedPitch;
	bool savedThirdPersonMode;

	void update();
	void updateThirdPerson();
	void updateAerialView();
	void saveCurrentState();
	void restoreState();
	void moveThirdPersonTarget(bool* keys, GLfloat deltaTime);
	void teleportToLocation(glm::vec3 position, GLfloat yaw, GLfloat pitch);
};

