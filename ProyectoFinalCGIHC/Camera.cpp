#include "Camera.h"
#include "Entidad.h"
#include "ComponenteFisico.h"
#include "ComponenteAnimacion.h"

// TODO: Hacer una clases para la cámara aérea y la cámara en tercera persona que hereden de Camera

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

	// Inicializar nivel del suelo
	groundLevel = 1.2f;
	
	// Inicializar detección de teclas de modo de cámara
	key8Pressed = false;
	key9Pressed = false;
	key0Pressed = false;

	// Inicializar modo vista aérea
	aerialViewMode = false;
	aerialViewHeight = 100.0f;  // Altura más alta para mejor vista del escenario
	aerialViewCenter = glm::vec3(0.0f, 0.0f, 0.0f);  // Centro de la escena
	//zeroKeyPressed = false;
	
	// Inicializar movimiento de cámara aérea
	aerialYaw = 0.0f;
	aerialPitch = -90.0f;  // Mirando directamente hacia abajo
	aerialMoveSpeed = 1.0f;  // Velocidad muy reducida de movimiento en vista aérea

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	// Alternar entre modos de cámara con teclas 8, 9 y 0
	
	// Tecla 8: Cámara Libre
	if (keys[GLFW_KEY_8]) {
		if (!key8Pressed) {
			setFreeCameraMode(true);
			key8Pressed = true;
		}
	} else {
		key8Pressed = false;
	}
	
	// Tecla 9: Tercera Persona
	if (keys[GLFW_KEY_9]) {
		if (!key9Pressed) {
			setThirdPersonMode(true);
			key9Pressed = true;
		}
	} else {
		key9Pressed = false;
	}
	
	// Tecla 0: Vista Aérea
	if (keys[GLFW_KEY_0]) {
		if (!key0Pressed) {
			setAerialViewMode(true);
			key0Pressed = true;
		}
	} else {
		key0Pressed = false;
	}

	// Si está en vista aérea, permitir movimiento limitado
	if (aerialViewMode) {
		GLfloat velocity = aerialMoveSpeed * deltaTime;
		
		// Movimiento horizontal (WASD) - Solo en plano XZ
		if (keys[GLFW_KEY_W])
		{
			aerialViewCenter.z -= velocity;  // Mover hacia adelante (norte)
		}

		if (keys[GLFW_KEY_S])
		{
			aerialViewCenter.z += velocity;  // Mover hacia atrás (sur)
		}

		if (keys[GLFW_KEY_A])
		{
			aerialViewCenter.x -= velocity;  // Mover a la izquierda 
		}

		if (keys[GLFW_KEY_D])
		{
			aerialViewCenter.x += velocity;  // Mover a la derecha
		}
		
		return;
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

	// Salto con Space
	if (keys[GLFW_KEY_SPACE])
	{
		if (thirdPersonTarget->fisica->estaEnSuelo()) {
			// Activar física de salto
			thirdPersonTarget->fisica->saltar(8.0f);
			
			// Activar animación de salto automáticamente
			if (thirdPersonTarget->animacion != nullptr) {
				thirdPersonTarget->animacion->activarAnimacion(1); // Índice 1 = salto
			}
		}
	}

	// Verificar si hay animación de salto activa
	bool enSalto = false;
	if (thirdPersonTarget->animacion != nullptr) {
		enSalto = thirdPersonTarget->animacion->estaActiva(1); // Índice 1 = animación de salto
	}

	// Aplicar el movimiento horizontal al personaje (NO vertical, eso lo maneja la física)
	if (glm::length(movement) > 0.0f) {
		// Solo aplicar movimiento horizontal
		thirdPersonTarget->posicionLocal.x += movement.x;
		thirdPersonTarget->posicionLocal.z += movement.z;

		// Rotación del personaje basada en dirección de movimiento
		glm::vec3 horizontalMovement(movement.x, 0.0f, movement.z);
		float horizontalLength = glm::length(horizontalMovement);
		
		if (horizontalLength > 0.001f) {  // Umbral pequeño para evitar divisiones por cero
			glm::vec3 moveDir = glm::normalize(horizontalMovement);
			float targetYaw = glm::degrees(atan2(moveDir.x, moveDir.z));
			// Actualizar rotación Y del personaje para que mire en la dirección de movimiento
			thirdPersonTarget->rotacionLocalQuat = glm::quat(1, 0, 0, 0);
			thirdPersonTarget->rotacionLocal = glm::vec3(
				thirdPersonTarget->rotacionInicial.x,
				thirdPersonTarget->rotacionInicial.y + targetYaw, 
				thirdPersonTarget->rotacionInicial.z
			);
		}
	}
	
	// Actualizar transformación
	thirdPersonTarget->actualizarTransformacion();

	// Calcular velocidad de movimiento para animación
	float velocidadMovimiento = glm::length(movement);
	
	// Aplicar la fisica al personaje(basicamente gravedad)
	if (thirdPersonTarget->fisica != nullptr) {
		thirdPersonTarget->fisica->aplicarFisica(deltaTime, groundLevel, 
			thirdPersonTarget->posicionLocal, thirdPersonTarget->posicionInicial);
	}
	
	// Actualizar animación del personaje activo
	if (thirdPersonTarget->animacion != nullptr) {
		// Animación de caminata (índice 0)
		if (!enSalto) {
			thirdPersonTarget->animacion->actualizarAnimacion(0, deltaTime, velocidadMovimiento);
		}
		// Animación de salto (índice 1)
		if (enSalto) {
			thirdPersonTarget->animacion->actualizarAnimacion(1, deltaTime, 0.0f);
		}
	}
	
	// Actualizar transformación
	thirdPersonTarget->actualizarTransformacion();
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	// En vista aérea, permitir rotación limitada
	if (aerialViewMode) {
		xChange *= turnSpeed * 0.5f;  // Reducir sensibilidad en vista aérea
		yChange *= turnSpeed * 0.5f;
		
		aerialYaw += xChange;
		aerialPitch += yChange;
		
		// Limitar pitch a 90 grados (de -90 a 0 grados)
		// -90 = mirando directamente hacia abajo
		// 0 = mirando horizontalmente
		if (aerialPitch > 0.0f) {
			aerialPitch = 0.0f;
		}
		if (aerialPitch < -90.0f) {
			aerialPitch = -90.0f;
		}
		
		// Normalizar yaw entre -180 y 180
		while (aerialYaw > 180.0f) aerialYaw -= 360.0f;
		while (aerialYaw < -180.0f) aerialYaw += 360.0f;
		
		return;
	}

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

void Camera::mouseScrollControl(GLfloat yOffset)
{
	// Solo aplicar zoom en vista aérea
	if (aerialViewMode) {
		// Ajustar altura con la rueda del mouse
		// yOffset positivo = scroll hacia arriba = alejar (aumentar altura)
		// yOffset negativo = scroll hacia abajo = acercar (disminuir altura)
		GLfloat zoomSpeed = 5.0f;
		aerialViewHeight -= yOffset * zoomSpeed;
		
		// Limitar altura mínima y máxima
		if (aerialViewHeight < 10.0f) {
			aerialViewHeight = 10.0f;
		}
		if (aerialViewHeight > 200.0f) {
			aerialViewHeight = 200.0f;
		}
	}
}

glm::mat4 Camera::calculateViewMatrix()
{
	// Si está en modo vista aérea, actualizar la posición de la cámara
	if (aerialViewMode) {
		updateAerialView();
	}
	// Si está en modo tercera persona, actualizar la posición de la cámara
	else if (thirdPersonMode && thirdPersonTarget != nullptr) {
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
	if (enable) {
		// Desactivar otros modos
		aerialViewMode = false;
	}
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

void Camera::setFreeCameraMode(bool enable)
{
	if (enable) {
		// Desactivar otros modos
		thirdPersonMode = false;
		aerialViewMode = false;
	}
}

bool Camera::isFreeCameraMode() const
{
	return !thirdPersonMode && !aerialViewMode;
}

void Camera::setAerialViewMode(bool enable)
{
	if (enable && !aerialViewMode) {
		// Desactivar otros modos
		thirdPersonMode = false;
		
		// Guardar el estado actual antes de cambiar a vista aerial
		saveCurrentState();
		aerialViewMode = true;
		updateAerialView();
	}
	else if (!enable && aerialViewMode) {
		// Restaurar el estado previo
		aerialViewMode = false;
		restoreState();
	}
}

void Camera::setAerialViewHeight(float height)
{
	aerialViewHeight = height;
	if (aerialViewMode) {
		updateAerialView();
	}
}

void Camera::setAerialViewCenter(glm::vec3 center)
{
	aerialViewCenter = center;
	if (aerialViewMode) {
		updateAerialView();
	}
}

bool Camera::isAerialViewMode() const
{
	return aerialViewMode;
}

void Camera::updateAerialView()
{
	// Calcular dirección de la cámara basada en aerialYaw y aerialPitch
	glm::vec3 direction;
	direction.x = cos(glm::radians(aerialYaw)) * cos(glm::radians(aerialPitch));
	direction.y = sin(glm::radians(aerialPitch));
	direction.z = sin(glm::radians(aerialYaw)) * cos(glm::radians(aerialPitch));
	direction = glm::normalize(direction);
	
	// Posicionar la cámara encima del centro
	position = aerialViewCenter + glm::vec3(0.0f, aerialViewHeight, 0.0f);
	
	// La cámara mira en la dirección calculada
	front = direction;
	
	// Calcular vectores right y up
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::saveCurrentState()
{
	savedPosition = position;
	savedFront = front;
	savedUp = up;
	savedRight = right;
	savedYaw = yaw;
	savedPitch = pitch;
	savedThirdPersonMode = thirdPersonMode;
	
	// Resetear ángulos de vista aérea al entrar
	aerialYaw = 0.0f;
	aerialPitch = -90.0f;  // Mirando hacia abajo por defecto
}

void Camera::restoreState()
{
	position = savedPosition;
	front = savedFront;
	up = savedUp;
	right = savedRight;
	yaw = savedYaw;
	pitch = savedPitch;
	
	// Restaurar el modo de tercera persona si estaba activo
	if (savedThirdPersonMode && thirdPersonTarget != nullptr) {
		thirdPersonMode = savedThirdPersonMode;
	}
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
	

	// Esto hace que la cámara se mantenga relativa al personaje durante el salto
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
