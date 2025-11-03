#include "Entidad.h"


Entidad::Entidad(const std::string& nombreObj, 
                 glm::vec3 pos, 
                 glm::vec3 rot, 
                 glm::vec3 escala)
    : nombreObjeto(nombreObj), nombreModelo(""), nombreMesh(""), nombreTextura(""), nombreMaterial(""),
      posicionLocal(pos), rotacionLocal(rot), 
      escalaLocal(escala), transformacionLocal(glm::mat4(1.0f)),
      TipoObjeto(TipoObjeto::MODELO), modelo(nullptr), mesh(nullptr), 
      texture(nullptr), material(nullptr),
      banderasAnimacion(0), numeroAnimaciones(0)
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
