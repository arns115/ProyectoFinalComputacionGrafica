# AudioManager - Documentación Completa

## ?? Tabla de Contenidos
1. [Descripción General](#descripción-general)
2. [Características](#características)
3. [Instalación](#instalación)
4. [API Completa](#api-completa)
5. [Ejemplos de Uso](#ejemplos-de-uso)
6. [Mejores Prácticas](#mejores-prácticas)
7. [Troubleshooting](#troubleshooting)

---

## ?? Descripción General

**AudioManager** es una clase completa de gestión de audio para aplicaciones C++ que utiliza la biblioteca **miniaudio.h**. Proporciona tres tipos principales de audio:

1. **Soundtrack (Música de fondo)**: Música en loop infinito mediante buffer
2. **Sonidos Ambientales (3D)**: Sonidos posicionales con espacialización 3D
3. **Sonidos Normales (One-shot)**: Efectos de sonido instantáneos

### ? Características Principales

- ? **Thread-safe**: Uso de mutexes para operaciones concurrentes
- ? **Soporte 3D**: Audio espacial con atenuación por distancia
- ? **Gestión automática**: Limpieza de sonidos inactivos
- ? **Control completo**: Volumen, loop, pausa, delay
- ? **Múltiples instancias**: Reproducir el mismo sonido varias veces simultáneamente
- ? **Eficiente**: Reutilización de slots para sonidos one-shot

---

## ?? Instalación

### Requisitos
- C++14 o superior
- miniaudio.h (incluido en `include/`)
- GLM (para vectores 3D)

### Archivos
```
ProyectoFinalCGIHC/
??? AudioManager.h              # Declaración de la clase
??? AudioManager.cpp            # Implementación
??? AudioManager_Ejemplo.cpp    # Ejemplos de uso
??? MiniAudio.cpp               # Implementación de miniaudio
??? include/
    ??? miniaudio.h             # Biblioteca miniaudio
```

### Configuración
1. Asegúrate de que `MiniAudio.cpp` contenga:
```cpp
#define MINIAUDIO_IMPLEMENTATION
#include "include/miniaudio.h"
```

2. Incluye AudioManager en tu proyecto:
```cpp
#include "AudioManager.h"
```

---

## ?? API Completa

### Inicialización y Limpieza

#### `AudioManager()`
Constructor por defecto. No inicializa el motor de audio automáticamente.

#### `~AudioManager()`
Destructor. Llama a `limpiar()` automáticamente.

#### `bool inicializar()`
Inicializa el motor de audio de miniaudio.

**Retorna:** `true` si se inicializó correctamente, `false` en caso de error.

```cpp
AudioManager audioManager;
if (!audioManager.inicializar()) {
    std::cerr << "Error al inicializar audio" << std::endl;
}
```

#### `void limpiar()`
Libera todos los recursos de audio y desinicializa el motor.

**Nota:** Se llama automáticamente en el destructor.

```cpp
audioManager.limpiar();
```

#### `bool estaInicializado() const`
Verifica si el motor de audio está inicializado.

**Retorna:** `true` si está inicializado.

---

### ?? Soundtrack (Música de Fondo)

#### `bool cargarSoundtrack(const std::string& nombre, const std::string& rutaArchivo)`
Registra una pista de música para uso posterior.

**Parámetros:**
- `nombre`: Identificador único para el soundtrack
- `rutaArchivo`: Ruta al archivo de audio (MP3, WAV, OGG, FLAC)

**Retorna:** `true` si se registró correctamente.

```cpp
audioManager.cargarSoundtrack("musica_menu", "Audio/menu_theme.mp3");
audioManager.cargarSoundtrack("musica_nivel1", "Audio/level1_music.ogg");
```

#### `bool reproducirSoundtrack(const std::string& nombre, float volumen = 1.0f)`
Reproduce un soundtrack en loop infinito mediante buffer.

**Parámetros:**
- `nombre`: Nombre del soundtrack previamente cargado
- `volumen`: Volumen de 0.0 (silencio) a 1.0 (máximo) [opcional]

**Retorna:** `true` si se reprodujo correctamente.

**Nota:** Detiene automáticamente el soundtrack anterior si había uno reproduciéndose.

```cpp
audioManager.reproducirSoundtrack("musica_menu", 0.7f); // 70% volumen
```

#### `void detenerSoundtrack()`
Detiene el soundtrack actual.

```cpp
audioManager.detenerSoundtrack();
```

#### `void pausarSoundtrack()`
Pausa el soundtrack actual.

```cpp
audioManager.pausarSoundtrack();
```

#### `void reanudarSoundtrack()`
Reanuda el soundtrack pausado.

```cpp
audioManager.reanudarSoundtrack();
```

#### `void setVolumenSoundtrack(float volumen)`
Ajusta el volumen del soundtrack actual.

**Parámetros:**
- `volumen`: Volumen de 0.0 a 1.0

```cpp
audioManager.setVolumenSoundtrack(0.5f); // 50% volumen
```

---

### ?? Sonidos Ambientales (Audio 3D)

#### `bool cargarSonidoAmbiental(const std::string& nombre, const std::string& rutaArchivo)`
Registra un sonido ambiental para uso posterior.

**Parámetros:**
- `nombre`: Identificador único
- `rutaArchivo`: Ruta al archivo de audio (WAV, MP3, OGG)

**Retorna:** `true` si se registró correctamente.

**Recomendación:** Usa archivos cortos (< 10 segundos) para loops ambientales.

```cpp
audioManager.cargarSonidoAmbiental("fuego", "Audio/Ambient/campfire.wav");
audioManager.cargarSonidoAmbiental("agua", "Audio/Ambient/waterfall.wav");
```

#### `bool reproducirSonidoAmbiental(const std::string& nombre, const glm::vec3& posicion, float volumen = 1.0f, bool loop = true)`
Reproduce un sonido ambiental en una posición 3D.

**Parámetros:**
- `nombre`: Nombre del sonido previamente cargado
- `posicion`: Posición 3D del sonido (glm::vec3)
- `volumen`: Volumen de 0.0 a 1.0 [opcional]
- `loop`: Si debe reproducirse en loop [opcional, default = true]

**Retorna:** `true` si se reprodujo correctamente.

**Características:**
- Espacialización 3D automática
- Atenuación por distancia (1.0 - 50.0 unidades)
- Múltiples instancias permitidas

```cpp
glm::vec3 posicionFogata(10.0f, 0.0f, 5.0f);
audioManager.reproducirSonidoAmbiental("fuego", posicionFogata, 1.0f, true);

// Múltiples instancias
glm::vec3 pos1(-20.0f, 0.0f, 30.0f);
glm::vec3 pos2(15.0f, 0.0f, -10.0f);
audioManager.reproducirSonidoAmbiental("agua", pos1, 0.8f, true);
audioManager.reproducirSonidoAmbiental("agua", pos2, 0.6f, true);
```

#### `void detenerSonidoAmbiental(const std::string& nombre)`
Detiene todas las instancias de un sonido ambiental específico.

```cpp
audioManager.detenerSonidoAmbiental("fuego");
```

#### `void detenerTodosSonidosAmbientales()`
Detiene todos los sonidos ambientales activos.

```cpp
audioManager.detenerTodosSonidosAmbientales();
```

#### `void actualizarPosicionListener(const glm::vec3& posicion, const glm::vec3& direccion)`
Actualiza la posición y dirección del oyente (cámara/jugador).

**Parámetros:**
- `posicion`: Posición 3D del oyente
- `direccion`: Dirección hacia donde mira el oyente (vector normalizado)

**Importante:** Debe llamarse cada frame en el game loop para audio 3D correcto.

```cpp
// En el game loop
glm::vec3 posicionCamara = camera.getPosition();
glm::vec3 direccionCamara = camera.getDirection();
audioManager.actualizarPosicionListener(posicionCamara, direccionCamara);
```

---

### ?? Sonidos Normales (One-shot)

#### `bool cargarSonidoNormal(const std::string& nombre, const std::string& rutaArchivo)`
Registra un sonido one-shot para uso posterior.

**Parámetros:**
- `nombre`: Identificador único
- `rutaArchivo`: Ruta al archivo de audio

**Retorna:** `true` si se registró correctamente.

**Uso típico:** Disparos, saltos, explosiones, pasos, colisiones, etc.

```cpp
audioManager.cargarSonidoNormal("disparo", "Audio/SFX/gunshot.wav");
audioManager.cargarSonidoNormal("salto", "Audio/SFX/jump.wav");
audioManager.cargarSonidoNormal("paso", "Audio/SFX/footstep.wav");
```

#### `bool reproducirSonidoNormal(const std::string& nombre, float volumen = 1.0f)`
Reproduce un sonido instantáneamente (sin espacialización).

**Parámetros:**
- `nombre`: Nombre del sonido previamente cargado
- `volumen`: Volumen de 0.0 a 1.0 [opcional]

**Retorna:** `true` si se reprodujo correctamente.

**Características:**
- No se reproduce en loop (one-shot)
- No tiene posición 3D (mono/stereo normal)
- Reutiliza slots automáticamente

```cpp
// Disparo al presionar tecla
if (teclaDisparoPresionada) {
    audioManager.reproducirSonidoNormal("disparo", 1.0f);
}

// Múltiples disparos rápidos (permitido)
audioManager.reproducirSonidoNormal("disparo", 1.0f);
audioManager.reproducirSonidoNormal("disparo", 1.0f);
audioManager.reproducirSonidoNormal("disparo", 1.0f);
```

#### `bool reproducirSonidoNormalConDelay(const std::string& nombre, float delay, float volumen = 1.0f)`
Reproduce un sonido después de un delay (usando threads).

**Parámetros:**
- `nombre`: Nombre del sonido previamente cargado
- `delay`: Tiempo de espera en segundos
- `volumen`: Volumen de 0.0 a 1.0 [opcional]

**Retorna:** `true` si se programó correctamente.

**Características:**
- Usa threads detached (no bloquea)
- Thread-safe

```cpp
// Explosión después de 2 segundos
audioManager.reproducirSonidoNormalConDelay("explosion", 2.0f, 1.0f);

// Secuencia de pasos
audioManager.reproducirSonidoNormalConDelay("paso", 0.0f, 1.0f);
audioManager.reproducirSonidoNormalConDelay("paso", 0.5f, 1.0f);
audioManager.reproducirSonidoNormalConDelay("paso", 1.0f, 1.0f);
```

---

### ?? Utilidades

#### `float getVolumenMaestro() const`
Obtiene el volumen maestro actual.

**Retorna:** Volumen de 0.0 a 1.0

```cpp
float vol = audioManager.getVolumenMaestro();
std::cout << "Volumen maestro: " << vol << std::endl;
```

#### `void setVolumenMaestro(float volumen)`
Establece el volumen maestro (afecta a todos los sonidos).

**Parámetros:**
- `volumen`: Volumen de 0.0 a 1.0

**Nota:** Se aplica automáticamente a todos los sonidos activos.

```cpp
audioManager.setVolumenMaestro(0.75f); // 75% del volumen
```

#### `void limpiarSonidosInactivos()`
Libera memoria de sonidos que han terminado de reproducirse.

**Recomendación:** Llamar periódicamente (cada 5-10 segundos o cada 300-600 frames).

```cpp
// En el game loop
static int frameCount = 0;
if (frameCount++ % 300 == 0) {
    audioManager.limpiarSonidosInactivos();
}
```

---

## ?? Ejemplos de Uso

### Ejemplo 1: Música de Menú

```cpp
AudioManager audioManager;
audioManager.inicializar();

// Cargar y reproducir música de menú
audioManager.cargarSoundtrack("musica_menu", "Audio/menu_theme.mp3");
audioManager.reproducirSoundtrack("musica_menu", 0.7f);

// Al salir del menú
audioManager.detenerSoundtrack();
```

### Ejemplo 2: Sonido de Salto

```cpp
// Setup
audioManager.cargarSonidoNormal("salto", "Audio/jump.wav");

// En el game loop, al detectar tecla espacio
if (keys[GLFW_KEY_SPACE] && jugador.enSuelo()) {
    jugador.saltar();
    audioManager.reproducirSonidoNormal("salto", 0.8f);
}
```

### Ejemplo 3: Fogata con Audio 3D

```cpp
// Setup
audioManager.cargarSonidoAmbiental("fuego", "Audio/campfire_loop.wav");

// Crear fogata en posición específica
glm::vec3 posicionFogata(50.0f, 0.0f, 100.0f);
audioManager.reproducirSonidoAmbiental("fuego", posicionFogata, 1.0f, true);

// En el game loop - actualizar listener
glm::vec3 posJugador = jugador.getPosicion();
glm::vec3 dirCamara = camara.getDireccion();
audioManager.actualizarPosicionListener(posJugador, dirCamara);

// El audio se escucha más fuerte cuando el jugador se acerca a la fogata
```

### Ejemplo 4: Sistema de Pasos

```cpp
// Setup
audioManager.cargarSonidoNormal("paso", "Audio/footstep.wav");

// En el game loop
if (jugador.estaCaminando()) {
    static float tiempoDesdeUltimoPaso = 0.0f;
    tiempoDesdeUltimoPaso += deltaTime;
    
    if (tiempoDesdeUltimoPaso >= 0.5f) { // Un paso cada 0.5 segundos
        audioManager.reproducirSonidoNormal("paso", 0.7f);
        tiempoDesdeUltimoPaso = 0.0f;
    }
}
```

### Ejemplo 5: Control por Distancia

```cpp
// Setup
audioManager.cargarSonidoAmbiental("monstruo", "Audio/monster_breathing.wav");
audioManager.cargarSonidoNormal("grito", "Audio/scream.wav");

glm::vec3 posMonstruo(100.0f, 0.0f, 50.0f);
audioManager.reproducirSonidoAmbiental("monstruo", posMonstruo, 1.0f, true);

// En el game loop
glm::vec3 posJugador = jugador.getPosicion();
float distancia = glm::distance(posJugador, posMonstruo);

if (distancia < 10.0f) {
    static bool gritoReproducido = false;
    if (!gritoReproducido) {
        audioManager.reproducirSonidoNormal("grito", 1.0f);
        gritoReproducido = true;
    }
}
```

---

## ?? Mejores Prácticas

### 1. Inicialización
```cpp
// ? CORRECTO
AudioManager audioManager;
if (!audioManager.inicializar()) {
    std::cerr << "Error de audio" << std::endl;
    return -1;
}

// ? INCORRECTO - No verificar inicialización
AudioManager audioManager;
audioManager.inicializar(); // Podría fallar silenciosamente
```

### 2. Cargar al Inicio
```cpp
// ? CORRECTO - Cargar todos los sonidos al inicio
void cargarSonidos(AudioManager& audio) {
    audio.cargarSoundtrack("musica1", "Audio/music1.mp3");
    audio.cargarSonidoNormal("disparo", "Audio/gun.wav");
    audio.cargarSonidoAmbiental("fuego", "Audio/fire.wav");
}

// ? INCORRECTO - Cargar en medio del gameplay
void disparar(AudioManager& audio) {
    audio.cargarSonidoNormal("disparo", "Audio/gun.wav"); // Lag!
    audio.reproducirSonidoNormal("disparo", 1.0f);
}
```

### 3. Actualizar Listener
```cpp
// ? CORRECTO - Actualizar cada frame
void gameLoop(AudioManager& audio) {
    while (running) {
        audio.actualizarPosicionListener(camera.pos, camera.dir);
        // ... resto del loop
    }
}

// ? INCORRECTO - No actualizar o hacerlo raramente
// El audio 3D no funcionará correctamente
```

### 4. Limpiar Sonidos
```cpp
// ? CORRECTO - Limpiar periódicamente
static int frame = 0;
if (frame++ % 300 == 0) {
    audioManager.limpiarSonidosInactivos();
}

// ? INCORRECTO - Nunca limpiar
// Fuga de memoria gradual
```

### 5. Volumen Apropiado
```cpp
// ? CORRECTO - Volúmenes balanceados
audioManager.reproducirSoundtrack("musica", 0.6f);      // Música de fondo suave
audioManager.reproducirSonidoNormal("explosion", 1.0f); // Efectos fuertes

// ? INCORRECTO - Todo al máximo
audioManager.reproducirSoundtrack("musica", 1.0f);      // Muy fuerte
audioManager.reproducirSonidoNormal("paso", 1.0f);      // Pasos ensordecedores
```

### 6. Gestión de Soundtrack
```cpp
// ? CORRECTO - Un soundtrack a la vez
audioManager.reproducirSoundtrack("nivel1", 0.7f);
// ... cambio de nivel ...
audioManager.reproducirSoundtrack("nivel2", 0.7f); // Detiene nivel1 automáticamente

// ? INCORRECTO - No gestionar cambios
// Pueden solaparse múltiples músicas
```

---

## ?? Troubleshooting

### Problema: No se escucha ningún sonido

**Soluciones:**
1. Verificar que `inicializar()` retornó `true`
2. Comprobar rutas de archivos (son case-sensitive en algunos sistemas)
3. Verificar que los archivos de audio existen
4. Asegurar que el volumen maestro no está en 0

```cpp
if (!audioManager.inicializar()) {
    std::cerr << "Error: AudioManager no inicializado" << std::endl;
}

// Verificar volumen
std::cout << "Volumen maestro: " << audioManager.getVolumenMaestro() << std::endl;
```

### Problema: Audio 3D no funciona

**Soluciones:**
1. Llamar `actualizarPosicionListener()` cada frame
2. Verificar que las posiciones son correctas (no NaN o infinito)
3. Comprobar que el sonido está dentro del rango de atenuación (1-50 unidades)

```cpp
// Debug
glm::vec3 posListener = camera.getPosition();
glm::vec3 posSonido(10.0f, 0.0f, 5.0f);
float distancia = glm::distance(posListener, posSonido);
std::cout << "Distancia al sonido: " << distancia << std::endl;
```

### Problema: Lag al reproducir sonidos

**Soluciones:**
1. Cargar todos los sonidos al inicio, no durante gameplay
2. Para soundtracks, usar archivos comprimidos (MP3, OGG)
3. Para efectos, usar archivos cortos (< 5 segundos)

```cpp
// ? Cargar al inicio
void init() {
    audioManager.cargarSonidoNormal("disparo", "Audio/gun.wav");
}

void disparar() {
    audioManager.reproducirSonidoNormal("disparo", 1.0f); // Sin lag
}
```

### Problema: Fuga de memoria

**Soluciones:**
1. Llamar `limpiarSonidosInactivos()` periódicamente
2. Asegurar que `limpiar()` se llama al finalizar
3. No crear múltiples instancias de AudioManager

```cpp
// ? Limpiar periódicamente
if (frameCount % 300 == 0) {
    audioManager.limpiarSonidosInactivos();
}

// ? Limpiar al finalizar
audioManager.limpiar();
```

### Problema: Threads no funcionan

**Soluciones:**
1. Verificar que el compilador soporta C++11 threads
2. En Visual Studio, asegurar que `/std:c++14` o superior está configurado
3. Para Linux, enlazar con `-pthread`

---

## ?? Notas Adicionales

### Formatos Soportados
- **MP3** (recomendado para soundtrack)
- **WAV** (recomendado para efectos)
- **OGG** (alternativa comprimida)
- **FLAC** (alta calidad)

### Limitaciones
- **Soundtrack**: Solo uno puede reproducirse a la vez
- **Sonidos Ambientales**: Ilimitados (limitado por RAM)
- **Sonidos Normales**: Ilimitados (reutiliza slots automáticamente)

### Thread Safety
- Todas las operaciones de reproducción son thread-safe
- `actualizarPosicionListener()` es thread-safe
- `limpiarSonidosInactivos()` es thread-safe

### Rendimiento
- **Soundtrack**: Usa streaming (bajo uso de RAM)
- **Ambientales**: Cargan todo en memoria (archivos cortos recomendados)
- **Normales**: Cargan todo en memoria (archivos cortos)

---

## ?? Soporte

Para bugs, preguntas o sugerencias, contactar al equipo de desarrollo.

**Versión:** 1.0.0  
**Fecha:** 2024  
**Biblioteca:** miniaudio (https://miniaud.io)

---

## ?? Licencia

Este código está diseñado para uso educativo y en proyectos de la clase de Computación Gráfica e Interacción Humano-Computadora.
