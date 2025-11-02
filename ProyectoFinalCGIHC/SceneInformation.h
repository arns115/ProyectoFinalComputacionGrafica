#pragma once

#include <vector>
#include <string>
#include "Entidad.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SkyboxManager.h"
#include "Skybox.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CommonValues.h"

// Clase para gestionar la información de la escena
// Se enfoca en gestión de recursos, entidades e iluminación
class SceneInformation {
public:
    SceneInformation();
    ~SceneInformation();
    
    // Inicializar la escena creando todos los objetos
    void inicializarEscena();
    
    // Agregar una entidad a la escena
    void agregarEntidad(Entidad* entidad);
    
    // Remover una entidad de la escena
    void removerEntidad(Entidad* entidad);
    
    
    
    // Acceso a las entidades
    std::vector<Entidad*>& getEntidades() { return entidades; }
    const std::vector<Entidad*>& getEntidades() const { return entidades; }
    
    // Establecer el skybox actual de la escena
    void setSkyboxActual(const std::string& skyboxName);
    
    // Obtener el skybox actual
    Skybox* getSkyboxActual() { return skyboxActual; }
    const Skybox* getSkyboxActual() const { return skyboxActual; }
    
    // Buscar una entidad por nombre (opcional, útil para debugging)
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
    
    // Managers de recursos
    ModelManager modelManager;
    TextureManager textureManager;
    MeshManager meshManager;
    SkyboxManager skyboxManager;
    
    // Skybox actual de la escena
    Skybox* skyboxActual;
    
    
    // Luz direccional
    DirectionalLight luzDireccional;
    
	// Arrays para todas las luces posibles en la escena
    PointLight pointLights[MAX_LUCES_DIFERENTES];
    SpotLight spotLights[MAX_LUCES_DIFERENTES];
    
    // Arrays para luces actualmente activas 
    PointLight pointLightsActuales[MAX_POINT_LIGHTS];
    SpotLight spotLightsActuales[MAX_SPOT_LIGHTS];
    
    // Contadores de luces activas
    unsigned int pointLightCountActual;
    unsigned int spotLightCountActual;
        
    // Inicializar skybox por defecto
    void inicializarSkybox();
    
    // Inicializar entidades de la escena
    void inicializarEntidades();
    
    // Inicializar sistema de iluminación
    void inicializarLuces();
    
    // Función auxiliar para vincular texturas y modelos a las entidades
    void vincularRecursos(Entidad* entidad);
};
