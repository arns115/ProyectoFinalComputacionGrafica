#pragma once

#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>
#include "Model.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"


class SceneRenderer;
class ComponenteFisico;
class ComponenteAnimacion;

// Enum para el tipo de geometría de la entidad
enum class TipoObjeto {
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
    
    // Establecer tipo de objeto
    void setTipoObjeto(TipoObjeto tipo) { TipoObjeto = tipo; }

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
    
    // Obtener tipo de geometría
    TipoObjeto getTipoObjeto() const { return TipoObjeto; }
    
    // Propiedades básicas
    std::string nombreObjeto;          // Nombre de la entidad
    std::string nombreModelo;          // Nombre del modelo
    std::string nombreMesh;            // Nombre del mesh
    std::string nombreTextura;         // Nombre de la textura
    std::string nombreMaterial;        // Nombre del material
    
    // Transformaciones locales
    glm::vec3 posicionLocal;           // Posición relativa al padre
    glm::vec3 rotacionLocal;           // Rotación en grados (X, Y, Z)
    glm::quat rotacionLocalQuat;       // Rotación como quaternion
    glm::vec3 escalaLocal;             // Escala local
    glm::mat4 transformacionLocal;     // Matriz de transformación local
    
    // Transformaciones iniciales
    glm::vec3 posicionInicial;         // Posición inicial
    glm::vec3 rotacionInicial;         // Rotación inicial
    glm::quat rotacionInicialQuat;     // Rotación inicial como quaternion
    glm::vec3 escalaInicial;           // Escala inicial
    
    // Jerarquía
    std::vector<Entidad*> hijos;       // Entidades hijas
    
    // Componentes de animacion y fisica (no todas las entdades los tienes)
    ComponenteFisico* fisica;         // Componente de física
    ComponenteAnimacion* animacion;     // Componente de animación
    
    friend class SceneRenderer;
    friend class ComponenteAnimacion;
    
private:
    // Tipo de geometría que usa esta entidad
    TipoObjeto TipoObjeto;
    
    // Punteros directos a recursos
    Model* modelo;
    Mesh* mesh;
    Texture* texture;
    Material* material;
    
    // Sincronizar rotacion con el quaternion
    void sincronizarRotacion();
};
