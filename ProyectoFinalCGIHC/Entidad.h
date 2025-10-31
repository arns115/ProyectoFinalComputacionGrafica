#pragma once

#include <vector>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "ModelManager.h"

// TODO: Crear una superclase GameObject y heredar Entidad de ella para futuras mejoras

// Estructura auxiliar para representar una parte de la entidad con su transformaci�n
struct EntidadParte {
    std::string nombreModelo;          // Nombre del modelo a renderizar
    glm::vec3 posicionLocal;           // Posici�n relativa al padre
    glm::vec3 rotacionLocal;           // Rotaci�n en grados (X, Y, Z)
    glm::vec3 escalaLocal;             // Escala local
    glm::mat4 transformacionLocal;     // Matriz de transformaci�n local
    
    std::vector<EntidadParte*> hijos;  // Partes hijas (jerarqu�a)
    
    // Constructor
    EntidadParte(const std::string& modelo, 
                 glm::vec3 pos = glm::vec3(0.0f), 
                 glm::vec3 rot = glm::vec3(0.0f), 
                 glm::vec3 escala = glm::vec3(1.0f));
    
    // Destructor
    ~EntidadParte();
    
    // Actualiza la matriz de transformaci�n local
    void actualizarTransformacion();
    
    // Agregar una parte hija
    void agregarHijo(EntidadParte* hijo);
};

// Clase principal Entidad para manejar modelos jer�rquicos
class Entidad {
public:
    // Constructor principal con transformaciones y ra�z opcional
    Entidad(glm::vec3 posicion = glm::vec3(0.0f), 
            glm::vec3 rotacion = glm::vec3(0.0f), 
            glm::vec3 escala = glm::vec3(1.0f),
            EntidadParte* raizModelo = nullptr);
    
    // Constructor alternativo: solo la ra�z con transformaciones por defecto
    Entidad(EntidadParte* raizModelo);
    
    // Constructor con nombre de modelo: crea autom�ticamente la ra�z
    Entidad(const std::string& nombreModeloRaiz,
            glm::vec3 posicion = glm::vec3(0.0f), 
            glm::vec3 rotacion = glm::vec3(0.0f), 
            glm::vec3 escala = glm::vec3(1.0f));
    
    // Destructor
    ~Entidad();
    
    // Establece la parte ra�z de la entidad
    void setRaiz(EntidadParte* nuevaRaiz);
    
    // Obtiene la parte ra�z
    EntidadParte* getRaiz();
    
    // Renderiza toda la jerarqu�a de modelos
    void renderizar(ModelManager& modelManager, GLuint uniformModel);
    
    // Actualiza la transformaci�n global de la entidad
    void actualizarTransformacionMundo();
    
    // M�todos de acceso y modificaci�n de transformaciones globales
    void setPosicion(const glm::vec3& pos);
    void setRotacion(const glm::vec3& rot);
    void setEscala(const glm::vec3& escala);
    
    // M�todos para modificar transformaciones de forma incremental
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
    
    // Parte ra�z de la jerarqu�a
    EntidadParte* raiz;
    
    // Funci�n recursiva para renderizar la jerarqu�a
    void renderizarRecursivo(EntidadParte* parte, 
                            const glm::mat4& transformacionPadre, 
                            ModelManager& modelManager, 
                            GLuint uniformModel);
};
