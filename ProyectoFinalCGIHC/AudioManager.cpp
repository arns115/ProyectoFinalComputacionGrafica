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
    std::lock_guard<std::mutex> lock(mutexAmbientales);

    // Buscar la ruta del archivo
    auto itRuta = rutasSonidosAmbientales.find(nombre);
    if (itRuta == rutasSonidosAmbientales.end()) {
        std::cerr << "[AudioManager] Error: Sonido ambiental no cargado: " << nombre << std::endl;
        return false;
    }

    // Buscar un slot libre en el vector
    SonidoAmbiental* slotLibre = nullptr;
    for (auto* sonido : sonidosAmbientales) {
        if (!sonido->activo) {
            slotLibre = sonido;
            break;
        }
    }

    // Si no hay slot libre, crear uno nuevo
    if (slotLibre == nullptr) {
        slotLibre = new SonidoAmbiental();
        sonidosAmbientales.push_back(slotLibre);

        // Inicializar el sonido con la ruta del archivo
        if (ma_sound_init_from_file(&engine, itRuta->second.c_str(),
            MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, NULL, NULL, &slotLibre->sound) != MA_SUCCESS) {
            std::cerr << "[AudioManager] Error al crear instancia de sonido: " << nombre << std::endl;
            return false;
        }

        // Configurar espacialización
        ma_sound_set_spatialization_enabled(&slotLibre->sound, MA_TRUE);
        ma_sound_set_positioning(&slotLibre->sound, ma_positioning_absolute);
        ma_sound_set_attenuation_model(&slotLibre->sound, ma_attenuation_model_linear);
        ma_sound_set_rolloff(&slotLibre->sound, 1.0f);
        ma_sound_set_min_distance(&slotLibre->sound, 5.0f);
        ma_sound_set_max_distance(&slotLibre->sound, 50.0f);
    }

    // Configurar el slot
    slotLibre->nombre = nombre;
    slotLibre->posicion = posicion;
    slotLibre->activo = true;
    slotLibre->enLoop = loop;

    // Configurar posición
    ma_sound_set_position(&slotLibre->sound, posicion.x, posicion.y, posicion.z);

    // Configurar volumen
    ma_sound_set_volume(&slotLibre->sound, volumen * volumenMaestro);

    // Configurar loop
    ma_sound_set_looping(&slotLibre->sound, loop ? MA_TRUE : MA_FALSE);

    // Reproducir
    ma_sound_start(&slotLibre->sound);

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

void AudioManager::detenerSonidosAmbientalesPorPatron(const std::string& patron)
{
    std::lock_guard<std::mutex> lock(mutexAmbientales);

    int contadorDetenidos = 0;
    for (auto* sonido : sonidosAmbientales) {
        if (sonido->activo && sonido->nombre.find(patron) != std::string::npos) {
            ma_sound_stop(&sonido->sound);
            sonido->activo = false;
            contadorDetenidos++;
        }
    }

    if (contadorDetenidos > 0) {
        std::cout << "[AudioManager] Detenidos " << contadorDetenidos
            << " sonidos ambientales con patrón: " << patron << std::endl;
    }
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

bool AudioManager::actualizarPosicionSonidoAmbiental(const std::string& nombre, const glm::vec3& nuevaPosicion)
{
    std::lock_guard<std::mutex> lock(mutexAmbientales);

    for (auto* sonido : sonidosAmbientales) {
        if (sonido->nombre == nombre && sonido->activo) {
            sonido->posicion = nuevaPosicion;
            ma_sound_set_position(&sonido->sound, nuevaPosicion.x, nuevaPosicion.y, nuevaPosicion.z);
            return true;
        }
    }

    return false;
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
                delete* it;
                it = sonidosAmbientales.erase(it);
            }
            else {
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