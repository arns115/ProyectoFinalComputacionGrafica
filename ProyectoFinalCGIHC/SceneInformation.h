#pragma once

#include <vector>
#include <string>
#include "Entidad.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SkyboxManager.h"
#include "MaterialManager.h"
#include "LightManager.h"
#include "Skybox.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CommonValues.h"
#include "Camera.h"

// Clase para gestionar la información de la escena
// Se enfoca en gestión de recursos, entidades e iluminación
class SceneInformation {
public:
    SceneInformation();
    ~SceneInformation();
    
	// Variables para luces actuales (para tomar la luz y meterla al arreglo)
    SpotLight spotLightActual;
    PointLight pointLightActual;
    
    // Variables para manejar posicion y direccion de luz que se agregara a luces actuales 
	glm::vec3 posicionLuzActual;
	glm::vec3 direccionLuzActual;


    // Inicializar la cámara con parámetros personalizados
    void inicializarCamara(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f),
                          glm::vec3 startUp = glm::vec3(0.0f, 1.0f, 0.0f),
                          GLfloat startYaw = -60.0f,
                          GLfloat startPitch = 0.0f,
                          GLfloat startMoveSpeed = 0.3f,
                          GLfloat startTurnSpeed = 0.5f);
    
    // Actualizar la escena cada frame (luces dinámicas, animaciones, etc.)
    void actualizarFrame(float deltaTime);

    // Actualizar la escena cada frame dependiendo del input del usuario
    void actualizarFrameInput(bool* keys, GLfloat mouseXChange, GLfloat mouseYChange, GLfloat scrollChange, float deltaTime);

    // Agregar una entidad a la escena
    void agregarEntidad(Entidad* entidad);
    
    // Remover una entidad de la escena
    void removerEntidad(Entidad* entidad);
    
    
    // Acceso a la cámara
    Camera& getCamara() { return camera; }
    const Camera& getCamara() const { return camera; }
    
    // Acceso a las entidades
    std::vector<Entidad*>& getEntidades() { return entidades; }
    const std::vector<Entidad*>& getEntidades() const { return entidades; }
    
    // Establecer el skybox actual de la escena
    void setSkyboxActual(const std::string& skyboxName);
    
    // Obtener el skybox actual
    Skybox* getSkyboxActual() { return skyboxActual; }
    const Skybox* getSkyboxActual() const { return skyboxActual; }
    
    // Buscar una entidad por nombre 
    Entidad* buscarEntidad(const std::string& nombre);

    // Establecer la luz direccional
    void setLuzDireccional(const DirectionalLight& light);
    DirectionalLight* getLuzDireccional() { return &luzDireccional; }
    const DirectionalLight* getLuzDireccional() const { return &luzDireccional; }
    
    // Agregar luces puntuales a las luces actuales 
    bool agregarLuzPuntualActual(const PointLight& light);
    void limpiarLucesPuntualesActuales();
    
    // Agregar spotlights a las luces actuales 
    bool agregarSpotLightActual(const SpotLight& light);
    void limpiarSpotLightsActuales();
    
    // Obtener arrays de luces actualmente activas
    PointLight* getPointLightsActuales() { return pointLightsActuales; }
    const PointLight* getPointLightsActuales() const { return pointLightsActuales; }
    
    SpotLight* getSpotLightsActuales() { return spotLightsActuales; }
    const SpotLight* getSpotLightsActuales() const { return spotLightsActuales; }
    
    // Obtener contadores de luces actuales
    unsigned int getPointLightCountActual() const { return pointLightCountActual; }
    unsigned int getSpotLightCountActual() const { return spotLightCountActual; }
    
private:
    // Vector con todas las entidades de la escena
    std::vector<Entidad*> entidades;
    
    // Cámara de la escena
    Camera camera;
    
    // Managers de recursos
    ModelManager modelManager;
    TextureManager textureManager;
    MeshManager meshManager;
    SkyboxManager skyboxManager;
    MaterialManager materialManager;
    LightManager lightManager;
    
    // Skybox actual de la escena
    Skybox* skyboxActual;
    
    // Luz direccional
    DirectionalLight luzDireccional;
    
    // Arrays para luces actualmente activas 
    PointLight pointLightsActuales[MAX_POINT_LIGHTS];
    SpotLight spotLightsActuales[MAX_SPOT_LIGHTS];
    
    // Contadores de luces activas
    unsigned int pointLightCountActual;
    unsigned int spotLightCountActual;

    // Booleano para saber si es de dia
    bool esDeDia = false;
	// Acumulador de tiempo para cambiar entre dia y noche (a los 2 minutos se cambia)
	GLfloat acumuladorTiempoDesdeCambio = 0.0f;

    // Entero para saber que personaje es actualmente
	int personajeActual = 1; // 1: Cuphead, 2: Isaac, 3: Gojo


    //Funciones para inicializar componentes de la escena
    

    // Inicializar skybox por defecto
    void inicializarSkybox();
    
    // Inicializar luces de la escena
    void inicializarLuces();
    
    // Inicializar entidades de la escena
    void inicializarEntidades();
    
    // Funciones para crear entidades específicas
    void crearPersonajePrincipal();
    void crearPiso();
    void crearObjetosGeometricos(); 
    void crearIsaac();
    void crearCabezaOlmeca();
    void crearHollow();
	void crearBossRoom();
    void crearPiramide();
    void crearCamino();
    void crearPrismaAgua();
    void crearPrismasPequenos();
    void crearSecretRoom();
    void crearFogatas();
	void crearComidaPerro();
    void crearRKey();
    
    // Función auxiliar para vincular texturas y modelos a las entidades
    void vincularRecursos(Entidad* entidad);
};
