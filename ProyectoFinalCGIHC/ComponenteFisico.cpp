#include "ComponenteFisico.h"

ComponenteFisico::ComponenteFisico()
    : velocidad(0.0f, 0.0f, 0.0f),
      gravedad(-15.0f),
      habilitada(false),
      enSuelo(false)
{
}

void ComponenteFisico::aplicarFisica(float deltaTime, float nivelSuelo, glm::vec3& posicion, const glm::vec3& posicionInicial)
{
    if (!habilitada) {
        return;
    }
    
    // Aplicar gravedad a la velocidad vertical
    velocidad.y += gravedad * deltaTime;
    
    // Aplicar velocidad a la posición
    posicion += velocidad * deltaTime;
    
    // Colisión con el suelo
    if (posicion.y <= posicionInicial.y) {
        posicion.y = posicionInicial.y;
        velocidad.y = 0.0f;
        enSuelo = true;
    } else {
        enSuelo = false;
    }
}

void ComponenteFisico::saltar(float fuerzaSalto)
{
    if (!habilitada) {
        return;
    }
    
    // Solo permitir saltar si está en el suelo
    if (enSuelo) {
        velocidad.y = fuerzaSalto;
        enSuelo = false;
    }
}
