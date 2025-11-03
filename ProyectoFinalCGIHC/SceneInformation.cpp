#include "SceneInformation.h"

SceneInformation::SceneInformation()
    : skyboxActual(nullptr), pointLightCountActual(0), spotLightCountActual(0)
{
    // Llamar a las funciones de inicialización separadas
    inicializarSkybox();  // Inicializar Skybox
    inicializarLuces();   // Inicializar luces 
    inicializarCamara();  // Inicializar cámara con valores por defecto
    inicializarEntidades();  // Inicializar Enitdades

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

// Function para inicializar la camara
void SceneInformation::inicializarCamara(glm::vec3 startPosition,
                                        glm::vec3 startUp,
                                        GLfloat startYaw,
                                        GLfloat startPitch,
                                        GLfloat startMoveSpeed,
                                        GLfloat startTurnSpeed)
{
    // Crear la cámara con los parámetros especificados
    camera = Camera(startPosition, startUp, startYaw, startPitch, startMoveSpeed, startTurnSpeed);
}

// Funcion para actualizar cada frame con las cosas que no dependen del input del usuario
void SceneInformation::actualizarFrame(float deltaTime)
{
    
}


// Funcion para actualizar cada frame con el input del usuario
void SceneInformation::actualizarFrameInput(bool* keys, GLfloat mouseXChange, GLfloat mouseYChange, GLfloat scrollChange, float deltaTime)
{
    // Actualizar cámara con input del usuario (mouse y teclado)
    camera.keyControl(keys, deltaTime);
    camera.mouseControl(mouseXChange, mouseYChange);
    camera.mouseScrollControl(scrollChange);  // Agregar control del scroll
}

// Funcion para inicializar la skybox
void SceneInformation::inicializarSkybox()
{
    // Establecer el skybox por defecto
    setSkyboxActual(AssetConstants::SkyboxNames::DAY);
}

// Funcion para inicializar la luz direccional
void SceneInformation::inicializarLuces()
{
    // Obtener la luz del sol desde el LightManager
    DirectionalLight* sunLight = lightManager.getDirectionalLight(AssetConstants::LightNames::SOL);
    
    if (sunLight != nullptr) {
        // Establecer la luz del sol como luz direccional principal
        luzDireccional = *sunLight;
    } else {
        // Si no existe, crear una luz direccional por defecto
        luzDireccional = DirectionalLight(
            1.0f, 1.0f, 1.0f,      
            0.3f, 0.5f,           
            0.0f, -1.0f, 0.0f     
        );
    }
    

}

// Funcion para inicializar todas las entidades
void SceneInformation::inicializarEntidades()
{
    crearPersonajePrincipal();
    crearPiso();
    

}

// Funcion para crear a los personjes(ahorita solo esta cuphead)
void SceneInformation::crearPersonajePrincipal()
{
    // Crear entidad prueba del personaje de Cuphead
    Entidad* testCharacter = new Entidad("testCharacter",
        glm::vec3(0.0f, 0.0f, 0.0f),      // Posición inicial
        glm::vec3(-90.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.5f, 1.5f, 1.5f));      // Escala
    
    testCharacter->setTipoObjeto(TipoObjeto::MODELO);
    testCharacter->nombreModelo = AssetConstants::ModelNames::CUPHEAD;
    testCharacter->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    testCharacter->actualizarTransformacion();
    agregarEntidad(testCharacter);
    
    // Configurar la cámara en tercera persona siguiendo al personaje(esto se va a cambiar mas adelante)
    camera.setThirdPersonTarget(testCharacter);
}

void SceneInformation::crearPiso()
{
    // Crear el piso
    Entidad* piso = new Entidad("piso");
    piso->setTipoObjeto(TipoObjeto::MESH);
    piso->nombreMesh = AssetConstants::MeshNames::PISO;
    piso->nombreTextura = AssetConstants::TextureNames::PASTO;
    piso->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    piso->posicionLocal = glm::vec3(0.0f, -1.0f, 0.0f);
    piso->escalaLocal = glm::vec3(30.0f, 1.0f, 30.0f);
    piso->actualizarTransformacion();
    agregarEntidad(piso);
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
    if (entidad->getTipoObjeto() == TipoObjeto::MODELO) {
        std::string modelName = entidad->nombreModelo;
        if (!modelName.empty()) {
            Model* modelo = modelManager.getModel(modelName);
            if (modelo != nullptr) {
                entidad->setModelo(modelName, modelo);
            }
        }
    }
    
    // Vincular mesh si la entidad usa un mesh
    if (entidad->getTipoObjeto() == TipoObjeto::MESH) {
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
    
    // Vincular material si la entidad tiene un nombre de material
    std::string materialName = entidad->nombreMaterial;
    if (!materialName.empty()) {
        Material* material = materialManager.getMaterial(materialName);
        if (material != nullptr) {
            entidad->setMaterial(materialName, material);
        }
    }
    
    // Vincular recursos recursivamente para todas las entidades hijas
    for (auto* hijo : entidad->hijos) {
        vincularRecursos(hijo);
    }
}
