// =============================================================================
// EJEMPLO DE USO DEL AUDIOMANAGER
// =============================================================================
// Este archivo muestra cómo utilizar la clase AudioManager en tu proyecto
// =============================================================================

#include "AudioManager.h"
#include <iostream>

void ejemploUsoBasico()
{
    // ==================== INICIALIZACIÓN ====================
    AudioManager audioManager;
    
    if (!audioManager.inicializar()) {
        std::cerr << "Error al inicializar AudioManager" << std::endl;
        return;
    }
    
    // ==================== SOUNDTRACK (Música de fondo) ====================
    // Cargar y reproducir música de fondo en loop infinito
    audioManager.cargarSoundtrack("musica_menu", "Audio/Soundtracks/menu_theme.mp3");
    audioManager.cargarSoundtrack("musica_juego", "Audio/Soundtracks/game_theme.mp3");
    
    // Reproducir soundtrack con volumen 0.7 (70%)
    audioManager.reproducirSoundtrack("musica_menu", 0.7f);
    
    // Control del soundtrack
    audioManager.pausarSoundtrack();            // Pausar
    audioManager.reanudarSoundtrack();          // Reanudar
    audioManager.setVolumenSoundtrack(0.5f);    // Ajustar volumen
    audioManager.detenerSoundtrack();           // Detener
    
    // Cambiar a otro soundtrack
    audioManager.reproducirSoundtrack("musica_juego", 0.8f);
    
    
    // ==================== SONIDOS AMBIENTALES (3D) ====================
    // Cargar sonidos ambientales (loops cortos con posición 3D)
    audioManager.cargarSonidoAmbiental("fuego", "Audio/Ambient/campfire.wav");
    audioManager.cargarSonidoAmbiental("agua", "Audio/Ambient/waterfall.wav");
    audioManager.cargarSonidoAmbiental("viento", "Audio/Ambient/wind.wav");
    
    // Reproducir sonido ambiental en una posición 3D con loop
    glm::vec3 posicionFogata(10.0f, 0.0f, 5.0f);
    audioManager.reproducirSonidoAmbiental("fuego", posicionFogata, 1.0f, true);
    
    // Reproducir múltiples instancias del mismo sonido en diferentes posiciones
    glm::vec3 posicionCascada1(-20.0f, 0.0f, 30.0f);
    glm::vec3 posicionCascada2(15.0f, 0.0f, -10.0f);
    audioManager.reproducirSonidoAmbiental("agua", posicionCascada1, 0.8f, true);
    audioManager.reproducirSonidoAmbiental("agua", posicionCascada2, 0.6f, true);
    
    // Actualizar posición del listener (cámara/jugador) cada frame
    // Esto debe llamarse en el game loop para audio 3D correcto
    glm::vec3 posicionCamara(0.0f, 1.5f, 0.0f);
    glm::vec3 direccionCamara(0.0f, 0.0f, -1.0f);
    audioManager.actualizarPosicionListener(posicionCamara, direccionCamara);
    
    // Detener un sonido ambiental específico
    audioManager.detenerSonidoAmbiental("viento");
    
    // Detener todos los sonidos ambientales
    audioManager.detenerTodosSonidosAmbientales();
    
    
    // ==================== SONIDOS NORMALES (One-shot) ====================
    // Cargar sonidos one-shot (disparos, saltos, explosiones, etc.)
    audioManager.cargarSonidoNormal("disparo", "Audio/SFX/gunshot.wav");
    audioManager.cargarSonidoNormal("salto", "Audio/SFX/jump.wav");
    audioManager.cargarSonidoNormal("explosion", "Audio/SFX/explosion.wav");
    audioManager.cargarSonidoNormal("paso", "Audio/SFX/footstep.wav");
    
    // Reproducir sonido instantáneamente
    audioManager.reproducirSonidoNormal("disparo", 1.0f);
    audioManager.reproducirSonidoNormal("salto", 0.8f);
    
    // Reproducir sonido con delay (útil para efectos secuenciales)
    audioManager.reproducirSonidoNormalConDelay("explosion", 2.0f, 1.0f); // Explota en 2 segundos
    
    
    // ==================== UTILIDADES ====================
    // Volumen maestro (afecta a todos los sonidos)
    audioManager.setVolumenMaestro(0.75f); // 75% del volumen
    
    // Limpiar sonidos inactivos (liberar memoria)
    audioManager.limpiarSonidosInactivos();
    
    
    // ==================== LIMPIEZA ====================
    // Al finalizar, liberar recursos
    audioManager.limpiar();
}

// =============================================================================
// EJEMPLO INTEGRADO EN GAME LOOP
// =============================================================================

void ejemploGameLoop()
{
    AudioManager audioManager;
    audioManager.inicializar();
    
    // Setup inicial
    audioManager.cargarSoundtrack("musica_nivel1", "Audio/level1_music.mp3");
    audioManager.reproducirSoundtrack("musica_nivel1", 0.7f);
    
    audioManager.cargarSonidoAmbiental("ambiente_bosque", "Audio/forest_ambient.wav");
    audioManager.cargarSonidoNormal("salto", "Audio/jump.wav");
    
    // Variables del juego (ejemplo)
    glm::vec3 posicionJugador(0.0f, 0.0f, 0.0f);
    glm::vec3 direccionCamara(0.0f, 0.0f, -1.0f);
    bool teclaEspacioPresionada = false;
    
    // Game Loop
    while (true) { // Reemplazar con condición real del game loop
        // ========== ACTUALIZAR AUDIO 3D ==========
        // Actualizar posición del listener cada frame
        audioManager.actualizarPosicionListener(posicionJugador, direccionCamara);
        
        // ========== INPUT DEL JUGADOR ==========
        // Ejemplo: Detectar tecla de salto
        if (teclaEspacioPresionada) {
            audioManager.reproducirSonidoNormal("salto", 1.0f);
        }
        
        // ========== LIMPIAR SONIDOS INACTIVOS ==========
        // Llamar cada cierto tiempo para liberar memoria
        static int frameCount = 0;
        if (frameCount++ % 300 == 0) { // Cada 300 frames
            audioManager.limpiarSonidosInactivos();
        }
        
        // ... resto de la lógica del juego ...
    }
    
    audioManager.limpiar();
}

// =============================================================================
// EJEMPLO: CONTROL POR DISTANCIA
// =============================================================================

void ejemploControlPorDistancia()
{
    AudioManager audioManager;
    audioManager.inicializar();
    
    // Cargar sonidos
    audioManager.cargarSonidoAmbiental("monstruo_respiracion", "Audio/monster_breathing.wav");
    audioManager.cargarSonidoNormal("grito_terror", "Audio/scream.wav");
    
    // Posiciones
    glm::vec3 posicionJugador(0.0f, 0.0f, 0.0f);
    glm::vec3 posicionMonstruo(50.0f, 0.0f, 0.0f);
    
    // Reproducir sonido ambiental del monstruo
    audioManager.reproducirSonidoAmbiental("monstruo_respiracion", posicionMonstruo, 1.0f, true);
    
    // Game Loop
    while (true) {
        // Actualizar posición del listener
        audioManager.actualizarPosicionListener(posicionJugador, glm::vec3(1.0f, 0.0f, 0.0f));
        
        // Calcular distancia
        float distancia = glm::distance(posicionJugador, posicionMonstruo);
        
        // Si el jugador está cerca del monstruo
        if (distancia < 5.0f) {
            // Reproducir grito de terror (una sola vez)
            static bool gritoReproducido = false;
            if (!gritoReproducido) {
                audioManager.reproducirSonidoNormal("grito_terror", 1.0f);
                gritoReproducido = true;
            }
        }
        
        // El audio 3D automáticamente ajusta volumen según distancia
        // (configurado con ma_sound_set_min_distance y ma_sound_set_max_distance)
        
        // ... resto de la lógica ...
    }
}

// =============================================================================
// EJEMPLO: MÚLTIPLES THREADS
// =============================================================================

void ejemploThreads()
{
    AudioManager audioManager;
    audioManager.inicializar();
    
    // Cargar sonidos
    audioManager.cargarSonidoNormal("efecto1", "Audio/effect1.wav");
    audioManager.cargarSonidoNormal("efecto2", "Audio/effect2.wav");
    audioManager.cargarSonidoNormal("efecto3", "Audio/effect3.wav");
    
    // Reproducir sonidos con delays (usando threads internamente)
    audioManager.reproducirSonidoNormalConDelay("efecto1", 0.0f, 1.0f);  // Inmediato
    audioManager.reproducirSonidoNormalConDelay("efecto2", 0.5f, 1.0f);  // Después de 0.5s
    audioManager.reproducirSonidoNormalConDelay("efecto3", 1.0f, 1.0f);  // Después de 1s
    
    // Los sonidos se reproducirán automáticamente en sus threads respectivos
    // No es necesario gestionar los threads manualmente
    
    // IMPORTANTE: Los threads son detached, así que no bloquean la ejecución
    
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Esperar para escuchar
    
    audioManager.limpiar();
}

// =============================================================================
// MAIN DE EJEMPLO (NO INCLUIR EN PROYECTO REAL)
// =============================================================================

int main()
{
    std::cout << "=== AudioManager - Ejemplos de Uso ===" << std::endl;
    
    // Descomentar el ejemplo que quieras probar:
    // ejemploUsoBasico();
    // ejemploGameLoop();
    // ejemploControlPorDistancia();
    // ejemploThreads();
    
    std::cout << "Presiona Enter para salir..." << std::endl;
    std::cin.get();
    
    return 0;
}
