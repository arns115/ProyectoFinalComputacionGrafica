#pragma once

#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "ModelManager.h"

// TODO: Crear una superclase GameObject y heredar Entidad de ella para futuras mejoras

// Estructura auxiliar para representar una parte de la entidad con su transformación
struct EntidadParte {
    std::string nombreModelo;          // Nombre del modelo a renderizar
    glm::vec3 posicionLocal;           // Posición relativa al padre
    glm::vec3 rotacionLocal;           // Rotación en grados (X, Y, Z)
    glm::vec3 escalaLocal;             // Escala local
    glm::mat4 transformacionLocal;     // Matriz de transformación local
    
    std::vector<EntidadParte*> hijos;  // Partes hijas (jerarquía)
    
    // Constructor
    EntidadParte(const std::string& modelo, 
                 glm::vec3 pos = glm::vec3(0.0f), 
                 glm::vec3 rot = glm::vec3(0.0f), 
                 glm::vec3 escala = glm::vec3(1.0f));
    
    // Destructor
    ~EntidadParte();
    
    // Actualiza la matriz de transformación local
    void actualizarTransformacion();
    
    // Agregar una parte hija
    void agregarHijo(EntidadParte* hijo);
};

// Clase principal Entidad para manejar modelos jerárquicos
class Entidad {
public:
    // Constructor principal con transformaciones y raíz opcional
    Entidad(glm::vec3 posicion = glm::vec3(0.0f), 
            glm::vec3 rotacion = glm::vec3(0.0f), 
            glm::vec3 escala = glm::vec3(1.0f),
            EntidadParte* raizModelo = nullptr);
    
    // Constructor alternativo: solo la raíz con transformaciones por defecto
    Entidad(EntidadParte* raizModelo);
    
    // Constructor con nombre de modelo: crea automáticamente la raíz
    Entidad(const std::string& nombreModeloRaiz,
            glm::vec3 posicion = glm::vec3(0.0f), 
            glm::vec3 rotacion = glm::vec3(0.0f), 
            glm::vec3 escala = glm::vec3(1.0f));
    
    // Destructor
    ~Entidad();
    
    // Establece la parte raíz de la entidad
    void setRaiz(EntidadParte* nuevaRaiz);
    
    // Obtiene la parte raíz
    EntidadParte* getRaiz();
    
    // Renderiza toda la jerarquía de modelos
    void renderizar(ModelManager& modelManager, GLuint uniformModel);
    
    // Actualiza la transformación global de la entidad
    void actualizarTransformacionMundo();
    
    // Métodos de acceso y modificación de transformaciones globales
    void setPosicion(const glm::vec3& pos);
    void setRotacion(const glm::vec3& rot);
    void setEscala(const glm::vec3& escala);
    
    // Métodos para modificar transformaciones de forma incremental
    void trasladar(const glm::vec3& delta);
    void rotar(const glm::vec3& delta);
    void escalar(const glm::vec3& factor);
    
    // Getters
    glm::vec3 getPosicion() const;
    glm::vec3 getRotacion() const;
    glm::vec3 getEscala() const;
    glm::mat4 getTransformacion() const;
    
private:
    // Transformaciones globales de la entidad
    glm::vec3 posicionMundo;
    glm::vec3 rotacionMundo;
    glm::vec3 escalaMundo;
    glm::mat4 transformacionMundo;
    
    // Parte raíz de la jerarquía
    EntidadParte* raiz;
    
    // Función recursiva para renderizar la jerarquía
    void renderizarRecursivo(EntidadParte* parte, 
                            const glm::mat4& transformacionPadre, 
                            ModelManager& modelManager, 
                            GLuint uniformModel);
};
