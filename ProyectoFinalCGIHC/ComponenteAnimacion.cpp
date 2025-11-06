#include "ComponenteAnimacion.h"
#include "Entidad.h"
#include "ComponenteFisico.h"
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <gtc/quaternion.hpp>
#include <cmath>
#include <random>
#include <chrono>

// Parametros de las diferentes animaciones
namespace {
    // Isaac
    const float ISAAC_AMPLITUD_PIERNAS = 35.0f;
    const float ISAAC_AMPLITUD_BRAZOS = 25.0f;
    const float ISAAC_AMPLITUD_CABEZA = 5.0f;
    const float ISAAC_BOBBING = 0.08f;
    
    // Variables globales para los calculos de animaciones
    // Cuphead - Caminata
    const float CUPHEAD_AMPLITUD_BRAZOS = 25.0f;         // Amplitud del swing de brazos
    const float CUPHEAD_AMPLITUD_ANTEBRAZOS = 8.0f;      // Movimiento leve de antebrazos
    const float CUPHEAD_AMPLITUD_MUSLOS = 40.0f;         // Movimiento pronunciado de muslos
    const float CUPHEAD_AMPLITUD_PIES = 30.0f;           // Movimiento pronunciado de pies
    const float CUPHEAD_BOBBING = 0.06f;                 // Movimiento vertical del cuerpo
    
    // Cuphead - Salto
    const float CUPHEAD_SALTO_COMPRESION = 0.3f;         // Factor de compresión (escala)
    const float CUPHEAD_SALTO_ROTACION = 360.0f;         // Grados de rotación del mortal
    const float CUPHEAD_SALTO_VELOCIDAD_ROTACION = 2.0f; // Velocidad de rotación (grados por unidad de velocidad vertical)
    const float CUPHEAD_SALTO_FASE_COMPRESION = 0.15f;   // Duración de la fase de compresión en segundos
    const float CUPHEAD_SALTO_ANGULO_BRAZOS = 120.0f;    // Amplitud máxima de brazos durante el mortal
    const float CUPHEAD_SALTO_ANGULO_MUSLOS = 90.0f;     // Amplitud máxima de muslos durante el mortal
    const float CUPHEAD_SALTO_DOBLEZ_BRAZO = 60.0f;      // Grados de doblez de brazos en compresión
    const float CUPHEAD_SALTO_DOBLEZ_MUSLO = 80.0f;      // Grados de doblez de muslos en compresión
    const float CUPHEAD_SALTO_DOBLEZ_PIE = 60.0f;        // Grados de doblez de pies en compresión
    const float CUPHEAD_SALTO_FACTOR_PIE_MAX = 1.5f;     // Factor multiplicador para doblez máximo del pie
    const float CUPHEAD_SALTO_FACTOR_PIE_MIN = 0.3f;     // Factor multiplicador para doblez mínimo del pie
    
    // Hollow - Movimiento dentro de la boss room
    const float HOLLOW_MIN_X = 115.0f;                   // Límite mínimo en X
    const float HOLLOW_MAX_X = 195.0f;                   // Límite máximo en X
    const float HOLLOW_MIN_Z = -150.0f;                  // Límite mínimo en Z
    const float HOLLOW_MAX_Z = -100.0f;                  // Límite máximo en Z
    const float HOLLOW_VELOCIDAD = 2.0f / 10;                 // Velocidad de movimiento
    const float HOLLOW_VELOCIDAD_ROTACION = 90.0f / 10;      // Velocidad de rotación en grados por segundo
    const float HOLLOW_MARGEN_BORDE = 2.0f;             // Margen antes de llegar al borde para cambiar dirección
    const float HOLLOW_AMPLITUD_ONDULACION = 15.0f;     // Amplitud de la ondulación del cuerpo
    const float HOLLOW_FRECUENCIA_ONDULACION = 3.0f / 60;    // Frecuencia de la ondulación
    
    // Variables globales para los calculos de animacion
    bool animacionActiva = false;
    float tiempo = 0.0f;
    float anguloBase = 0.0f;
    float anguloOpuesto = 0.0f;
    
    // Generador de números aleatorios compartido por todas las animaciones
    std::mt19937 generadorAleatorioAnimaciones(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Variables para Hollow
    float hollowDireccionObjetivo = 0.0f;                // Dirección objetivo en grados (rotación Y)
    float hollowDireccionActual = 0.0f;                  // Dirección actual en grados
    Entidad* entidadActual = nullptr;                    // Variable por si se tienen que recorrer varois hijos
    glm::vec3 hollowPosicionActual = glm::vec3(0.0f);   // Posición actual de hollow
    glm::vec3 hollowDireccionAdelante = glm::vec3(0.0f, 0.0f, 1.0f); // Dirección hacia adelante
    glm::vec3 hollowPosicionFutura = glm::vec3(0.0f);   // Posición futura para verificar colisión con bordes
    bool hollowCercaDelBorde = false;                    // Indica si está cerca del borde
    glm::vec3 hollowCentroRectangulo = glm::vec3(0.0f); // Centro del área de movimiento
    glm::vec3 hollowHaciaElCentro = glm::vec3(0.0f);    // Vector hacia el centro
    float hollowAnguloHaciaCentro = 0.0f;                // Ángulo hacia el centro en grados
    float hollowNuevaDireccion = 0.0f;                   // Nueva dirección aleatoria
    float hollowDiferencia = 0.0f;                       // Diferencia entre dirección objetivo y actual
    float hollowRotacionDelta = 0.0f;                    // Delta de rotación por frame
    float hollowRotacionAplicar = 0.0f;                  // Rotación a aplicar en este frame
    float hollowFase = 0.0f;                             // Fase de ondulación
    float hollowAngulo = 0.0f;                           // Ángulo de ondulación
    
    // Variables para ComidaPerro 
    const float COMIDA_PERRO_AMPLITUD = 0.3f;           // Amplitud del movimiento vertical
    const float COMIDA_PERRO_FRECUENCIA = 2.0f / 60;    // Frecuencia del movimiento (ciclos por frame)
    float comidaPerroTiempo = 0.0f;                      // Tiempo acumulado para la animación
    
    // Quaternion para guardar rotación pre-salto
    glm::quat rotacionPreSaltoQuat(1.0f, 0.0f, 0.0f, 0.0f);
}

ComponenteAnimacion::ComponenteAnimacion(Entidad* entidad)
    : entidad(entidad),
    banderasAnimacion(0),
    numeroAnimaciones(0) 
{
    // Inicializar arrays de tiempos y velocidades
    for (int i = 0; i < 16; i++) {
        tiemposAnimacion[i] = 0.0f;
        velocidadesAnimacion[i] = 0.1f;
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

// FuncionPrincipal para actualizar la animacion1
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
    else if (entidad->nombreObjeto == "comida_perro") {
		animarComidaPerro(indiceAnimacion, deltaTime);
    }
    else if (entidad->nombreObjeto == "cuphead_torso") {
        // Índice 0: Animación de caminata
        // Índice 1: Animación de salto
        if (indiceAnimacion == 0) {
            animarCuphead(indiceAnimacion, deltaTime, velocidadMovimiento);
        }
        else if (indiceAnimacion == 1) {
            animarCupheadSalto(indiceAnimacion, deltaTime);
        }

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
    if (entidad == nullptr) {
        return;
    }
    
    animacionActiva = estaActiva(indiceAnimacion);
    
    // Inicializar la configuración específica de Hollow solo al activar la animación
    if (!animacionActiva) {
        // Activar la animación
        activarAnimacion(indiceAnimacion);
        
        // Extraer el ángulo Y actual del quaternion
        glm::vec3 eulerAngles = glm::eulerAngles(entidad->rotacionLocalQuat);
        hollowDireccionActual = glm::degrees(eulerAngles.y);
        hollowDireccionObjetivo = hollowDireccionActual;
    }
    
    // Obtener posición actual
    hollowPosicionActual = entidad->posicionLocal;
    
    // Calcular la dirección hacia adelante
    hollowDireccionAdelante = entidad->rotacionLocalQuat * glm::vec3(0.0f, 0.0f, 1.0f);
    hollowDireccionAdelante.y = 0.0f; 
    if (glm::length(hollowDireccionAdelante) > 0.01f) {
        hollowDireccionAdelante = glm::normalize(hollowDireccionAdelante);
    } else {
        // Si la dirección es muy pequeña, usar una por defecto
        hollowDireccionAdelante = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    
    // Calcular posición futura basada en la dirección actual
    hollowPosicionFutura = hollowPosicionActual + hollowDireccionAdelante * HOLLOW_VELOCIDAD * deltaTime * 2.0f;
    
    // Verificar si está cerca del borde
    hollowCercaDelBorde = (hollowPosicionFutura.x <= HOLLOW_MIN_X + HOLLOW_MARGEN_BORDE ||
                           hollowPosicionFutura.x >= HOLLOW_MAX_X - HOLLOW_MARGEN_BORDE ||
                           hollowPosicionFutura.z <= HOLLOW_MIN_Z + HOLLOW_MARGEN_BORDE ||
                           hollowPosicionFutura.z >= HOLLOW_MAX_Z - HOLLOW_MARGEN_BORDE);
    
    // Cambiar dirección aleatoriamente si está cerca del borde
    if (hollowCercaDelBorde) {
        // Calcular dirección hacia el centro del área
        hollowCentroRectangulo = glm::vec3((HOLLOW_MIN_X + HOLLOW_MAX_X) * 0.5f, 
                                            hollowPosicionActual.y, 
                                            (HOLLOW_MIN_Z + HOLLOW_MAX_Z) * 0.5f);
        hollowHaciaElCentro = glm::normalize(hollowCentroRectangulo - hollowPosicionActual);
        
        // Calcular ángulo hacia el centro
        hollowAnguloHaciaCentro = glm::degrees(atan2(hollowHaciaElCentro.x, hollowHaciaElCentro.z));
        
        // Añadir variación aleatoria de ±90 grados alrededor de la dirección al centro
        std::uniform_real_distribution<float> distribVariacion(-90.0f, 90.0f);
        hollowNuevaDireccion = hollowAnguloHaciaCentro + distribVariacion(generadorAleatorioAnimaciones);
        
        hollowDireccionObjetivo = hollowNuevaDireccion;
    }
    
    // Rotar hacia la dirección objetivo
    hollowDiferencia = hollowDireccionObjetivo - hollowDireccionActual;
    
    // Ajustar a que sea entre -180 y 180
    while (hollowDiferencia > 180.0f) hollowDiferencia -= 360.0f;
    while (hollowDiferencia < -180.0f) hollowDiferencia += 360.0f;
    
    // Aplicar rotación gradual
    if (abs(hollowDiferencia) > 0.1f) {
        hollowRotacionDelta = HOLLOW_VELOCIDAD_ROTACION * deltaTime;
        hollowRotacionAplicar = 0.0f;
        
        if (abs(hollowDiferencia) < hollowRotacionDelta) {
            hollowRotacionAplicar = hollowDiferencia;
            hollowDireccionActual = hollowDireccionObjetivo;
        } else {
            hollowRotacionAplicar = (hollowDiferencia > 0.0f ? hollowRotacionDelta : -hollowRotacionDelta);
            hollowDireccionActual += hollowRotacionAplicar;
        }
        
        // Ajustar a que sea entre -180 y 180
        while (hollowDireccionActual >= 360.0f) hollowDireccionActual -= 360.0f;
        while (hollowDireccionActual < 0.0f) hollowDireccionActual += 360.0f;
        
        // Aplicar la rotación
        entidad->rotacionLocal.y = hollowRotacionAplicar;
    }
    
    // Mover hacia adelante en la dirección actual
    hollowDireccionAdelante = entidad->rotacionLocalQuat * glm::vec3(0.0f, 0.0f, 1.0f);
    hollowDireccionAdelante.y = 0.0f;
    if (glm::length(hollowDireccionAdelante) > 0.01f) {
        hollowDireccionAdelante = glm::normalize(hollowDireccionAdelante);
    } else {
        hollowDireccionAdelante = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    
    entidad->posicionLocal += hollowDireccionAdelante * HOLLOW_VELOCIDAD * deltaTime;
    
    // Limitar la posición dentro de los límites
    entidad->posicionLocal.x = glm::clamp(entidad->posicionLocal.x, HOLLOW_MIN_X, HOLLOW_MAX_X);
    entidad->posicionLocal.z = glm::clamp(entidad->posicionLocal.z, HOLLOW_MIN_Z, HOLLOW_MAX_Z);
    
    // Aplicar ondulación al cuerpo 
    tiempo += deltaTime * HOLLOW_FRECUENCIA_ONDULACION;
    
    // Rotar cada una de las partes de hollow
    entidadActual = entidad->hijos[0];
        
    for (int i = 0; i < 5 && entidadActual != nullptr; i++) {
        // Calcular fase de ondulación para este segmento
        hollowFase = tiempo - (i * 0.3f);
        hollowAngulo = sin(hollowFase) * HOLLOW_AMPLITUD_ONDULACION;
            
        // Aplicar rotación de ondulación 
        entidadActual->rotacionLocalQuat = 
            glm::angleAxis(glm::radians(entidadActual->rotacionInicial.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(entidadActual->rotacionInicial.y + hollowAngulo), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::angleAxis(glm::radians(entidadActual->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f));
            
        entidadActual->rotacionLocal = glm::vec3(0.0f);
            
        // Avanzar al hijo de la entidad
        if (!entidadActual->hijos.empty()) {
            entidadActual = entidadActual->hijos[0];
        } else {
            entidadActual = nullptr;
        }
        
    }
    
    // Actualizar transformación de la cabeza
    entidad->actualizarTransformacion();
}

// Animación de caminata para Cuphead
void ComponenteAnimacion::animarCuphead(int indiceAnimacion, float deltaTime, float velocidadMovimiento)
{
    if (estaActiva(1)) {  // Índice 1 = animación de salto
        // Si la animación de caminata estaba activa, desactivarla
        if (estaActiva(indiceAnimacion)) {
            desactivarAnimacion(indiceAnimacion);
            tiemposAnimacion[indiceAnimacion] = 0.0f;
            
            // Restaurar posición Y original del torso
            entidad->posicionLocal.y = entidad->posicionInicial.y;
            entidad->actualizarTransformacion();
        }
        return;  // No ejecutar animación de caminata mientras se salta
    }
    
    animacionActiva = estaActiva(indiceAnimacion);
    
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
            
            hijo->rotacionLocalQuat = 
                glm::angleAxis(glm::radians(hijo->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                glm::angleAxis(glm::radians(hijo->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                glm::angleAxis(glm::radians(hijo->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
            
            // Animar antebrazo derecho si existe
            for (auto* nieto : hijo->hijos) {
                if (nieto && nieto->nombreObjeto.find("cuphead_antebrazo_derecho") != std::string::npos) {
                    nieto->rotacionLocal.x = nieto->rotacionInicial.x + (anguloBase * CUPHEAD_AMPLITUD_ANTEBRAZOS * 0.5f);
                    
                    nieto->rotacionLocalQuat = 
                        glm::angleAxis(glm::radians(nieto->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                        glm::angleAxis(glm::radians(nieto->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                        glm::angleAxis(glm::radians(nieto->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
                    nieto->actualizarTransformacion();
                }
            }
        }
        else if (nombre.find("cuphead_brazo_izquierdo") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloOpuesto * CUPHEAD_AMPLITUD_BRAZOS);
            
            hijo->rotacionLocalQuat = 
                glm::angleAxis(glm::radians(hijo->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                glm::angleAxis(glm::radians(hijo->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                glm::angleAxis(glm::radians(hijo->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
            
            // Animar antebrazo izquierdo si existe
            for (auto* nieto : hijo->hijos) {
                if (nieto && nieto->nombreObjeto.find("cuphead_antebrazo_izquierdo") != std::string::npos) {
                    nieto->rotacionLocal.x = nieto->rotacionInicial.x + (anguloOpuesto * CUPHEAD_AMPLITUD_ANTEBRAZOS * 0.5f);
                    
                    nieto->rotacionLocalQuat = 
                        glm::angleAxis(glm::radians(nieto->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                        glm::angleAxis(glm::radians(nieto->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                        glm::angleAxis(glm::radians(nieto->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
                    nieto->actualizarTransformacion();
                }
            }
        }
        
        // Piernas - movimiento pronunciado adelante/atrás (rotación en eje X)
        else if (nombre.find("cuphead_muslo_derecho") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloOpuesto * CUPHEAD_AMPLITUD_MUSLOS);
            
            hijo->rotacionLocalQuat = 
                glm::angleAxis(glm::radians(hijo->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                glm::angleAxis(glm::radians(hijo->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                glm::angleAxis(glm::radians(hijo->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
            
            // Animar pie derecho con movimiento más pronunciado
            for (auto* nieto : hijo->hijos) {
                if (nieto && nieto->nombreObjeto.find("cuphead_pie_derecho") != std::string::npos) {
                    // El pie se dobla más cuando el muslo está hacia atrás
                    float factorPie = anguloOpuesto < 0 ? abs(anguloOpuesto) * CUPHEAD_SALTO_FACTOR_PIE_MAX : anguloOpuesto * CUPHEAD_SALTO_FACTOR_PIE_MIN;
                    nieto->rotacionLocal.x = nieto->rotacionInicial.x + (factorPie * CUPHEAD_AMPLITUD_PIES);
                    
                    nieto->rotacionLocalQuat = 
                        glm::angleAxis(glm::radians(nieto->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                        glm::angleAxis(glm::radians(nieto->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                        glm::angleAxis(glm::radians(nieto->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
                    nieto->actualizarTransformacion();
                }
            }
        }
        else if (nombre.find("cuphead_muslo_izquierdo") != std::string::npos) {
            hijo->rotacionLocal.x = hijo->rotacionInicial.x + (anguloBase * CUPHEAD_AMPLITUD_MUSLOS);
            
            hijo->rotacionLocalQuat = 
                glm::angleAxis(glm::radians(hijo->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                glm::angleAxis(glm::radians(hijo->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                glm::angleAxis(glm::radians(hijo->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
            
            // Animar pie izquierdo con movimiento más pronunciado
            for (auto* nieto : hijo->hijos) {
                if (nieto && nieto->nombreObjeto.find("cuphead_pie_izquierdo") != std::string::npos) {
                    // El pie se dobla más cuando el muslo está hacia atrás 
                    float factorPie = anguloBase < 0 ? abs(anguloBase) * CUPHEAD_SALTO_FACTOR_PIE_MAX : anguloBase * CUPHEAD_SALTO_FACTOR_PIE_MIN;
                    nieto->rotacionLocal.x = nieto->rotacionInicial.x + (factorPie * CUPHEAD_AMPLITUD_PIES);
                    
                    nieto->rotacionLocalQuat = 
                        glm::angleAxis(glm::radians(nieto->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                        glm::angleAxis(glm::radians(nieto->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                        glm::angleAxis(glm::radians(nieto->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
                    nieto->actualizarTransformacion();
                }
            }
        }
        
        hijo->actualizarTransformacion();
    }
    
    
    // Después de aplicar transformaciones, verificar si ya terminó la animación para desactivarla
    if (velocidadMovimiento <= 0.01f && tiemposAnimacion[indiceAnimacion] >= glm::two_pi<float>()) {
        desactivarAnimacion(indiceAnimacion);
        tiemposAnimacion[indiceAnimacion] = 0.0f;
        
        // Restaurar posición Y original
        entidad->posicionLocal.y = entidad->posicionInicial.y;
    }
    entidad->actualizarTransformacion();

}

// Animación de salto con mortal para Cuphead
void ComponenteAnimacion::animarCupheadSalto(int indiceAnimacion, float deltaTime)
{
    bool animacionActiva = estaActiva(indiceAnimacion);
    
    // Si la animación no está activa, no hacer nada
    if (!animacionActiva) {
        return;
    }
    
    // Verificar si la entidad tiene componente físico
    if (entidad->fisica == nullptr) {
        desactivarAnimacion(indiceAnimacion);
        return;
    }
    
    // Al inicio de la animación (tiempo = 0), guardar la rotación actual
    if (tiemposAnimacion[indiceAnimacion] == 0.0f) {
        rotacionPreSaltoQuat = entidad->rotacionLocalQuat;
    }
    
    // Verificar si el personaje ha aterrizado
    if (entidad->fisica->estaEnSuelo() && tiemposAnimacion[indiceAnimacion] > 0.0f) {
        // Terminar la animación
        desactivarAnimacion(indiceAnimacion);
        tiemposAnimacion[indiceAnimacion] = 0.0f;
        
        // Restaurar todas las transformaciones de los hijos
        for (auto* hijo : entidad->hijos) {
            if (hijo == nullptr) continue;
            
			hijo->posicionLocal = hijo->posicionInicial;
            hijo->rotacionLocal = hijo->rotacionInicial;
            hijo->rotacionLocalQuat = hijo->rotacionInicialQuat;
            hijo->escalaLocal = hijo->escalaInicial;
            hijo->actualizarTransformacion();

            // Restaurar también los nietos (antebrazos y pies)
            for (auto* nieto : hijo->hijos) {
                if (nieto == nullptr) continue;
				nieto->posicionLocal = nieto->posicionInicial;
                nieto->rotacionLocal = nieto->rotacionInicial;
                nieto->rotacionLocalQuat = nieto->rotacionInicialQuat;
                nieto->escalaLocal = nieto->escalaInicial;
                nieto->actualizarTransformacion();
            }
        }
        
        // Restaurar a la rotación antes del salto
        entidad->rotacionLocalQuat = rotacionPreSaltoQuat;
        entidad->escalaLocal = entidad->escalaInicial;
        entidad->actualizarTransformacion();
        
        return;
    }
    
    // Acumular tiempo solo si no está en el suelo
    tiemposAnimacion[indiceAnimacion] += deltaTime;
    
    // Fase 1: Compresión 
    if (tiemposAnimacion[indiceAnimacion] < CUPHEAD_SALTO_FASE_COMPRESION && !entidad->fisica->estaEnSuelo()) {
        float factorCompresion = tiemposAnimacion[indiceAnimacion] / CUPHEAD_SALTO_FASE_COMPRESION; // 0.0 a 1.0
        
        // Comprimir el torso 
        float escalaY = 1.0f - (factorCompresion * CUPHEAD_SALTO_COMPRESION);
        float escalaXZ = 1.0f + (factorCompresion * CUPHEAD_SALTO_COMPRESION * 0.5f);
        
        entidad->escalaLocal = glm::vec3(
            entidad->escalaInicial.x * escalaXZ,
            entidad->escalaInicial.y * escalaY,
            entidad->escalaInicial.z * escalaXZ
        );
        
        // Comprimir extremidades hacia el cuerpo
        for (auto* hijo : entidad->hijos) {
            if (hijo == nullptr) continue;
            
            const std::string& nombre = hijo->nombreObjeto;
            
            // Brazos hacia adentro
            if (nombre.find("cuphead_brazo") != std::string::npos) {
                hijo->rotacionLocal.z = hijo->rotacionInicial.z + (factorCompresion * CUPHEAD_SALTO_DOBLEZ_BRAZO * 
                    (nombre.find("derecho") != std::string::npos ? 1.0f : -1.0f));
                
                hijo->rotacionLocalQuat = 
                    glm::angleAxis(glm::radians(hijo->rotacionLocal.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                    glm::angleAxis(glm::radians(hijo->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                    glm::angleAxis(glm::radians(hijo->rotacionInicial.x), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            // Piernas dobladas
            else if (nombre.find("cuphead_muslo") != std::string::npos) {
                hijo->rotacionLocal.x = hijo->rotacionInicial.x - (factorCompresion * CUPHEAD_SALTO_DOBLEZ_MUSLO);
                
                hijo->rotacionLocalQuat = 
                    glm::angleAxis(glm::radians(hijo->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                    glm::angleAxis(glm::radians(hijo->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                    glm::angleAxis(glm::radians(hijo->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
                
                // Doblar los pies más
                for (auto* nieto : hijo->hijos) {
                    if (nieto && nieto->nombreObjeto.find("cuphead_pie") != std::string::npos) {
                        nieto->rotacionLocal.x = nieto->rotacionInicial.x + (factorCompresion * CUPHEAD_SALTO_DOBLEZ_PIE);
                        
                        nieto->rotacionLocalQuat = 
                            glm::angleAxis(glm::radians(nieto->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                            glm::angleAxis(glm::radians(nieto->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                            glm::angleAxis(glm::radians(nieto->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
                        nieto->actualizarTransformacion();
                    }
                }
            }
            
            hijo->actualizarTransformacion();
        }
    }
    // Fase 2: Mortal en el aire
    else {
        // Descomprimir completamente
        entidad->escalaLocal = entidad->escalaInicial;
        
        // Calcular rotación basada en la velocidad vertical acumulada
        // La rotación aumenta mientras el personaje está en el aire
        float velocidadVertical = entidad->fisica->velocidad.y;
        float tiempoEnAire = tiemposAnimacion[indiceAnimacion] - CUPHEAD_SALTO_FASE_COMPRESION;
        
        // Calcular rotación acumulada (más rápida al inicio, más lenta al final)
        float rotacionActual = tiempoEnAire * CUPHEAD_SALTO_VELOCIDAD_ROTACION * abs(velocidadVertical);
        
        // Limitar la rotación a un máximo para evitar giros excesivos
        if (rotacionActual > CUPHEAD_SALTO_ROTACION) {
            rotacionActual = CUPHEAD_SALTO_ROTACION;
        }
        
        // Aplicar rotación del mortal sobre la rotación pre-salto
        glm::quat rotacionMortal = glm::angleAxis(glm::radians(rotacionActual), glm::vec3(1.0f, 0.0f, 0.0f));
        entidad->rotacionLocalQuat = rotacionPreSaltoQuat * rotacionMortal;
        
        // Factor de progreso del mortal (0.0 a 1.0 basado en la rotación)
        float factorMortal = rotacionActual / CUPHEAD_SALTO_ROTACION;
        
        // Extender extremidades gradualmente durante el mortal
        for (auto* hijo : entidad->hijos) {
            if (hijo == nullptr) continue;
            
            const std::string& nombre = hijo->nombreObjeto;
            
            // Brazos: primero hacia adelante, luego hacia atrás siguiendo el mortal
            if (nombre.find("cuphead_brazo") != std::string::npos) {
                float anguloBrazo = sin(factorMortal * glm::pi<float>()) * CUPHEAD_SALTO_ANGULO_BRAZOS;
                hijo->rotacionLocal.x = hijo->rotacionInicial.x + anguloBrazo;
                hijo->rotacionLocal.z = hijo->rotacionInicial.z;
                
                hijo->rotacionLocalQuat = 
                    glm::angleAxis(glm::radians(hijo->rotacionLocal.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                    glm::angleAxis(glm::radians(hijo->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                    glm::angleAxis(glm::radians(hijo->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            // Piernas: extender durante el mortal
            else if (nombre.find("cuphead_muslo") != std::string::npos) {
                float anguloMuslo = sin(factorMortal * glm::pi<float>()) * -CUPHEAD_SALTO_ANGULO_MUSLOS;
                hijo->rotacionLocal.x = hijo->rotacionInicial.x + anguloMuslo;
                
                hijo->rotacionLocalQuat = 
                    glm::angleAxis(glm::radians(hijo->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                    glm::angleAxis(glm::radians(hijo->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                    glm::angleAxis(glm::radians(hijo->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
                
                // Extender los pies
                for (auto* nieto : hijo->hijos) {
                    if (nieto && nieto->nombreObjeto.find("cuphead_pie") != std::string::npos) {
                        float anguloPie = (1.0f - factorMortal) * CUPHEAD_SALTO_DOBLEZ_PIE;
                        nieto->rotacionLocal.x = nieto->rotacionInicial.x + anguloPie;
                        
                        nieto->rotacionLocalQuat = 
                            glm::angleAxis(glm::radians(nieto->rotacionInicial.z), glm::vec3(0.0f, 0.0f, 1.0f)) * 
                            glm::angleAxis(glm::radians(nieto->rotacionInicial.y), glm::vec3(0.0f, 1.0f, 0.0f)) * 
                            glm::angleAxis(glm::radians(nieto->rotacionLocal.x), glm::vec3(1.0f, 0.0f, 0.0f));
                        nieto->actualizarTransformacion();
                    }
                }
            }
            
            hijo->actualizarTransformacion();
        }
    }
    
    // Actualizar transformación del torso
    entidad->actualizarTransformacion();
}

// Animacion para el item de la secret room (sube y baja todo el tiempo)
void ComponenteAnimacion::animarComidaPerro(int indiceAnimacion, float deltaTime) {
    if (entidad == nullptr) {
        return;
    }
    
    animacionActiva = estaActiva(indiceAnimacion);
    
    // Activar la animación si no está activa
    if (!animacionActiva) {
        activarAnimacion(indiceAnimacion);
    }
    
    // Acumular tiempo para el movimiento sinusoidal
    comidaPerroTiempo += deltaTime * COMIDA_PERRO_FRECUENCIA;
    
    // Mantener el tiempo en el rango 0 y 2 * pi
    if (comidaPerroTiempo >= glm::two_pi<float>()) {
        comidaPerroTiempo -= glm::two_pi<float>();
    }

    // Aplicar el desplazamiento 
    entidad->posicionLocal.y = entidad->posicionInicial.y + sin(comidaPerroTiempo) * COMIDA_PERRO_AMPLITUD;
    
    // Actualizar transformación
    entidad->actualizarTransformacion();
}

