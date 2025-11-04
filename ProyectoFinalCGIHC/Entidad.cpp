#include "Entidad.h"


Entidad::Entidad(const std::string& nombreObj, 
                 glm::vec3 pos, 
                 glm::vec3 rot, 
                 glm::vec3 escala)
    : nombreObjeto(nombreObj), nombreModelo(""), nombreMesh(""), nombreTextura(""), nombreMaterial(""),
      posicionLocal(pos), rotacionLocal(rot), 
      escalaLocal(escala), transformacionLocal(glm::mat4(1.0f)),
      posicionInicial(pos), rotacionInicial(rot), escalaInicial(escala),  // Guardar valores iniciales (estado inicial del objeto)
      TipoObjeto(TipoObjeto::MODELO), modelo(nullptr), mesh(nullptr), 
      texture(nullptr), material(nullptr),
      banderasAnimacion(0), numeroAnimaciones(0),
      velocidad(0.0f, 0.0f, 0.0f),        // NUEVO: Inicializar velocidad
      gravedad(-15.0f),                    // NUEVO: Gravedad estándar
      fisicaHabilitada(false),            // NUEVO: Física deshabilitada por defecto
      enSuelo(false)                       // NUEVO: No está en el suelo inicialmente
{
    actualizarTransformacion();
}


Entidad::~Entidad() 
{
}

void Entidad::actualizarTransformacion() 
{
    transformacionLocal = glm::mat4(1.0f);
    
    // Aplicar traslación
    transformacionLocal = glm::translate(transformacionLocal, posicionLocal);
    
    // Aplicar rotaciones (en orden Z, Y, X)
    transformacionLocal = glm::rotate(transformacionLocal, 
                                     glm::radians(rotacionLocal.z), 
                                     glm::vec3(0.0f, 0.0f, 1.0f));
    transformacionLocal = glm::rotate(transformacionLocal, 
                                     glm::radians(rotacionLocal.y), 
                                     glm::vec3(0.0f, 1.0f, 0.0f));
    transformacionLocal = glm::rotate(transformacionLocal, 
                                     glm::radians(rotacionLocal.x), 
                                     glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Aplicar escala
    transformacionLocal = glm::scale(transformacionLocal, escalaLocal);
}

void Entidad::agregarHijo(Entidad* hijo) 
{
    if (hijo != nullptr) {
        hijos.push_back(hijo);
    }
}

// NUEVO: Aplicar física (gravedad y colisión con suelo)
void Entidad::aplicarFisica(float deltaTime, float nivelSuelo)
{
    if (!fisicaHabilitada) {
        return;
    }
    
    // Aplicar gravedad a la velocidad vertical
    velocidad.y += gravedad * deltaTime;
    
    // Aplicar velocidad a la posición
    posicionLocal += velocidad * deltaTime;
    
    // Colisión con el suelo
    if (posicionLocal.y <= nivelSuelo + posicionInicial.y) {
        posicionLocal.y = nivelSuelo + posicionInicial.y;
        velocidad.y = 0.0f;
        enSuelo = true;
    } else {
        enSuelo = false;
    }
    
    // Actualizar transformación después de aplicar física
    actualizarTransformacion();
}

// NUEVO: Aplicar salto
void Entidad::saltar(float fuerzaSalto)
{
    if (!fisicaHabilitada) {
        return;
    }
    
    // Solo permitir saltar si está en el suelo
    if (enSuelo) {
        velocidad.y = fuerzaSalto;
        enSuelo = false;
    }
}

void Entidad::setModelo(const std::string& nombreModelo, Model* modelo)
{
    this->nombreModelo = nombreModelo;
    this->modelo = modelo;
    this->TipoObjeto = TipoObjeto::MODELO;
    this->mesh = nullptr;
    this->nombreMesh = "";
}

void Entidad::setMesh(const std::string& nombreMesh, Mesh* mesh)
{
    this->nombreMesh = nombreMesh;
    this->mesh = mesh;
    this->TipoObjeto = TipoObjeto::MESH;
    this->modelo = nullptr;
    this->nombreModelo = "";
}

void Entidad::setTextura(const std::string& nombreTextura, Texture* texture)
{
    this->nombreTextura = nombreTextura;
    this->texture = texture;
}

void Entidad::setMaterial(const std::string& nombreMaterial, Material* mat)
{
    this->nombreMaterial = nombreMaterial;
    this->material = mat;
}

void Entidad::limpiarGeometria()
{
    this->nombreModelo = "";
    this->nombreMesh = "";
    this->modelo = nullptr;
    this->mesh = nullptr;
}

void Entidad::limpiarTextura()
{
    this->nombreTextura = "";
    this->texture = nullptr;
}
