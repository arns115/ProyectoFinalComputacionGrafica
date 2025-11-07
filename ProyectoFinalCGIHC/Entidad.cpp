#include "Entidad.h"


Entidad::Entidad(const std::string& nombreObj, 
                 glm::vec3 pos, 
                 glm::vec3 rot, 
                 glm::vec3 escala)
    : nombreObjeto(nombreObj), nombreModelo(""), nombreMesh(""), nombreTextura(""), nombreMaterial(""),
      posicionLocal(pos), rotacionLocal(glm::vec3(0.0f)), 
      escalaLocal(escala), transformacionLocal(glm::mat4(1.0f)),
      posicionInicial(pos), rotacionInicial(rot), escalaInicial(escala),
      TipoObjeto(TipoObjeto::MODELO), modelo(nullptr), mesh(nullptr), 
      texture(nullptr), material(nullptr), fisica(nullptr), animacion(nullptr)
{
    rotacionLocalQuat = glm::angleAxis(glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                        glm::angleAxis(glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                        glm::angleAxis(glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotacionInicialQuat = rotacionLocalQuat;
    actualizarTransformacion();
}


Entidad::~Entidad() 
{
}

void Entidad::actualizarTransformacion() 
{
    // Se actualiza la informacion del aquaternion
    sincronizarRotacion();
    
    transformacionLocal = glm::mat4(1.0f);
	// Se traslada adecuadamente
    transformacionLocal = glm::translate(transformacionLocal, posicionLocal);
	// Se rota adecuadamente
    transformacionLocal *= glm::mat4_cast(rotacionLocalQuat);
	// Se escala adecuadamente
    transformacionLocal = glm::scale(transformacionLocal, escalaLocal);
}


// Sincroniza la rotacion con el quaternion
void Entidad::sincronizarRotacion() {
    rotacionLocalQuat = rotacionLocalQuat * glm::quat(glm::radians(rotacionLocal));
	rotacionLocalQuat = glm::normalize(rotacionLocalQuat);
    rotacionLocal = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Entidad::agregarHijo(Entidad* hijo) 
{
    if (hijo != nullptr) {
        hijos.push_back(hijo);
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









