# Instrucciones para Completar la Integración del AudioManager

## ? Archivos Ya Modificados

1. **SceneInformation.h** - ? Ya incluye:
   - `#include "AudioManager.h"`
   - `AudioManager audioManager;` (miembro privado)
   - Declaración de `void inicializarAudio();`

2. **SceneInformation.cpp** - ? Ya incluye:
   - Llamada a `inicializarAudio()` en el constructor
   - Llamada a `audioManager.limpiar()` en el destructor

## ?? Paso Pendiente: Agregar la Función inicializarAudio()

**IMPORTANTE:** Debes agregar manualmente la implementación de `inicializarAudio()` en `SceneInformation.cpp`

### Ubicación Exacta:
Agregar **después** de la función `inicializarLuces()` y **antes** de `inicializarEntidades()`.

### Código a Agregar:

```cpp
// Funcion para inicializar el audio de la escena
void SceneInformation::inicializarAudio()
{
    // Inicializar el motor de audio
    if (!audioManager.inicializar()) {
        std::cerr << "[SceneInformation] Error al inicializar AudioManager" << std::endl;
        return;
    }
    
    std::cout << "[SceneInformation] AudioManager inicializado correctamente" << std::endl;
    
    // Cargar el soundtrack principal
    // NOTA: Asegúrate de tener un archivo de audio en la ruta especificada
    // Puedes usar MP3, WAV, OGG, FLAC, etc.
    audioManager.cargarSoundtrack("musica_ambiente", "Audio/ambient_soundtrack.mp3");
    
    // Reproducir el soundtrack en loop con volumen tenue (30% del máximo)
    // Esto asegura que no se sobreponga a futuros efectos de sonido
    if (!audioManager.reproducirSoundtrack("musica_ambiente", 0.3f)) {
        std::cerr << "[SceneInformation] Advertencia: No se pudo reproducir el soundtrack" << std::endl;
        std::cerr << "[SceneInformation] Verifica que el archivo existe en: Audio/ambient_soundtrack.mp3" << std::endl;
    } else {
        std::cout << "[SceneInformation] Soundtrack reproduciéndose en loop (volumen: 30%)" << std::endl;
    }
    
    // Establecer volumen maestro también bajo para no interferir con efectos futuros
    audioManager.setVolumenMaestro(0.8f); // 80% del volumen maestro
}
```

## ?? Crear Carpeta de Audio

Crea la siguiente estructura de carpetas en tu proyecto:

```
ProyectoFinalCGIHC/
??? Audio/
    ??? Soundtracks/         (opcional - para organizar)
    ?   ??? ambient_soundtrack.mp3
    ??? Ambient/              (opcional - para sonidos 3D)
    ??? SFX/                  (opcional - para efectos)
```

## ?? Archivo de Audio Requerido

**Nombre del archivo:** `ambient_soundtrack.mp3`  
**Ubicación:** `ProyectoFinalCGIHC/Audio/ambient_soundtrack.mp3`  
**Formato:** MP3 (también soporta WAV, OGG, FLAC)

### Alternativas si no tienes el archivo:

#### Opción 1: Usar otro archivo de audio
Modifica la ruta en `inicializarAudio()`:
```cpp
audioManager.cargarSoundtrack("musica_ambiente", "Audio/tu_musica.mp3");
```

#### Opción 2: Deshabilitar el audio temporalmente
Comenta las líneas de reproducción:
```cpp
// audioManager.cargarSoundtrack("musica_ambiente", "Audio/ambient_soundtrack.mp3");
// audioManager.reproducirSoundtrack("musica_ambiente", 0.3f);
```

## ?? Características del Soundtrack Implementado

### Configuración Actual:
- ? **Volumen:** 30% (0.3f) - Tenue para no sobreponerse
- ? **Volumen Maestro:** 80% (0.8f) - Permite ajuste global
- ? **Loop:** Infinito - Se reproduce continuamente
- ? **Thread-Safe:** Sí - Funciona correctamente con múltiples threads
- ? **Funciona en todas las cámaras:** Sí - No espacializado

### Ventajas:
1. **No se sobrepone a efectos futuros:** Volumen bajo (30%)
2. **Se reproduce en todas las cámaras:** Audio 2D sin espacialización
3. **Loop infinito automático:** No necesita reiniciarse manualmente
4. **Se limpia automáticamente:** El destructor lo gestiona

## ?? Controles y Comportamiento

### Durante la Ejecución:
- **Inicio:** La música comienza automáticamente al iniciar el programa
- **Loop:** Se repite indefinidamente sin interrupciones
- **Cambio de cámara (1/2/3):** La música continúa sin interrupciones
- **Cambio día/noche (G):** La música continúa sin interrupciones
- **Fin:** La música se detiene automáticamente al cerrar el programa

### Para Modificar el Volumen en Tiempo Real:
Si necesitas ajustar el volumen, puedes agregar controles de teclado en `actualizarFrameInput()`:

```cpp
// M: Aumentar volumen
if (keys[GLFW_KEY_M]) {
    float volActual = audioManager.getVolumenMaestro();
    audioManager.setVolumenMaestro(volActual + 0.1f);
}

// N: Disminuir volumen
if (keys[GLFW_KEY_N]) {
    float volActual = audioManager.getVolumenMaestro();
    audioManager.setVolumenMaestro(volActual - 0.1f);
}
```

## ? Verificación de la Integración

### 1. Compilar el Proyecto
```
Build > Build Solution (Ctrl+Shift+B)
```

### 2. Verificar la Consola al Ejecutar
Deberías ver:
```
[SceneInformation] AudioManager inicializado correctamente
[AudioManager] Soundtrack 'musica_ambiente' cargado: Audio/ambient_soundtrack.mp3
[AudioManager] Reproduciendo soundtrack: musica_ambiente
[SceneInformation] Soundtrack reproduciéndose en loop (volumen: 30%)
```

### 3. Si Ves Errores:
```
[SceneInformation] Advertencia: No se pudo reproducir el soundtrack
[SceneInformation] Verifica que el archivo existe en: Audio/ambient_soundtrack.mp3
```
**Solución:** Coloca un archivo de audio en la ruta correcta o usa una alternativa.

## ?? Resultado Esperado

- ? **Música de fondo:** Se reproduce automáticamente al iniciar
- ? **Volumen tenue:** 30% del máximo (no interfiere con efectos)
- ? **Loop infinito:** Se repite continuamente
- ? **Todas las cámaras:** Funciona en tercera persona (Cuphead, Isaac, Gojo) y cámara libre
- ? **Sin interrupciones:** Continúa durante cambios de día/noche, animaciones, etc.
- ? **Limpieza automática:** Se detiene al cerrar el programa

## ?? Troubleshooting

### Problema: "Error al inicializar AudioManager"
- Verifica que miniaudio esté correctamente compilado
- Revisa que `MiniAudio.cpp` contenga:
  ```cpp
  #define MINIAUDIO_IMPLEMENTATION
  #include "include/miniaudio.h"
  ```

### Problema: "No se pudo reproducir el soundtrack"
- Verifica que el archivo de audio existe en `Audio/ambient_soundtrack.mp3`
- Verifica que el formato es soportado (MP3, WAV, OGG, FLAC)
- Revisa que la ruta es relativa al ejecutable

### Problema: No se escucha nada
- Verifica el volumen del sistema operativo
- Verifica que no hay errores en la consola
- Intenta aumentar el volumen en el código a `0.7f`

## ?? Documentación Completa

Para más información sobre el AudioManager, consulta:
- `AUDIOMANAGER_README.md` - Documentación completa
- `AudioManager_Ejemplo.cpp` - Ejemplos de uso

## ?? ¡Listo!

Una vez que agregues la función `inicializarAudio()` y coloques un archivo de audio, el soundtrack debería reproducirse automáticamente en loop durante toda la ejecución del programa.
