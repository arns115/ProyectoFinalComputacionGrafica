#pragma once

#include <vector>
#include <string>
#include "Entidad.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SkyboxManager.h"
#include "Skybox.h"

// Clase para gestionar la información de la escena
// Se enfoca en gestión de recursos y entidades
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
    
    // Función auxiliar para vincular texturas y modelos a las entidades
    void vincularRecursos(Entidad* entidad);
};
