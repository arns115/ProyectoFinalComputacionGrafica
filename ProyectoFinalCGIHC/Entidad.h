#pragma once

#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Model.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

// Forward declaration
class SceneRenderer;

// Enum para el tipo de geometría de la entidad
enum class TipoGeometria {
    MODELO,     // Usa un modelo 3D
    MESH        // Usa un mesh
};

// Clase principal Entidad para manejar entidades en la escena
class Entidad {
public:
    // Constructor con nombre de objeto
    Entidad(const std::string& nombreObjeto = "", 
            glm::vec3 pos = glm::vec3(0.0f), 
            glm::vec3 rot = glm::vec3(0.0f), 
            glm::vec3 escala = glm::vec3(1.0f));
    
    // Destructor
    ~Entidad();
    
    // Actualiza la matriz de transformación local
    void actualizarTransformacion();
    
    // Agregar una entidad hija
    void agregarHijo(Entidad* hijo);
    
    // Establecer modelo con nombre y apuntador
    void setModelo(const std::string& nombreModelo, Model* modelo);

    // Establecer Mesh con nombre y apuntador
    void setMesh(const std::string& nombreMesh, Mesh* mesh);

    // Establecer textura con nombre y apuntador
    void setTextura(const std::string& nombreTextura, Texture* texture);
    
    // Establecer material con nombre y apuntador
    void setMaterial(const std::string& nombreMaterial, Material* mat);
    
    // Limpiar geometría y textura
    void limpiarGeometria();
    void limpiarTextura();
    
    // Acceso público a propiedades
    std::string nombreObjeto;          // Nombre de la entidad
    std::string nombreModelo;          // Nombre del modelo
    std::string nombreMesh;            // Nombre del mesh
    std::string nombreTextura;         // Nombre de la textura
    std::string nombreMaterial;        // Nombre del material
    
    // Acceso a transformaciones locales
    glm::vec3 posicionLocal;           // Posición relativa al padre
    glm::vec3 rotacionLocal;           // Rotación en grados (X, Y, Z)
    glm::vec3 escalaLocal;             // Escala local
    glm::mat4 transformacionLocal;     // Matriz de transformación local
    
    std::vector<Entidad*> hijos;       // Entidades hijas (jerarquía)
    
    // Obtener tipo de geometría
    TipoGeometria getTipoGeometria() const { return tipoGeometria; }
    
    // SceneRenderer necesita acceso a los miembros privados
    friend class SceneRenderer;
    
private:
    // Tipo de geometría que usa esta entidad
    TipoGeometria tipoGeometria;
    
    // Punteros directos a recursos
    Model* modelo;
    Mesh* mesh;
    Texture* texture;
    Material* material;
};
