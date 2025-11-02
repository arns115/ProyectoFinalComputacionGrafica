#include "SceneInformation.h"

SceneInformation::SceneInformation()
{
    // Constructor vacío - los managers se inicializan automáticamente
}

SceneInformation::~SceneInformation()
{
    for (auto* entidad : entidades) {
        delete entidad;
    }
    entidades.clear();
}

void SceneInformation::inicializarEscena()
{

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
    // Nota: NO se elimina la entidad, solo se remueve del vector
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
