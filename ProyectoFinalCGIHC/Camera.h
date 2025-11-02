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

	void update();
	void updateThirdPerson();
	void moveThirdPersonTarget(bool* keys, GLfloat deltaTime);
};

