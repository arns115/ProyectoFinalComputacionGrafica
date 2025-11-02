#include "SceneInformation.h"

SceneInformation::SceneInformation()
    : skyboxActual(nullptr), pointLightCountActual(0), spotLightCountActual(0)
{
    // Llamar a las funciones de inicialización separadas
    inicializarSkybox();
    inicializarEntidades();
    inicializarLuces();
}

SceneInformation::~SceneInformation()
{
    // Liberar todas las entidades
    for (auto* entidad : entidades) {
        delete entidad;
    }
    entidades.clear();

    // Limpiar referencia al skybox
    skyboxActual = nullptr;

}





void SceneInformation::inicializarSkybox()
{
    // Establecer el skybox por defecto
    setSkyboxActual(AssetConstants::SkyboxNames::DAY);
}

void SceneInformation::inicializarEntidades()
{
    // Aquí se pueden crear entidades iniciales de la escena
    // Por ahora, la escena comienza vacía
}

void SceneInformation::inicializarLuces()
{
    // Inicializar luz direccional por defecto (blanca, suave, apuntando hacia abajo)
    luzDireccional = DirectionalLight(1.0f, 1.0f, 1.0f,
                                     0.3f, 0.3f,
                                     0.0f, -1.0f, 0.0f);
    

}

void SceneInformation::agregarEntidad(Entidad* entidad)
{
    if (entidad != nullptr) {
        // Vincular recursos (modelos, meshes y texturas) antes de agregar
        vincularRecursos(entidad);
        entidades.push_back(entidad);
    }
}

void SceneInformation::removerEntidad(Entidad* entidad)
{
    if (entidad == nullptr) return;
    
    // Buscar y remover la entidad del vector
    auto it = std::find(entidades.begin(), entidades.end(), entidad);
    if (it != entidades.end()) {
        entidades.erase(it);
    }
    // Nota: NO se elimina la entidad, solo se elimina del vector
}

Entidad* SceneInformation::buscarEntidad(const std::string& nombre)
{
    // Buscar una entidad por nombre de objeto
    for (auto* entidad : entidades) {
        if (entidad != nullptr) {
            // Buscar primero por nombreObjeto si existe
            if (!entidad->nombreObjeto.empty() && entidad->nombreObjeto == nombre) {
                return entidad;
            }
        }
    }
    return nullptr;
}


void SceneInformation::setSkyboxActual(const std::string& skyboxName)
{
    skyboxActual = skyboxManager.getSkybox(skyboxName);
}

void SceneInformation::setLuzDireccional(const DirectionalLight& light)
{
    luzDireccional = light;
}

bool SceneInformation::agregarLuzPuntualActual(const PointLight& light)
{
    if (pointLightCountActual >= MAX_POINT_LIGHTS) {
        return false; // No hay espacio para más luces actuales
    }
    
    // Agregar al arreglo de luces actuales
    pointLightsActuales[pointLightCountActual] = light;
    pointLightCountActual++;
    
    return true;
}

void SceneInformation::limpiarLucesPuntualesActuales()
{
    pointLightCountActual = 0;
}

bool SceneInformation::agregarSpotLightActual(const SpotLight& light)
{
    if (spotLightCountActual >= MAX_SPOT_LIGHTS) {
        return false; // No hay espacio para más luces actuales
    }
    
    // Agregar solo al arreglo de luces actuales
    spotLightsActuales[spotLightCountActual] = light;
    spotLightCountActual++;
    
    return true;
}

void SceneInformation::limpiarSpotLightsActuales()
{
    spotLightCountActual = 0;
}

void SceneInformation::vincularRecursos(Entidad* entidad)
{
    if (entidad == nullptr) return;
    
    // Vincular modelo si la entidad usa un modelo
    if (entidad->getTipoGeometria() == TipoGeometria::MODELO) {
        std::string modelName = entidad->nombreModelo;
        if (!modelName.empty()) {
            Model* modelo = modelManager.getModel(modelName);
            if (modelo != nullptr) {
                entidad->setModelo(modelName, modelo);
            }
        }
    }
    
    // Vincular mesh si la entidad usa un mesh
    if (entidad->getTipoGeometria() == TipoGeometria::MESH) {
        std::string meshName = entidad->nombreMesh;
        if (!meshName.empty()) {
            Mesh* mesh = meshManager.getMesh(meshName);
            if (mesh != nullptr) {
                entidad->setMesh(meshName, mesh);
            }
        }
    }
    
    // Vincular textura si la entidad tiene un nombre de textura
    std::string textureName = entidad->nombreTextura;
    if (!textureName.empty()) {
        Texture* textura = textureManager.getTexture(textureName);
        if (textura != nullptr) {
            entidad->setTextura(textureName, textura);
        }
    }
    
    // Vincular recursos recursivamente para todas las entidades hijas
    for (auto* hijo : entidad->hijos) {
        vincularRecursos(hijo);
    }
}
