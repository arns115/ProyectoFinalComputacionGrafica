#pragma once

#include <glm.hpp>

// Componente de física para entidades
class ComponenteFisico {
public:
    ComponenteFisico();
    
    // Aplicar física (gravedad y colisión)
    void aplicarFisica(float deltaTime, float nivelSuelo, glm::vec3& posicion, const glm::vec3& posicionInicial);
    
    // Salto de la entidad
    void saltar(float fuerzaSalto);
    
    // Verificar si está en el suelo
    bool estaEnSuelo() const { return enSuelo; }
    
    // Habilitar/deshabilitar física
    void habilitar(bool habilitar) { habilitada = habilitar; }
    bool estaHabilitada() const { return habilitada; }

    glm::vec3 velocidad;    // Velocidad del objeto
    float gravedad;         // Fuerza de gravedad
    
private:
    bool habilitada;        // Si la física está activa
    bool enSuelo;          // Si el objeto está tocando el suelo
};
