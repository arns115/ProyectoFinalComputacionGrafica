#pragma once

#include <vector>
#include <string>
#include <glm.hpp>
#include <iostream>
#include <fstream>

#define MAX_FRAMES 100 //Número de cuadros máximos


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
    
    int i_max_steps = 30; //Número de pasos entre cuadros para interpolación, a mayor número , más lento será el movimiento
    int i_curr_steps = 0;
    typedef struct _frame
    {
        // Importante notar que rotaciones se haran en orden x, y, z al momento de animar, por lo tanto hay que tener cuidado con como se definen los keyframes ya que si no se haran mal las rotaciones
        //Variables para GUARDAR Key Frames
        float movX;		//Variable para PosicionX
        float movY;		//Variable para PosicionY
        float movZ;		//Variable para PosicionZ
        float movXInc;		//Variable para IncrementoX
        float movYInc;		//Variable para IncrementoY
        float movZInc;		//Variable para IncrementoZ
        float giroY;		//Variable para giroY
        float giroX;		//Variable para giroX
        float giroZ;		//Variable para giroZ
        float giroXInc;		//Variable para IncrementogiroX
        float giroYInc;		//Variable para IncrementogiroY
        float giroZInc;		//Variable para IncrementogiroZ
    }FRAME;

    FRAME KeyFrame[MAX_FRAMES];
    int FrameIndex = 0;			//El número de cuadros guardados actualmente desde 0 para no sobreescribir
    bool play = false;
    int playIndex = 0;

    void cargarKeyframes(void);
    void interpolation(void);
    void animateKeyframes(void);

private:
	Entidad* entidad;  // Referencia a la entidad que tiene esta informacion de animacion
    
    // Funciones de animación específicas por entidad
    void animarIsaac(int indiceAnimacion, float deltaTime, float velocidadMovimiento);
    void animarHollow(int indiceAnimacion, float deltaTime);
    void animarCuphead(int indiceAnimacion, float deltaTime, float velocidadMovimiento);
    void animarCupheadSalto(int indiceAnimacion, float deltaTime);  // Animación de salto
	void animarComidaPerro(int indiceAnimacion, float deltaTime); // Animacion de comida de perro(item del juego de isaac)
	void animarPuerta(int indiceAnimacion, float deltaTime); // Animacion de la puerta secreta
};
