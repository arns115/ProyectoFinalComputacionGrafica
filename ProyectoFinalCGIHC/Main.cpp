#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include "Window.h"
#include "SceneInformation.h"
#include "SceneRenderer.h"
#include "CommonValues.h"

Window mainWindow;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	// Renderizador de la escena
	SceneRenderer sceneRenderer;

	// Crear SceneInformation (gestiona recursos y estado de la escena)
	// Las entidades y luces se crean automáticamente en el constructor
	SceneInformation scene;
	
	// Inicializar el SceneRenderer
	if (!sceneRenderer.inicializar()) {
		return 1;
	}

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	// Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / LIMIT_FPS;
		lastTime = now;

		// Recibir eventos del usuario
		glfwPollEvents();
		
		// Actualizar la escena con input del usuario (cámara, controles, etc.)
		scene.actualizarFrameInput(mainWindow.getsKeys(), 
		                           mainWindow.getXChange(), 
		                           mainWindow.getYChange(),
		                           mainWindow.getScrollChange(),  // Agregar scroll
		                           deltaTime);

		// Actualizar la escena (luces dinámicas, animaciones, etc.)
		scene.actualizarFrame(deltaTime);

		// Renderizar frame completo
		sceneRenderer.renderizarFrame(
			scene.getSkyboxActual(),
			scene.getCamara(),
			projection,
			scene.getEntidades(),
			scene.getLuzDireccional(),
			scene.getPointLightsActuales(),
			scene.getPointLightCountActual(),
			scene.getSpotLightsActuales(),
			scene.getSpotLightCountActual()
		);

		mainWindow.swapBuffers();
	}

	return 0;
}
