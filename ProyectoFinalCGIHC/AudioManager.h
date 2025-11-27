#pragma once

#include "include/miniaudio.h"
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <glm.hpp>

// Estructura para representar un sonido ambiental con posición 3D
struct SonidoAmbiental {
    ma_sound sound;
    glm::vec3 posicion;
    bool activo;
    bool enLoop;
    std::string nombre;
};

// Estructura para representar un sonido normal 
struct SonidoNormal {
    ma_sound sound;
    glm::vec3 posicion;
    bool activo;
    std::string nombre;
};

// Clase para gestionar todo el audio del juego
class AudioManager {
public:
    // Constructor y destructor
    AudioManager();
    ~AudioManager();

    // Inicializar el motor de audio
    bool inicializar();

    // Limpiar y liberar recursos
    void limpiar();

    // ==================== SOUNDTRACK (Música de fondo) ====================
    // 
    // Cargar una pista de música (soundtrack) desde un archivo
    bool cargarSoundtrack(const std::string& nombre, const std::string& rutaArchivo);

    // Reproducir soundtrack en loop indefinido
    bool reproducirSoundtrack(const std::string& nombre, float volumen = 1.0f);

    // Detener el soundtrack actual
    void detenerSoundtrack();

    // Pausar/reanudar soundtrack
    void pausarSoundtrack();
    void reanudarSoundtrack();

    // Ajustar volumen del soundtrack (0.0 a 1.0)
    void setVolumenSoundtrack(float volumen);

    // ==================== SONIDOS AMBIENTALES (3D) ====================
    // 
    // Cargar un sonido ambiental desde un archivo
    bool cargarSonidoAmbiental(const std::string& nombre, const std::string& rutaArchivo);

    // Reproducir sonido ambiental en una posición 3D con loop
    bool reproducirSonidoAmbiental(const std::string& nombre, const glm::vec3& posicion,
        float volumen = 1.0f, bool loop = true);

    // Detener un sonido ambiental específico
    void detenerSonidoAmbiental(const std::string& nombre);

    // Detener todos los sonidos ambientales
    void detenerTodosSonidosAmbientales();

    // Detener todos los sonidos ambientales que contengan un texto en su nombre
    void detenerSonidosAmbientalesPorPatron(const std::string& patron);

    // Actualizar posición del listener (cámara/jugador)
    void actualizarPosicionListener(const glm::vec3& posicion, const glm::vec3& direccion);

    // Actualizar posición de un sonido ambiental existente
    bool actualizarPosicionSonidoAmbiental(const std::string& nombre, const glm::vec3& nuevaPosicion);

    // ==================== SONIDOS NORMALES (One-shot) ====================
    // 
    // Cargar un sonido normal desde un archivo
    bool cargarSonidoNormal(const std::string& nombre, const std::string& rutaArchivo);

    // Reproducir sonido normal (no espacial, one-shot)
    bool reproducirSonidoNormal(const std::string& nombre, float volumen = 1.0f);

    // Reproducir sonido normal con delay (útil para efectos secuenciales)
    bool reproducirSonidoNormalConDelay(const std::string& nombre, float delay, float volumen = 1.0f);

    // ==================== UTILIDADES ====================
    // 
    // Verificar si el motor está inicializado
    bool estaInicializado() const { return inicializado; }

    // Obtener volumen maestro
    float getVolumenMaestro() const { return volumenMaestro; }

    // Establecer volumen maestro (afecta a todos los sonidos)
    void setVolumenMaestro(float volumen);

    // Limpiar sonidos inactivos (liberar memoria)
    void limpiarSonidosInactivos();

private:
    // Motor de audio de miniaudio
    ma_engine engine;
    bool inicializado;

    // Volumen maestro
    float volumenMaestro;

    // ==================== SOUNDTRACK ====================

    ma_sound* soundtrackActual;
    std::string nombreSoundtrackActual;
    std::map<std::string, std::string> rutasSoundtracks; // nombre -> ruta archivo

    // ==================== SONIDOS AMBIENTALES ====================

    std::vector<SonidoAmbiental*> sonidosAmbientales;
    std::map<std::string, std::string> rutasSonidosAmbientales; // nombre -> ruta archivo
    std::mutex mutexAmbientales;

    // ==================== SONIDOS NORMALES ====================

    std::vector<SonidoNormal*> sonidosNormales;
    std::map<std::string, std::string> rutasSonidosNormales; // nombre -> ruta archivo
    std::mutex mutexNormales;

    // Encontrar un sonido ambiental por nombre
    SonidoAmbiental* encontrarSonidoAmbiental(const std::string& nombre);

    // Encontrar un slot libre para sonido normal
    SonidoNormal* encontrarSlotLibreSonidoNormal();

    // Thread para reproducir sonido con delay
    void threadReproducirConDelay(const std::string& nombre, float delay, float volumen);
};