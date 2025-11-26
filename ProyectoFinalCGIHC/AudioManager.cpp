#include "AudioManager.h"
#include <iostream>
#include <chrono>
//#include <algorithm>

// Constructor
AudioManager::AudioManager()
    : inicializado(false)
    , volumenMaestro(1.0f)
    , soundtrackActual(nullptr)
{
}

// Destructor
AudioManager::~AudioManager()
{
    limpiar();
}

// Inicializar el motor de audio
bool AudioManager::inicializar()
{
    if (inicializado) {
        std::cout << "[AudioManager] Ya está inicializado." << std::endl;
        return true;
    }
    
    // Inicializar el engine de miniaudio
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cerr << "[AudioManager] Error al inicializar el motor de audio: " << result << std::endl;
        return false;
    }
    
    inicializado = true;
    std::cout << "[AudioManager] Motor de audio inicializado correctamente." << std::endl;
    
    return true;
}

// Limpiar y liberar recursos
void AudioManager::limpiar()
{
    if (!inicializado) {
        return;
    }
    
    // Detener y liberar soundtrack
    detenerSoundtrack();
    if (soundtrackActual != nullptr) {
        ma_sound_uninit(soundtrackActual);
        delete soundtrackActual;
        soundtrackActual = nullptr;
    }
    
    // Limpiar sonidos ambientales
    detenerTodosSonidosAmbientales();
    {
        std::lock_guard<std::mutex> lock(mutexAmbientales);
        for (auto* sonido : sonidosAmbientales) {
            ma_sound_uninit(&sonido->sound);
            delete sonido;
        }
        sonidosAmbientales.clear();
    }
    
    // Limpiar sonidos normales
    {
        std::lock_guard<std::mutex> lock(mutexNormales);
        for (auto* sonido : sonidosNormales) {
            if (sonido->activo) {
                ma_sound_uninit(&sonido->sound);
            }
            delete sonido;
        }
        sonidosNormales.clear();
    }
    
    // Desinicializar el engine
    ma_engine_uninit(&engine);
    inicializado = false;
    
    std::cout << "[AudioManager] Recursos de audio liberados." << std::endl;
}

// ==================== SOUNDTRACK (Música de fondo) ====================

bool AudioManager::cargarSoundtrack(const std::string& nombre, const std::string& rutaArchivo)
{
    if (!inicializado) {
        std::cerr << "[AudioManager] Motor no inicializado." << std::endl;
        return false;
    }
    
    // Guardar la ruta para uso posterior
    rutasSoundtracks[nombre] = rutaArchivo;
    
    std::cout << "[AudioManager] Soundtrack '" << nombre << "' cargado: " << rutaArchivo << std::endl;
    return true;
}

bool AudioManager::reproducirSoundtrack(const std::string& nombre, float volumen)
{
    if (!inicializado) {
        std::cerr << "[AudioManager] Motor no inicializado." << std::endl;
        return false;
    }
    
    // Verificar si el soundtrack existe
    auto it = rutasSoundtracks.find(nombre);
    if (it == rutasSoundtracks.end()) {
        std::cerr << "[AudioManager] Soundtrack '" << nombre << "' no encontrado." << std::endl;
        return false;
    }
    
    // Detener soundtrack actual si existe
    detenerSoundtrack();
    
    // Crear nuevo soundtrack
    soundtrackActual = new ma_sound();
    
    // Inicializar el sonido con loop activado
    ma_uint32 flags = MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_NO_SPATIALIZATION;
    ma_result result = ma_sound_init_from_file(&engine, it->second.c_str(), flags, NULL, NULL, soundtrackActual);
    
    if (result != MA_SUCCESS) {
        std::cerr << "[AudioManager] Error al cargar soundtrack '" << nombre << "': " << result << std::endl;
        delete soundtrackActual;
        soundtrackActual = nullptr;
        return false;
    }
    
    // Configurar loop infinito
    ma_sound_set_looping(soundtrackActual, MA_TRUE);
    
    // Configurar volumen
    ma_sound_set_volume(soundtrackActual, volumen * volumenMaestro);
    
    // Iniciar reproducción
    ma_sound_start(soundtrackActual);
    
    nombreSoundtrackActual = nombre;
    
    std::cout << "[AudioManager] Reproduciendo soundtrack: " << nombre << std::endl;
    return true;
}

void AudioManager::detenerSoundtrack()
{
    if (soundtrackActual != nullptr && ma_sound_is_playing(soundtrackActual)) {
        ma_sound_stop(soundtrackActual);
    }
}

void AudioManager::pausarSoundtrack()
{
    if (soundtrackActual != nullptr && ma_sound_is_playing(soundtrackActual)) {
        ma_sound_stop(soundtrackActual);
        std::cout << "[AudioManager] Soundtrack pausado." << std::endl;
    }
}

void AudioManager::reanudarSoundtrack()
{
    if (soundtrackActual != nullptr && !ma_sound_is_playing(soundtrackActual)) {
        ma_sound_start(soundtrackActual);
        std::cout << "[AudioManager] Soundtrack reanudado." << std::endl;
    }
}

void AudioManager::setVolumenSoundtrack(float volumen)
{
    if (soundtrackActual != nullptr) {
        ma_sound_set_volume(soundtrackActual, volumen * volumenMaestro);
    }
}

// ==================== SONIDOS AMBIENTALES (3D) ====================

bool AudioManager::cargarSonidoAmbiental(const std::string& nombre, const std::string& rutaArchivo)
{
    if (!inicializado) {
        std::cerr << "[AudioManager] Motor no inicializado." << std::endl;
        return false;
    }
    
    // Guardar la ruta para uso posterior
    rutasSonidosAmbientales[nombre] = rutaArchivo;
    
    std::cout << "[AudioManager] Sonido ambiental '" << nombre << "' cargado: " << rutaArchivo << std::endl;
    return true;
}

bool AudioManager::reproducirSonidoAmbiental(const std::string& nombre, const glm::vec3& posicion, 
                                             float volumen, bool loop)
{
    if (!inicializado) {
        std::cerr << "[AudioManager] Motor no inicializado." << std::endl;
        return false;
    }
    
    // Verificar si el sonido existe
    auto it = rutasSonidosAmbientales.find(nombre);
    if (it == rutasSonidosAmbientales.end()) {
        std::cerr << "[AudioManager] Sonido ambiental '" << nombre << "' no encontrado." << std::endl;
        return false;
    }
    
    // Crear nueva instancia de sonido ambiental
    SonidoAmbiental* sonidoAmb = new SonidoAmbiental();
    sonidoAmb->nombre = nombre;
    sonidoAmb->posicion = posicion;
    sonidoAmb->activo = true;
    sonidoAmb->enLoop = loop;
    
    // Inicializar el sonido con espacialización 3D
    ma_uint32 flags = MA_SOUND_FLAG_DECODE; // No usar streaming para sonidos cortos
    ma_result result = ma_sound_init_from_file(&engine, it->second.c_str(), flags, NULL, NULL, &sonidoAmb->sound);
    
    if (result != MA_SUCCESS) {
        std::cerr << "[AudioManager] Error al cargar sonido ambiental '" << nombre << "': " << result << std::endl;
        delete sonidoAmb;
        return false;
    }
    
    // Configurar espacialización
    ma_sound_set_positioning(&sonidoAmb->sound, ma_positioning_absolute);
    ma_sound_set_position(&sonidoAmb->sound, posicion.x, posicion.y, posicion.z);
    
    // Configurar atenuación (distancia de audición)
    ma_sound_set_min_distance(&sonidoAmb->sound, 1.0f);
    ma_sound_set_max_distance(&sonidoAmb->sound, 50.0f);
    ma_sound_set_attenuation_model(&sonidoAmb->sound, ma_attenuation_model_inverse);
    
    // Configurar loop
    ma_sound_set_looping(&sonidoAmb->sound, loop ? MA_TRUE : MA_FALSE);
    
    // Configurar volumen
    ma_sound_set_volume(&sonidoAmb->sound, volumen * volumenMaestro + 0.4);
    
    // Iniciar reproducción
    ma_sound_start(&sonidoAmb->sound);
    
    // Agregar a la lista de sonidos ambientales
    {
        std::lock_guard<std::mutex> lock(mutexAmbientales);
        sonidosAmbientales.push_back(sonidoAmb);
    }
    
    std::cout << "[AudioManager] Reproduciendo sonido ambiental: " << nombre 
              << " en posición (" << posicion.x << ", " << posicion.y << ", " << posicion.z << ")" << std::endl;
    
    return true;
}

void AudioManager::detenerSonidoAmbiental(const std::string& nombre)
{
    std::lock_guard<std::mutex> lock(mutexAmbientales);
    
    for (auto* sonido : sonidosAmbientales) {
        if (sonido->nombre == nombre && sonido->activo) {
            ma_sound_stop(&sonido->sound);
            sonido->activo = false;
            std::cout << "[AudioManager] Sonido ambiental detenido: " << nombre << std::endl;
        }
    }
}

void AudioManager::detenerTodosSonidosAmbientales()
{
    std::lock_guard<std::mutex> lock(mutexAmbientales);
    
    for (auto* sonido : sonidosAmbientales) {
        if (sonido->activo) {
            ma_sound_stop(&sonido->sound);
            sonido->activo = false;
        }
    }
    
    std::cout << "[AudioManager] Todos los sonidos ambientales detenidos." << std::endl;
}

void AudioManager::actualizarPosicionListener(const glm::vec3& posicion, const glm::vec3& direccion)
{
    if (!inicializado) {
        return;
    }
    
    // Actualizar posición del listener (cámara/jugador)
    ma_engine_listener_set_position(&engine, 0, posicion.x, posicion.y, posicion.z);
    
    // Actualizar dirección del listener
    ma_vec3f forward = { direccion.x, direccion.y, direccion.z };
    ma_vec3f up = { 0.0f, 1.0f, 0.0f };
    ma_engine_listener_set_direction(&engine, 0, forward.x, forward.y, forward.z);
    ma_engine_listener_set_world_up(&engine, 0, up.x, up.y, up.z);
}

// ==================== SONIDOS NORMALES (One-shot) ====================

bool AudioManager::cargarSonidoNormal(const std::string& nombre, const std::string& rutaArchivo)
{
    if (!inicializado) {
        std::cerr << "[AudioManager] Motor no inicializado." << std::endl;
        return false;
    }
    
    // Guardar la ruta para uso posterior
    rutasSonidosNormales[nombre] = rutaArchivo;
    
    std::cout << "[AudioManager] Sonido normal '" << nombre << "' cargado: " << rutaArchivo << std::endl;
    return true;
}

bool AudioManager::reproducirSonidoNormal(const std::string& nombre, float volumen)
{
    if (!inicializado) {
        std::cerr << "[AudioManager] Motor no inicializado." << std::endl;
        return false;
    }
    
    // Verificar si el sonido existe
    auto it = rutasSonidosNormales.find(nombre);
    if (it == rutasSonidosNormales.end()) {
        std::cerr << "[AudioManager] Sonido normal '" << nombre << "' no encontrado." << std::endl;
        return false;
    }
    
    // Buscar un slot libre o crear uno nuevo
    SonidoNormal* sonidoNorm = encontrarSlotLibreSonidoNormal();
    
    if (sonidoNorm == nullptr) {
        // Crear nuevo slot
        sonidoNorm = new SonidoNormal();
        std::lock_guard<std::mutex> lock(mutexNormales);
        sonidosNormales.push_back(sonidoNorm);
    }
    
    sonidoNorm->nombre = nombre;
    sonidoNorm->activo = true;
    
    // Inicializar el sonido sin espacialización
    ma_uint32 flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION;
    ma_result result = ma_sound_init_from_file(&engine, it->second.c_str(), flags, NULL, NULL, &sonidoNorm->sound);
    
    if (result != MA_SUCCESS) {
        std::cerr << "[AudioManager] Error al cargar sonido normal '" << nombre << "': " << result << std::endl;
        sonidoNorm->activo = false;
        return false;
    }
    
    // Configurar volumen
    ma_sound_set_volume(&sonidoNorm->sound, volumen * volumenMaestro);
    
    // No configurar loop (one-shot)
    ma_sound_set_looping(&sonidoNorm->sound, MA_FALSE);
    
    // Iniciar reproducción
    ma_sound_start(&sonidoNorm->sound);
    
    std::cout << "[AudioManager] Reproduciendo sonido normal: " << nombre << std::endl;
    
    return true;
}

bool AudioManager::reproducirSonidoNormalConDelay(const std::string& nombre, float delay, float volumen)
{
    if (!inicializado) {
        std::cerr << "[AudioManager] Motor no inicializado." << std::endl;
        return false;
    }
    
    // Verificar si el sonido existe
    auto it = rutasSonidosNormales.find(nombre);
    if (it == rutasSonidosNormales.end()) {
        std::cerr << "[AudioManager] Sonido normal '" << nombre << "' no encontrado." << std::endl;
        return false;
    }
    
    // Crear un thread para reproducir el sonido después del delay
    std::thread([this, nombre, delay, volumen]() {
        threadReproducirConDelay(nombre, delay, volumen);
    }).detach();
    
    std::cout << "[AudioManager] Sonido normal '" << nombre << "' programado con delay de " 
              << delay << " segundos." << std::endl;
    
    return true;
}

// ==================== UTILIDADES ====================

void AudioManager::setVolumenMaestro(float volumen)
{
    volumenMaestro = (volumen < 0.0f) ? 0.0f : (volumen > 1.0f) ? 1.0f : volumen;
    
    // Actualizar volumen del soundtrack actual
    if (soundtrackActual != nullptr) {
        ma_sound_set_volume(soundtrackActual, volumenMaestro);
    }
    
    std::cout << "[AudioManager] Volumen maestro establecido a: " << volumenMaestro << std::endl;
}

void AudioManager::limpiarSonidosInactivos()
{
    // Limpiar sonidos ambientales inactivos
    {
        std::lock_guard<std::mutex> lock(mutexAmbientales);
        auto it = sonidosAmbientales.begin();
        while (it != sonidosAmbientales.end()) {
            if (!(*it)->activo || !ma_sound_is_playing(&(*it)->sound)) {
                ma_sound_uninit(&(*it)->sound);
                delete *it;
                it = sonidosAmbientales.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // Limpiar sonidos normales inactivos
    {
        std::lock_guard<std::mutex> lock(mutexNormales);
        for (auto* sonido : sonidosNormales) {
            if (sonido->activo && !ma_sound_is_playing(&sonido->sound)) {
                ma_sound_uninit(&sonido->sound);
                sonido->activo = false;
            }
        }
    }
}

// ==================== FUNCIONES AUXILIARES ====================

SonidoAmbiental* AudioManager::encontrarSonidoAmbiental(const std::string& nombre)
{
    std::lock_guard<std::mutex> lock(mutexAmbientales);
    
    for (auto* sonido : sonidosAmbientales) {
        if (sonido->nombre == nombre) {
            return sonido;
        }
    }
    
    return nullptr;
}

SonidoNormal* AudioManager::encontrarSlotLibreSonidoNormal()
{
    std::lock_guard<std::mutex> lock(mutexNormales);
    
    for (auto* sonido : sonidosNormales) {
        if (!sonido->activo) {
            return sonido;
        }
    }
    
    return nullptr;
}

void AudioManager::threadReproducirConDelay(const std::string& nombre, float delay, float volumen)
{
    // Esperar el delay especificado
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay * 1000)));
    
    // Reproducir el sonido
    reproducirSonidoNormal(nombre, volumen);
}
