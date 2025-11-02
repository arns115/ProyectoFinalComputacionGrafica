#pragma once

#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Model.h"
#include "Texture.h"
#include "Mesh.h"

// Enum para el tipo de geometría de la entidad
enum class TipoGeometria {
    MODELO,     // Usa un modelo 3D
    MESH        // Usa un mesh
};

// Clase principal Entidad para manejar modelos jerárquicos
class Entidad {
public:
    // Constructor con nombre de modelo
    Entidad(const std::string& modelo, 
            glm::vec3 pos = glm::vec3(0.0f), 
            glm::vec3 rot = glm::vec3(0.0f), 
            glm::vec3 escala = glm::vec3(1.0f));
    
    // Destructor
    ~Entidad();
    
    // Actualiza la matriz de transformación local
    void actualizarTransformacion();
    
    // Agregar una entidad hija
    void agregarHijo(Entidad* hijo);
    
    // Establecer el tipo de geometría y asignar modelo o mesh
    // Acepta tanto el nombre como el puntero al recurso
    void setModelo(const std::string& nombreModelo, Model* modelo);
    void setMesh(const std::string& nombreMesh, Mesh* mesh);
    
    // Establecer textura con nombre y puntero
    void setTextura(const std::string& nombreTextura, Texture* texture);
    
    // Limpiar geometría y textura
    void limpiarGeometria();
    void limpiarTextura();
    
    // Acceso público a propiedades
    std::string nombreObjeto;          // Nombre único del objeto/entidad
    std::string nombreModelo;          // Nombre del modelo
    std::string nombreMesh;            // Nombre del mesh
    std::string nombreTextura;         // Nombre de la textura
    
    // Acceso a transformaciones locales
    glm::vec3 posicionLocal;           // Posición relativa al padre
    glm::vec3 rotacionLocal;           // Rotación en grados (X, Y, Z)
    glm::vec3 escalaLocal;             // Escala local
    glm::mat4 transformacionLocal;     // Matriz de transformación local
    
    std::vector<Entidad*> hijos;       // Entidades hijas (jerarquía)
    
    // Obtener tipo de geometría
    TipoGeometria getTipoGeometria() const { return tipoGeometria; }
    
private:
    // Tipo de geometría que usa esta entidad
    TipoGeometria tipoGeometria;
    
    // Punteros directos a recursos
    Model* modelo;
    Mesh* mesh;
    Texture* texture;
};
