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
    
    // Aplicar física (gravedad) - NUEVO
    void aplicarFisica(float deltaTime, float nivelSuelo = -1.0f);
    
    // Aplicar salto - NUEVO
    void saltar(float fuerzaSalto = 5.0f);
    
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

    // Establecer número de animaciones
    void setNumeroAnimaciones(unsigned int numAnimaciones) { numeroAnimaciones = numAnimaciones; }

    // Habilitar/deshabilitar física - NUEVO
    void habilitarFisica(bool habilitar) { fisicaHabilitada = habilitar; }
    bool tieneFisicaHabilitada() const { return fisicaHabilitada; }
    
    // Verificar si está en el suelo - NUEVO
    bool estaEnSuelo() const { return enSuelo; }
    
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
    
    // Transformaciones iniciales (para poder regresar objetos a sus estados iniciales)
    glm::vec3 posicionInicial;         // Posición inicial configurada en el constructor
    glm::vec3 rotacionInicial;         // Rotación inicial configurada en el constructor
    glm::vec3 escalaInicial;           // Escala inicial configurada en el constructor
    
    std::vector<Entidad*> hijos;       // Entidades hijas (jerarquía)
    
    // Propiedades de animacion
    unsigned short banderasAnimacion;  // Banderas para control de animación (cada bit representa si esa animacion esta activa, por ejemplo el bit menos signficiativo representa si la primera animacion esta activa)
    unsigned int numeroAnimaciones;    // Numero de animaciones de la entidad
    
    // Propiedades físicas - NUEVO
    glm::vec3 velocidad;               // Velocidad del objeto (incluye velocidad vertical)
    float gravedad;                    // Fuerza de gravedad aplicada
    bool fisicaHabilitada;             // Si la física está activa para esta entidad
    bool enSuelo;                      // Si el objeto está tocando el suelo
    
    // Obtener tipo de geometría
    TipoObjeto getTipoObjeto() const { return TipoObjeto; }
    
    // SceneRenderer necesita acceso a los miembros privados
    friend class SceneRenderer;
    
private:
    // Tipo de geometría que usa esta entidad
    TipoObjeto TipoObjeto;
    
    // Punteros directos a recursos
    Model* modelo;
    Mesh* mesh;
    Texture* texture;
    Material* material;
};
