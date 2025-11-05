#include "ComponenteAnimacion.h"
#include "Entidad.h"
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <cmath>
#include <random>

// Parametros de las diferentes animaciones (ahora solo esta implementado caminar de isaac)
namespace {
    const float ISAAC_AMPLITUD_PIERNAS = 35.0f;
    const float ISAAC_AMPLITUD_BRAZOS = 25.0f;
    const float ISAAC_AMPLITUD_CABEZA = 5.0f;
    const float ISAAC_BOBBING = 0.08f;
    
    // Variables globales para los calculos de animaciones
    bool animacionActiva = false;
    float tiempo = 0.0f;
    float anguloBase = 0.0f;
    float anguloOpuesto = 0.0f;
}

ComponenteAnimacion::ComponenteAnimacion(Entidad* entidad)
    : entidad(entidad),
    banderasAnimacion(0),
    numeroAnimaciones(0)
{
    // Inicializar arrays de tiempos y velocidades
    for (int i = 0; i < 16; i++) {
        tiemposAnimacion[i] = 0.0f;
        velocidadesAnimacion[i] = 0.1f;  // Velocidad default
    }
}

// Activa la animacion del bit especificado
void ComponenteAnimacion::activarAnimacion(int indiceAnimacion)
{
    if (indiceAnimacion >= 0 && indiceAnimacion < 16) {
        banderasAnimacion |= (1 << indiceAnimacion);
        tiemposAnimacion[indiceAnimacion] = 0.0f;  
    }
}

// Apaga la animacion del bit especificado
void ComponenteAnimacion::desactivarAnimacion(int indiceAnimacion)
{
    if (indiceAnimacion >= 0 && indiceAnimacion < 16) {
        banderasAnimacion &= ~(1 << indiceAnimacion);
    }
}

// Checa si una animacion esta activa (la animacion del bit especificado)
bool ComponenteAnimacion::estaActiva(int indiceAnimacion) const
{
    if (indiceAnimacion >= 0 && indiceAnimacion < 16) {
        return (banderasAnimacion & (1 << indiceAnimacion)) != 0;
    }
    return false;
}

// FuncionPrinciapl para actualizar la animacion1
void ComponenteAnimacion::actualizarAnimacion(int indiceAnimacion, float deltaTime, float velocidadMovimiento)
{
    if (entidad == nullptr || indiceAnimacion < 0 || indiceAnimacion >= 16) {
        return;
    }
    
    // Dependiendo de entidad llamar a su funcion de animacion
    if (entidad->nombreObjeto == "isaac_cuerpo") {
        animarIsaac(indiceAnimacion, deltaTime, velocidadMovimiento);
    }
    else if (entidad->nombreObjeto == "hollow") {
		animarHollow(indiceAnimacion, deltaTime);
    }
}

// Se utiliza funcion senoidal para animar a Isaac caminando (se utiliza funcion senoidal del tiempo para saber que angulo darle a los brazos y a las piernas)
// Se termina si tiempo acumulado llega a 2 * pi
void ComponenteAnimacion::animarIsaac(int indiceAnimacion, float deltaTime, float velocidadMovimiento)
{
    bool animacionActiva = estaActiva(indiceAnimacion);
    
    // Si hay movimiento, activar animación
    if (velocidadMovimiento > 0.01f) {
        if (!animacionActiva) {
            activarAnimacion(indiceAnimacion);
        }
        
        // Acumular tiempo
        tiemposAnimacion[indiceAnimacion] += deltaTime * velocidadesAnimacion[indiceAnimacion];
        
		// se ajusta para que la posicion sea la misma cuando se empezo a mover
        if (tiemposAnimacion[indiceAnimacion] >= glm::two_pi<float>()) {
            tiemposAnimacion[indiceAnimacion] -= glm::two_pi<float>();
        }
    }
    else if (animacionActiva) {
        // Continuar hasta completar el ciclo
        tiemposAnimacion[indiceAnimacion] += deltaTime * velocidadesAnimacion[indiceAnimacion];
        
        // Verificar si ya termino la animacion
        if (tiemposAnimacion[indiceAnimacion] >= glm::two_pi<float>()) {
			// Ajustar el tiempo para que se posicionen bien las partes del cuerpo
            tiemposAnimacion[indiceAnimacion] = glm::two_pi<float>();
        }
    }
    else {
        // No hay movimiento y no hay animación activa
        return;
    }
    
    // Aplicar transformaciones de animación
    tiempo = tiemposAnimacion[indiceAnimacion];
    anguloBase = sin(tiempo);
    anguloOpuesto = sin(tiempo + glm::pi<float>());

    for (auto* hijo : entidad->hijos) {
        if (hijo == nullptr) continue;
        
        const std::string& nombre = hijo->nombreObjeto;
        
        // Piernas
        if (nombre.find("pierna_derecha") != std::string::npos) {
            hijo->rotacionLocalQuat = glm::angleAxis(glm::radians(hijo->rotacionInicial.x + (anguloBase * ISAAC_AMPLITUD_PIERNAS)), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if (nombre.find("pierna_izquierda") != std::string::npos) {
            hijo->rotacionLocalQuat = glm::angleAxis(glm::radians(hijo->rotacionInicial.x + (anguloOpuesto * ISAAC_AMPLITUD_PIERNAS)), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        // Brazos
        else if (nombre.find("brazo_derecho") != std::string::npos) {
            hijo->rotacionLocalQuat = glm::angleAxis(glm::radians(hijo->rotacionInicial.x + (anguloOpuesto * ISAAC_AMPLITUD_BRAZOS)), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if (nombre.find("brazo_izquierdo") != std::string::npos) {
            hijo->rotacionLocalQuat = glm::angleAxis(glm::radians(hijo->rotacionInicial.x + (anguloBase * ISAAC_AMPLITUD_BRAZOS)), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        
        // Cabeza
        else if (nombre.find("cabeza") != std::string::npos) {
            hijo->rotacionLocalQuat = glm::angleAxis(glm::radians(hijo->rotacionInicial.z + (anguloBase * ISAAC_AMPLITUD_CABEZA)), glm::vec3(0.0f, 0.0f, 1.0f));
            hijo->posicionLocal.y = hijo->posicionInicial.y + abs(sin(tiempo * 2.0f)) * ISAAC_BOBBING;
        }
        
        hijo->actualizarTransformacion();
    }
    
    // Después de aplicar transformaciones, verificar si ya termino la animacion para desactivarla
    if (velocidadMovimiento <= 0.01f && tiemposAnimacion[indiceAnimacion] >= glm::two_pi<float>()) {
        desactivarAnimacion(indiceAnimacion);
        tiemposAnimacion[indiceAnimacion] = 0.0f;
    }
}

void ComponenteAnimacion::animarHollow(int indiceAnimacion, float deltaTime) {

}
