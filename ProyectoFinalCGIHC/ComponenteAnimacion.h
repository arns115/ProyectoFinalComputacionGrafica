#pragma once

#include <vector>
#include <string>

class Entidad;

// Componente de animacion para entidades
class ComponenteAnimacion {
public:
    ComponenteAnimacion(Entidad* owner);
    
    // Actualizar animacion numero 1
    void actualizarAnimacion(int indiceAnimacion, float deltaTime, float velocidadMovimiento);
    
    // Control de banderas de animación
    void activarAnimacion(int indiceAnimacion);
    void desactivarAnimacion(int indiceAnimacion);
    bool estaActiva(int indiceAnimacion) const;
    
    // Propiedades de las animacion
	unsigned short banderasAnimacion;  // Short para banderas de animacion (cada bit representa una animacion) (si bit x esta prendido, la animacion x esta activa)
    unsigned int numeroAnimaciones;    // Número de animaciones disponibles
    float tiemposAnimacion[16];        // Tiempo por cada animación
    float velocidadesAnimacion[16];    // Velocidad por cada animación
    
private:
	Entidad* entidad;  // Referencia a la entidad que tiene esta informacion de animacion
    
    // Funciones de animación específicas por entidad (se van a agregar mas)
    void animarIsaac(int indiceAnimacion, float deltaTime, float velocidadMovimiento);
};
