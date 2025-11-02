#include "Camera.h"
#include "Entidad.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	// Inicializar modo tercera persona
	thirdPersonMode = false;
	thirdPersonTarget = nullptr;
	thirdPersonDistance = 5.0f;
	thirdPersonHeight = 2.0f;
	thirdPersonMoveSpeed = 1.0f;  // Velocidad por defecto para el personaje
	
	// Inicializar detección de tecla Q
	qKeyPressed = false;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	// Alternar entre cámara libre y tercera persona con la tecla Q
	if (keys[GLFW_KEY_Q]) {
		if (!qKeyPressed) {
			setThirdPersonMode(!thirdPersonMode);
			qKeyPressed = true;
		}
	} else {
		qKeyPressed = false;
	}

	// Si está en tercera persona, mover el personaje en lugar de la cámara
	if (thirdPersonMode && thirdPersonTarget != nullptr) {
		moveThirdPersonTarget(keys, deltaTime);
		return;
	}

	// Modo cámara libre: mover la cámara normalmente
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}

	if (keys[GLFW_KEY_SPACE]) {
		position += up * velocity;
	}

	if (keys[GLFW_KEY_LEFT_CONTROL]) {
		position -= up * velocity;
	}
}

void Camera::moveThirdPersonTarget(bool* keys, GLfloat deltaTime)
{
	if (thirdPersonTarget == nullptr) {
		return;
	}

	GLfloat velocity = thirdPersonMoveSpeed * deltaTime;
	glm::vec3 movement(0.0f);

	// Calcular dirección de movimiento basada en la orientación de la cámara
	// Proyectar el vector front en el plano XZ (ignorar componente Y)
	glm::vec3 forwardFlat = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
	glm::vec3 rightFlat = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

	// Movimiento adelante/atrás
	if (keys[GLFW_KEY_W])
	{
		movement += forwardFlat * velocity;
	}

	if (keys[GLFW_KEY_S])
	{
		movement -= forwardFlat * velocity;
	}

	// Movimiento lateral
	if (keys[GLFW_KEY_A])
	{
		movement -= rightFlat * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		movement += rightFlat * velocity;
	}

	// Movimiento vertical (opcional)
	if (keys[GLFW_KEY_SPACE])
	{
		movement.y += velocity;
	}

	if (keys[GLFW_KEY_LEFT_CONTROL])
	{
		movement.y -= velocity;
	}

	// Aplicar el movimiento al personaje
	if (glm::length(movement) > 0.0f) {
		thirdPersonTarget->posicionLocal += movement;
		thirdPersonTarget->actualizarTransformacion();

		// Solo rotar el personaje si hay movimiento horizontal (X o Z)
		// Esto evita valores NaN cuando solo hay movimiento en Y
		glm::vec3 horizontalMovement(movement.x, 0.0f, movement.z);
		float horizontalLength = glm::length(horizontalMovement);
		
		if (horizontalLength > 0.001f) {  // Umbral pequeño para evitar divisiones por cero
			glm::vec3 moveDir = glm::normalize(horizontalMovement);
			float targetYaw = glm::degrees(atan2(moveDir.x, moveDir.z));
			thirdPersonTarget->rotacionLocal = glm::vec3(
				thirdPersonTarget->rotacionLocal.x,
				targetYaw,
				thirdPersonTarget->rotacionLocal.z
			);
			thirdPersonTarget->actualizarTransformacion();
		}
		// Si solo hay movimiento en Y, no rotamos el personaje
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	// Si está en modo tercera persona, actualizar la posición de la cámara
	if (thirdPersonMode && thirdPersonTarget != nullptr) {
		updateThirdPerson();
	}

	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

void Camera::setThirdPersonMode(bool enable)
{
	thirdPersonMode = enable;
}

void Camera::setThirdPersonTarget(Entidad* target)
{
	thirdPersonTarget = target;
}

void Camera::setThirdPersonDistance(float distance)
{
	thirdPersonDistance = distance;
}

void Camera::setThirdPersonHeight(float height)
{
	thirdPersonHeight = height;
}

void Camera::setThirdPersonMoveSpeed(float speed)
{
	thirdPersonMoveSpeed = speed;
}

bool Camera::isThirdPersonMode() const
{
	return thirdPersonMode;
}

void Camera::updateThirdPerson()
{
	if (thirdPersonTarget == nullptr) {
		return;
	}

	// Obtener la posición del objetivo
	glm::vec3 targetPos = thirdPersonTarget->posicionLocal;
	glm::vec3 targetRot = thirdPersonTarget->rotacionLocal;

	// Calcular la dirección de la cámara basada en yaw y pitch
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);

	// Posicionar la cámara detrás del objetivo
	position = targetPos - direction * thirdPersonDistance;
	position.y += thirdPersonHeight;

	// Hacer que la cámara mire hacia el objetivo
	front = glm::normalize(targetPos + glm::vec3(0.0f, thirdPersonHeight * 0.5f, 0.0f) - position);
	
	// Actualizar los vectores right y up
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

Camera::~Camera()
{
}
