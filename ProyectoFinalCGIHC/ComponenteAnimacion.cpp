#include "ComponenteAnimacion.h"
#include "Entidad.h"
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <cmath>

// Parametros de las diferentes animaciones
namespace {
    // Isaac
    const float ISAAC_AMPLITUD_PIERNAS = 35.0f;
    const float ISAAC_AMPLITUD_BRAZOS = 25.0f;
    const float ISAAC_AMPLITUD_CABEZA = 5.0f;
    const float ISAAC_BOBBING = 0.08f;
    
    // Cuphead
    const float CUPHEAD_AMPLITUD_BRAZOS = 25.0f;         // Amplitud del swing de brazos
    const float CUPHEAD_AMPLITUD_ANTEBRAZOS = 8.0f;      // Movimiento leve de antebrazos
    const float CUPHEAD_AMPLITUD_MUSLOS = 40.0f;         // Movimiento pronunciado de muslos
    const float CUPHEAD_AMPLITUD_PIES = 30.0f;           // Movimiento pronunciado de pies
    const float CUPHEAD_BOBBING = 0.06f;                 // Movimiento vertical del cuerpo
    
    // Variables globales para los calculos de animacion
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
    if (entidad->nombreObjeto.find("isaac_cuerpo") != std::string::npos) {
        animarIsaac(indiceAnimacion, deltaTime, velocidadMovimiento);
    }
    else if (entidad->nombreObjeto.find("cuphead_torso") != std::string::npos) {
        animarCuphead(indiceAnimacion, deltaTime, velocidadMovimiento);
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
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloBase * ISAAC_AMPLITUD_PIERNAS);
        }
        else if (nombre.find("pierna_izquierda") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloOpuesto * ISAAC_AMPLITUD_PIERNAS);
        }
        
        // Brazos
        else if (nombre.find("brazo_derecho") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloOpuesto * ISAAC_AMPLITUD_BRAZOS);
        }
        else if (nombre.find("brazo_izquierdo") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloBase * ISAAC_AMPLITUD_BRAZOS);
        }
        
        // Cabeza
        else if (nombre.find("cabeza") != std::string::npos) {
            hijo->rotacionLocal.z = hijo->rotacionInicial.z + (anguloBase * ISAAC_AMPLITUD_CABEZA);
            hijo->posicionLocal.y = hijo->posicionInicial.y + 
                                   abs(sin(tiempo * 2.0f)) * ISAAC_BOBBING;
        }
        
        hijo->actualizarTransformacion();
    }
    
    // Después de aplicar transformaciones, verificar si ya termino la animacion para desactivarla
    if (velocidadMovimiento <= 0.01f && tiemposAnimacion[indiceAnimacion] >= glm::two_pi<float>()) {
        desactivarAnimacion(indiceAnimacion);
        tiemposAnimacion[indiceAnimacion] = 0.0f;
    }
}

// Animación de caminata para Cuphead
void ComponenteAnimacion::animarCuphead(int indiceAnimacion, float deltaTime, float velocidadMovimiento)
{
    bool animacionActiva = estaActiva(indiceAnimacion);
    
    // Si hay movimiento, activar animación
    if (velocidadMovimiento > 0.01f) {
        if (!animacionActiva) {
            activarAnimacion(indiceAnimacion);
        }
        
        // Acumular tiempo
        tiemposAnimacion[indiceAnimacion] += deltaTime * velocidadesAnimacion[indiceAnimacion];
        
        // Se ajusta para que la posicion sea la misma cuando se empezó a mover
        if (tiemposAnimacion[indiceAnimacion] >= glm::two_pi<float>()) {
            tiemposAnimacion[indiceAnimacion] -= glm::two_pi<float>();
        }
    }
    else if (animacionActiva) {
        // Continuar hasta completar el ciclo
        tiemposAnimacion[indiceAnimacion] += deltaTime * velocidadesAnimacion[indiceAnimacion];
        
        // Verificar si ya terminó la animación
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
    
    // Bobbing del torso (movimiento vertical sutil)
    entidad->posicionLocal.y = entidad->posicionInicial.y + 
                               abs(sin(tiempo * 2.0f)) * CUPHEAD_BOBBING;
    
    for (auto* hijo : entidad->hijos) {
        if (hijo == nullptr) continue;
        
        const std::string& nombre = hijo->nombreObjeto;
        
        // Brazos - movimiento alternado adelante/atrás (rotación en eje X)
        if (nombre.find("cuphead_brazo_derecho") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloBase * CUPHEAD_AMPLITUD_BRAZOS);
            
            // Animar antebrazo derecho si existe
            for (auto* nieto : hijo->hijos) {
                if (nieto && nieto->nombreObjeto.find("cuphead_antebrazo_derecho") != std::string::npos) {
                    nieto->rotacionLocal.x = nieto->rotacionInicial.x + (anguloBase * CUPHEAD_AMPLITUD_ANTEBRAZOS * 0.5f);
                    nieto->actualizarTransformacion();
                }
            }
        }
        else if (nombre.find("cuphead_brazo_izquierdo") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloOpuesto * CUPHEAD_AMPLITUD_BRAZOS);
            
            // Animar antebrazo izquierdo si existe
            for (auto* nieto : hijo->hijos) {
                if (nieto && nieto->nombreObjeto.find("cuphead_antebrazo_izquierdo") != std::string::npos) {
                    nieto->rotacionLocal.x = nieto->rotacionInicial.x + (anguloOpuesto * CUPHEAD_AMPLITUD_ANTEBRAZOS * 0.5f);
                    nieto->actualizarTransformacion();
                }
            }
        }
        
        // Piernas - movimiento pronunciado adelante/atrás (rotación en eje X)
        else if (nombre.find("cuphead_muslo_derecho") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloOpuesto * CUPHEAD_AMPLITUD_MUSLOS);
            
            // Animar pie derecho con movimiento más pronunciado
            for (auto* nieto : hijo->hijos) {
                if (nieto && nieto->nombreObjeto.find("cuphead_pie_derecho") != std::string::npos) {
                    // El pie se dobla más cuando el muslo está hacia atrás (simula rodilla)
                    float factorPie = anguloOpuesto < 0 ? abs(anguloOpuesto) * 1.5f : anguloOpuesto * 0.3f;
                    nieto->rotacionLocal.x = nieto->rotacionInicial.x + (factorPie * CUPHEAD_AMPLITUD_PIES);
                    nieto->actualizarTransformacion();
                }
            }
        }
        else if (nombre.find("cuphead_muslo_izquierdo") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloBase * CUPHEAD_AMPLITUD_MUSLOS);
            
            // Animar pie izquierdo con movimiento más pronunciado
            for (auto* nieto : hijo->hijos) {
                if (nieto && nieto->nombreObjeto.find("cuphead_pie_izquierdo") != std::string::npos) {
                    // El pie se dobla más cuando el muslo está hacia atrás (simula rodilla)
                    float factorPie = anguloBase < 0 ? abs(anguloBase) * 1.5f : anguloBase * 0.3f;
                    nieto->rotacionLocal.x = nieto->rotacionInicial.x + (factorPie * CUPHEAD_AMPLITUD_PIES);
                    nieto->actualizarTransformacion();
                }
            }
        }
        
        hijo->actualizarTransformacion();
    }
    
    // Actualizar transformación del torso
    entidad->actualizarTransformacion();
    
    // Después de aplicar transformaciones, verificar si ya terminó la animación para desactivarla
    if (velocidadMovimiento <= 0.01f && tiemposAnimacion[indiceAnimacion] >= glm::two_pi<float>()) {
        desactivarAnimacion(indiceAnimacion);
        tiemposAnimacion[indiceAnimacion] = 0.0f;
        
        // Restaurar posición Y original
        entidad->posicionLocal.y = entidad->posicionInicial.y;
        entidad->actualizarTransformacion();
    }
}

