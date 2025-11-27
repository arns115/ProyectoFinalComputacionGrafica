#include "SceneInformation.h"
#include "ComponenteFisico.h"
#include "ComponenteAnimacion.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

SceneInformation::SceneInformation()
    : skyboxActual(nullptr), pointLightCountActual(0), spotLightCountActual(0)
{
    // Llamar a las funciones de inicialización separadas
    inicializarAudio();   // Inicializar Audio (primero para que empiece la música)
    inicializarSkybox();  // Inicializar Skybox
    inicializarLuces();   // Inicializar luces 
    inicializarCamara();  // Inicializar cámara con valores por defecto
    inicializarEntidades();  // Inicializar Enitdades

}

SceneInformation::~SceneInformation()
{
    // Liberar todas las entidades
    for (auto* entidad : entidades) {
        delete entidad;
    }
    entidades.clear();

    // Limpiar referencia al skybox
    skyboxActual = nullptr;

    // Limpiar audio
    audioManager.limpiar();

}

// Function para inicializar la camara
void SceneInformation::inicializarCamara(glm::vec3 startPosition,
    glm::vec3 startUp,
    GLfloat startYaw,
    GLfloat startPitch,
    GLfloat startMoveSpeed,
    GLfloat startTurnSpeed)
{
    // Crear la cámara con los parámetros especificados
    camera = Camera(startPosition, startUp, startYaw, startPitch, startMoveSpeed, startTurnSpeed);
}

// Funcion para inicializar la skybox
void SceneInformation::inicializarSkybox()
{
    // Establecer el skybox por defecto
    setSkyboxActual(AssetConstants::SkyboxNames::NIGHT);
}

// Funcion para inicializar la luz direccional
void SceneInformation::inicializarLuces()
{
    // Obtener la luz del sol desde el LightManager
    DirectionalLight* nightLight = lightManager.getDirectionalLight(AssetConstants::LightNames::ESTRELLAS);

    if (nightLight != nullptr) {
        // Establecer la luz del sol como luz direccional principal
        luzDireccional = *nightLight;
    }
    else {
        // Si no existe, crear una luz direccional por defecto
        luzDireccional = DirectionalLight(
            1.0f, 1.0f, 1.0f,
            0.3f, 0.5f,
            0.0f, -1.0f, 0.0f
        );
    }


}

// Funcion para inicializar todas las entidades
void SceneInformation::inicializarEntidades()
{
    crearPiso();
    crearCamino();
    crearIslas();
    crearChinampaAgua();
    crearObjetosGeometricos();
    crearCabezaOlmeca();
    crearPiramide();
    crearHollow();
    crearBossRoom();
    crearSalaDiablo();
    crearDiablo();
    crearSecretRoom();
    crearFogatas();
    crearComidaPerro();
    crearPuertaSecreta();
    crearArbolesAlrededorChinampa();
    crearCanoa();
    crearCanchaPelotaMaya();
    crearPelotaDeJuegoDePelota();
    crearPrimo();
    crearLuchador();
    crearBlackHole();
    crearPyramideMuseo();
    creaCarpa();
    crearLamparasCalles();
    crearLamparasRing();
    crearPez();  // Crear el pez en el agua
    // Los personajes deben ser los ultimos en crearse para que la camara facilmente los pueda seguir (estaran en orden al final del vector de entidades)
    // Primero Cuphead
    // Segundo Isaac
    // Tercero Gojo
    crearPersonajePrincipal();
    crearIsaac();
    crearGojo();
    crearPoblacionMaya();


}

// Funcion para actualizar cada frame with las cosas que no dependen del input del usuario
void SceneInformation::actualizarFrame(float deltaTime)
{
    // Actualizar el ciclo dia/noche
    acumuladorTiempoDesdeCambio += deltaTime;
    if (acumuladorTiempoDesdeCambio >= 30.0f / LIMIT_FPS) // 30 segundos = 1 minuto
    {
        bool eraNoche = !esDeDia; // Guardar estado anterior
        esDeDia = !esDeDia; // Cambiar entre dia y noche
        acumuladorTiempoDesdeCambio = 0.0f; // Reiniciar el acumulador

        if (esDeDia)
        {
            // Se pone la skyboxAdecuada y la luz direccional del sol
            setSkyboxActual(AssetConstants::SkyboxNames::DAY);
            luzDireccional = *lightManager.getDirectionalLight(AssetConstants::LightNames::SOL);

            // Desactivar grillos cuando sale el sol
            if (eraNoche) {
                desactivarGrillos();
            }

            // NUEVO: Activar sonido del tianguis durante el día
            if (eraNoche) {
                glm::vec3 posicionMercado(-50.0f, -1.0f, 75.0f); // Centro del área del mercado
                audioManager.reproducirSonidoAmbiental("tianguis", posicionMercado, 0.9f, true);
                std::cout << "[SceneInformation] Sonido del tianguis activado (día)" << std::endl;
            }
        }
        else
        {
            // Se pone la skyboxAdecuada y la luz direccional de las estrellas
            setSkyboxActual(AssetConstants::SkyboxNames::NIGHT);
            luzDireccional = *lightManager.getDirectionalLight(AssetConstants::LightNames::ESTRELLAS);

            // Activar grillos cuando cae la noche
            if (!eraNoche) {
                activarGrillos();
            }

            // NUEVO: Desactivar sonido del tianguis durante la noche
            if (!eraNoche) {
                audioManager.detenerSonidoAmbiental("tianguis");
                std::cout << "[SceneInformation] Sonido del tianguis desactivado (noche)" << std::endl;
            }
        }
    }

    // Se posiciona linterna en la posicion y direccion de la camara
    spotLightActual = *lightManager.getSpotLight(AssetConstants::LightNames::LINTERNA);
    posicionLuzActual = camera.getCameraPosition();
    direccionLuzActual = camera.getCameraDirection();
    spotLightActual.SetFlash(posicionLuzActual, direccionLuzActual);
    agregarSpotLightActual(spotLightActual);

    // Actualizar animación de la canoa y su sonido
    for (auto* entidad : entidades) {
        if (entidad != nullptr && entidad->nombreObjeto == "canoa" && entidad->animacion != nullptr) {
            bool animacionActiva = entidad->animacion->estaActiva(0);

            // Animar la canoa
            entidad->animacion->animarCanoa(0, deltaTime);

            // Gestionar el SFX del remo
            if (animacionActiva) {
                // Obtener la posición actual de la canoa
                glm::vec3 posicionCanoa = entidad->posicionLocal;

                // Actualizar la posición del sonido si existe, o reproducirlo si no
                if (!audioManager.actualizarPosicionSonidoAmbiental("remo_canoa", posicionCanoa)) {
                    // Si no existe, reproducirlo
                    audioManager.reproducirSonidoAmbiental("remo_canoa", posicionCanoa, 0.5f, true);
                }
            }
            else {
                // Si la animación se desactiva, detener el sonido
                audioManager.detenerSonidoAmbiental("remo_canoa");
            }
        }
    }

    // Actualizar posición del listener (cámara) para audio 3D
    audioManager.actualizarPosicionListener(camera.getCameraPosition(), camera.getCameraDirection());

    // Obtener posición del personaje activo
    glm::vec3 posicionPersonajeActivo(0.0f);
    Entidad* personajeActivoEntidad = nullptr;

    // Buscar el personaje activo por nombre según personajeActual
    if (personajeActual == 1) {
        personajeActivoEntidad = buscarEntidad("cuphead_torso");
    }
    else if (personajeActual == 2) {
        personajeActivoEntidad = buscarEntidad("isaac_cuerpo");
    }
    else if (personajeActual == 3) {
        personajeActivoEntidad = buscarEntidad("gojo");
    }

    if (personajeActivoEntidad != nullptr) {
        posicionPersonajeActivo = personajeActivoEntidad->posicionLocal;

        // NUEVO: Gestionar sonido de caminata
        // Calcular distancia recorrida desde el último frame
        float distanciaRecorrida = glm::distance(posicionPersonajeActivo, posicionAnteriorPersonaje);
        float umbralMovimiento = 0.01f; // Umbral mínimo de movimiento para considerar que está caminando

        if (distanciaRecorrida > umbralMovimiento) {
            // El personaje se está moviendo
            if (!sonidoCaminataActivo) {
                // Iniciar sonido de caminata
                audioManager.reproducirSonidoAmbiental("caminando", posicionPersonajeActivo, 0.7f, true);
                sonidoCaminataActivo = true;
            }
            else {
                // Actualizar posición del sonido
                audioManager.actualizarPosicionSonidoAmbiental("caminando", posicionPersonajeActivo);
            }
        }
        else {
            // El personaje está quieto
            if (sonidoCaminataActivo) {
                // Detener sonido de caminata
                audioManager.detenerSonidoAmbiental("caminando");
                sonidoCaminataActivo = false;
            }
        }

        // Actualizar posición anterior
        posicionAnteriorPersonaje = posicionPersonajeActivo;
    }

    // Vector temporal para almacenar luces puntuales con sus distancias
    struct LuzConDistancia {
        PointLight luz;
        float distancia;
    };
    std::vector<LuzConDistancia> lucesTemporales;

    // Actualizar animaciones de las entidades que tengan componente de animacion
    for (auto* entidad : entidades) {
        if (entidad != nullptr) {
            if (entidad->nombreObjeto == "hollow") {
                entidad->animacion->actualizarAnimacion(0, deltaTime, 1.0);
            }
            if (entidad->nombreObjeto == "pedestal_piedra") {
                entidad->hijos[0]->animacion->actualizarAnimacion(0, deltaTime, 1.0);
            }
            if (entidad->nombreObjeto == "fuego_azul" || entidad->nombreObjeto == "fuego_azul2") {
                pointLightActual = *lightManager.getPointLight(AssetConstants::LightNames::PUNTUAL_AZUL);
                glm::vec3 posicionLuz = entidad->posicionLocal + glm::vec3(0.0f, 1.0f, 0.0f);
                pointLightActual.setPosition(posicionLuz);

                // Calcular distancia al personaje activo
                float distancia = glm::distance(posicionPersonajeActivo, posicionLuz);
                lucesTemporales.push_back({ pointLightActual, distancia });
            }
            // si esta activa la animacion se llama a la funcion de actualizarala
            if (entidad->nombreObjeto == "puerta_secret_room" && entidad->animacion->estaActiva(0)) {
                entidad->animacion->actualizarAnimacion(0, deltaTime, 1.0);
            }
            if (entidad->nombreObjeto == "pelota") {
                entidad->animacion->animateKeyframes();
            }

            // MODIFICADO: Gestionar animación y sonido del pez
            if (entidad->nombreObjeto == "pez" && entidad->animacion != nullptr) {
                bool animacionActivaAhora = entidad->animacion->play;

                // Actualizar animación
                entidad->animacion->animateKeyframes();

                // Gestionar sonido del pez
                if (animacionActivaAhora && !animacionPezActiva) {
                    // La animación acaba de activarse
                    glm::vec3 posicionPez = entidad->posicionLocal;
                    audioManager.reproducirSonidoAmbiental("pez", posicionPez, 0.8f, true);
                    animacionPezActiva = true;
                    std::cout << "[SceneInformation] Sonido del pez activado" << std::endl;
                }
                else if (!animacionActivaAhora && animacionPezActiva) {
                    // La animación acaba de desactivarse
                    audioManager.detenerSonidoAmbiental("pez");
                    animacionPezActiva = false;
                    std::cout << "[SceneInformation] Sonido del pez desactivado" << std::endl;
                }
                else if (animacionActivaAhora && animacionPezActiva) {
                    // Actualizar posición del sonido mientras la animación está activa
                    glm::vec3 posicionPez = entidad->posicionLocal;
                    audioManager.actualizarPosicionSonidoAmbiental("pez", posicionPez);
                }
            }

            // Procesar lámparas de calle y sus luces
            if (entidad->nombreObjeto.find("lampara_") == 0) {
                // Esta es una lámpara de calle
                // Buscar su hijo que es la luz
                for (auto* hijo : entidad->hijos) {
                    if (hijo != nullptr && hijo->nombreObjeto == "punto_luz") {
                        // Calcular la posición mundial de la luz
                        glm::vec3 posicionMundialLuz = glm::vec3(
                            entidad->transformacionLocal * glm::vec4(hijo->posicionLocal, 1.0f)
                        );

                        // Crear luz puntual con color amarillo cálido
                        pointLightActual = PointLight(
                            1.0f, 0.9f, 0.7f,  // Color amarillo cálido
                            0.3f, 0.8f,         // Intensidad ambiental y difusa
                            posicionMundialLuz.x, posicionMundialLuz.y, posicionMundialLuz.z,
                            0.3f, 0.1f, 0.005f   // Atenuación constante, lineal, exponencial
                        );

                        // Calcular distancia al personaje activo
                        float distancia = glm::distance(posicionPersonajeActivo, posicionMundialLuz);
                        lucesTemporales.push_back({ pointLightActual, distancia });
                        break; // Solo necesitamos procesar una luz por lámpara
                    }
                }
            }

            // Procesar lámparas del ring (base_light) y sus spotlights
            if (entidad->nombreObjeto.find("base_light_") == 0) {
                // Solo procesar si las luces del ring están activas
                if (!lucesRingActivas) continue;

                // Esta es una base de lámpara del ring
                // Buscar el lamp_ring hijo y su spotlight
                for (auto* lampRing : entidad->hijos) {
                    if (lampRing != nullptr && lampRing->nombreObjeto.find("lamp_ring_") == 0) {
                        // Encontramos el lamp_ring, ahora buscar el spotlight
                        for (auto* spotlight : lampRing->hijos) {
                            if (spotlight != nullptr && spotlight->nombreObjeto == "spotlight_ring") {
                                // Calcular la posición mundial del spotlight usando las matrices de transformación
                                glm::vec3 posicionMundialSpotlight = glm::vec3(
                                    entidad->transformacionLocal * lampRing->transformacionLocal * glm::vec4(spotlight->posicionLocal, 1.0f)
                                );

                                // La dirección es hacia abajo y hacia el ring
                                // El ring está aproximadamente en (2.0f, 32.2f, -149.5f)
                                glm::vec3 posicionRing(2.0f, 32.2f, -149.5f);
                                glm::vec3 direccionSpotlight = glm::normalize(posicionRing - posicionMundialSpotlight);

                                // Crear spotlight blanco apuntando al ring
                                spotLightActual = SpotLight(
                                    1.0f, 1.0f, 1.0f,  // Color blanco
                                    0.3f, 1.0f,         // Intensidad ambiental y difusa
                                    posicionMundialSpotlight.x, posicionMundialSpotlight.y, posicionMundialSpotlight.z,
                                    direccionSpotlight.x, direccionSpotlight.y, direccionSpotlight.z,
                                    1.0f, 0.05f, 0.01f, // Atenuación constante, lineal, exponencial
                                    30.0f               // Ángulo de apertura (edge)
                                );
                                agregarSpotLightActual(spotLightActual);
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    // Ordenar luces por distancia (las más cercanas primero)
    std::sort(lucesTemporales.begin(), lucesTemporales.end(),
        [](const LuzConDistancia& a, const LuzConDistancia& b) {
            return a.distancia < b.distancia;
        });

    // Agregar solo las 4 luces más cercanas
    int lucesAgregadas = 0;
    for (const auto& luzConDist : lucesTemporales) {
        if (lucesAgregadas >= 4) break;
        agregarLuzPuntualActual(luzConDist.luz);
        lucesAgregadas++;
    }

    // Actualizar animación del luchador
    Entidad* luchadorEntidad = nullptr;
    Entidad* primoEntidad = nullptr;
    for (auto* entidad : entidades) {
        if (entidad != nullptr) {
            if (entidad->nombreObjeto == "luchador_torso") luchadorEntidad = entidad;
            if (entidad->nombreObjeto == "primo") primoEntidad = entidad;
        }
    }
    if (luchadorEntidad != nullptr && primoEntidad != nullptr && luchadorEntidad->animacion != nullptr) {
        luchadorEntidad->animacion->animarLuchador(0, deltaTime, primoEntidad);
    }
}
// Funcion para actualizar cada frame con el input del usuario
void SceneInformation::actualizarFrameInput(bool* keys, GLfloat mouseXChange, GLfloat mouseYChange, GLfloat scrollChange, float deltaTime)
{
    limpiarSpotLightsActuales(); // Limpiar los spotlights actuales para actualizarlos cada frame
    limpiarLucesPuntualesActuales(); // Limpiar las luces puntuales actuales para actualizarlas cada frame

    // Actualizar cámara con input del usuario (mouse y teclado)
    camera.keyControl(keys, deltaTime);
    camera.mouseControl(mouseXChange, mouseYChange);
    camera.mouseScrollControl(scrollChange);  // Agregar control del scroll



    // Se maneja todo lo del teclado que no tenga que ver con la camara
    // 1: Cambia el modo tercera persona a cuphead
    if (keys[GLFW_KEY_1]) {
        Entidad* cuphead = buscarEntidad("cuphead_torso");
        if (cuphead != nullptr) {
            camera.setThirdPersonTarget(cuphead);
            personajeActual = 1;
        }
    }
    // 2: Cambia el modo tercera persona a Isaac
    if (keys[GLFW_KEY_2]) {
        Entidad* isaac = buscarEntidad("isaac_cuerpo");
        if (isaac != nullptr) {
            camera.setThirdPersonTarget(isaac);
            personajeActual = 2;
        }
    }
    // 3: Cambia el modo tercera persona a Gojo
    if (keys[GLFW_KEY_3]) {
        Entidad* gojo = buscarEntidad("gojo");
        if (gojo != nullptr) {
            camera.setThirdPersonTarget(gojo);
            personajeActual = 3;
        }
    }
    // Se itera sobre las entidades para hacer acciones especificas
    for (auto* entidad : entidades) {
        if (entidad->nombreObjeto == "puerta_secret_room") {
            // Z: Se abre o cierra la puerta secreta
            if (keys[GLFW_KEY_Z]) {
                entidad->animacion->activarAnimacion(0); // Activar animacion de abrir puerta
                audioManager.reproducirSonidoAmbiental("abrir_puerta", glm::vec3(180.0f, 8.25f, 200.0f), 0.5f, false);
            }
        }
        // P: Lanza la pelota del juego de pelota maya
        if (entidad->nombreObjeto == "pelota") {
            if (keys[GLFW_KEY_P]) {
                entidad->animacion->play = true;
            }
        }
        // B: Activa la animación del pez por keyframes
        if (entidad->nombreObjeto == "pez" && entidad->animacion != nullptr) {
            if (keys[GLFW_KEY_B]) {
                entidad->animacion->play = true;
            }
        }
    }

    // G: Activar/Desactivar animación de la canoa
    static bool teclaGPresionada = false;
    if (keys[GLFW_KEY_G]) {
        if (!teclaGPresionada) {
            for (auto* entidad : entidades) {
                if (entidad != nullptr && entidad->nombreObjeto == "canoa" && entidad->animacion != nullptr) {
                    if (entidad->animacion->estaActiva(0)) {
                        entidad->animacion->desactivarAnimacion(0);
                    }
                    else {
                        entidad->animacion->activarAnimacion(0);
                    }
                }
            }
            teclaGPresionada = true;
        }
    }
    else {
        teclaGPresionada = false;
    }

    // O: Activar/Desactivar luces del ring
    static bool teclaOPresionada = false;
    if (keys[GLFW_KEY_O]) {
        if (!teclaOPresionada) {
            lucesRingActivas = !lucesRingActivas;
            teclaOPresionada = true;
        }
    }
    else {
        teclaOPresionada = false;
    }

    // Y: Alternar volumen del soundtrack entre 0.0 (silenciado) y 0.1 (bajo)
    static bool teclaYPresionada = false;
    static bool soundtrackActivado = true; 
    if (keys[GLFW_KEY_Y]) {
        if (!teclaYPresionada) {
            soundtrackActivado = !soundtrackActivado;

            if (soundtrackActivado) {
                // Activar soundtrack con volumen bajo
                audioManager.setVolumenSoundtrack(0.1f);
                std::cout << "[SceneInformation] Soundtrack activado (volumen: 10%)" << std::endl;
            }
            else {
                // Silenciar soundtrack
                audioManager.setVolumenSoundtrack(0.0f);
                std::cout << "[SceneInformation] Soundtrack silenciado" << std::endl;
            }

            teclaYPresionada = true;
        }
    }
    else {
        teclaYPresionada = false;
    }
}

// Funcion para crear a los personjes
void SceneInformation::crearPersonajePrincipal()
{
    // Crear el modelo jerárquico de Cuphead siguiendo la estructura especificada
    // Padre: cuphead_torso

    // 1. Crear el torso (padre raíz)
    Entidad* cuphead_torso = new Entidad("cuphead_torso",
        glm::vec3(0.0f, 0.0f, 0.0f),       // Posición inicial en el mundo
        glm::vec3(-90.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.5f, 1.5f, 1.5f));      // Escala

    cuphead_torso->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_torso->setModelo(AssetConstants::ModelNames::CUPHEAD_TORSO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_TORSO));
    cuphead_torso->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // Crear y configurar componente de física para el torso
    cuphead_torso->fisica = new ComponenteFisico();
    cuphead_torso->fisica->habilitar(true);
    cuphead_torso->fisica->gravedad = -0.5f;

    // Crear y configurar componente de animación
    cuphead_torso->animacion = new ComponenteAnimacion(cuphead_torso);

    // 2. Crear la cabeza (hijo del torso)
    Entidad* cuphead_cabeza = new Entidad("cuphead_cabeza",
        glm::vec3(0.0f, 0.0f, 0.0f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),       // Sin rotación adicional
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala normal

    cuphead_cabeza->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_cabeza->setModelo(AssetConstants::ModelNames::CUPHEAD_CABEZA, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_CABEZA));
    cuphead_cabeza->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 3. Crear la leche (hijo de la cabeza)
    Entidad* cuphead_leche = new Entidad("cuphead_leche",
        glm::vec3(0.0f, 0.0f, 0.6f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_leche->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_leche->setModelo(AssetConstants::ModelNames::CUPHEAD_LECHE, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_LECHE));
    cuphead_leche->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    // Forzar la textura
    cuphead_leche->setTextura(AssetConstants::TextureNames::CUPHEAD_TEXTURE, textureManager.getTexture(AssetConstants::TextureNames::CUPHEAD_TEXTURE));

    // 4. Crear el popote (hijo de la leche)
    Entidad* cuphead_popote = new Entidad("cuphead_popote",
        glm::vec3(-0.3f, 0.0f, 0.3f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.2f, 0.2f, 0.2f));

    cuphead_popote->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_popote->setModelo(AssetConstants::ModelNames::CUPHEAD_POPOTE, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_POPOTE));
    cuphead_popote->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    // Forzar la textura
    cuphead_popote->setTextura(AssetConstants::TextureNames::POPOTE_ROJO, textureManager.getTexture(AssetConstants::TextureNames::POPOTE_ROJO));

    // 5. Crear brazo derecho (hijo del torso)
    Entidad* cuphead_brazo_derecho = new Entidad("cuphead_brazo_derecho",
        glm::vec3(-0.15f, 0.0f, 0.2f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, -35.0f, 0.0f),        // Rotación para bajar el brazo naturalmente
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_brazo_derecho->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_brazo_derecho->setModelo(AssetConstants::ModelNames::CUPHEAD_BRAZO_DERECHO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_BRAZO_DERECHO));
    cuphead_brazo_derecho->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 6. Crear antebrazo derecho (hijo del brazo derecho)
    Entidad* cuphead_antebrazo_derecho = new Entidad("cuphead_antebrazo_derecho",
        glm::vec3(-0.2f, 0.0f, -0.005f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, -15.0f, 0.0f),          // Rotación adicional para el antebrazo
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_antebrazo_derecho->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_antebrazo_derecho->setModelo(AssetConstants::ModelNames::CUPHEAD_ANTEBRAZO_DERECHO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_ANTEBRAZO_DERECHO));
    cuphead_antebrazo_derecho->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 7. Crear brazo izquierdo (hijo del torso)
    Entidad* cuphead_brazo_izquierdo = new Entidad("cuphead_brazo_izquierdo",
        glm::vec3(0.15f, 0.0f, 0.2f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 35.0f, 0.0f),      // Rotación para bajar el brazo naturalmente (opuesto al derecho)
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_brazo_izquierdo->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_brazo_izquierdo->setModelo(AssetConstants::ModelNames::CUPHEAD_BRAZO_IZQUIERDO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_BRAZO_IZQUIERDO));
    cuphead_brazo_izquierdo->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 8. Crear antebrazo izquierdo (hijo del brazo izquierdo)
    Entidad* cuphead_antebrazo_izquierdo = new Entidad("cuphead_antebrazo_izquierdo",
        glm::vec3(0.2f, 0.0f, -0.015f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 15.0f, 0.0f),        // Rotación adicional para el antebrazo (opuesto al derecho)
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_antebrazo_izquierdo->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_antebrazo_izquierdo->setModelo(AssetConstants::ModelNames::CUPHEAD_ANTEBRAZO_IZQUIERDO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_ANTEBRAZO_IZQUIERDO));
    cuphead_antebrazo_izquierdo->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 9. Crear muslo derecho (hijo del torso)
    Entidad* cuphead_muslo_derecho = new Entidad("cuphead_muslo_derecho",
        glm::vec3(-0.13f, 0.0f, -0.25f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_muslo_derecho->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_muslo_derecho->setModelo(AssetConstants::ModelNames::CUPHEAD_MUSLO_DERECHO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_MUSLO_DERECHO));
    cuphead_muslo_derecho->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 10. Crear pie derecho (hijo del muslo derecho)
    Entidad* cuphead_pie_derecho = new Entidad("cuphead_pie_derecho",
        glm::vec3(0.0f, 0.0f, -0.1f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_pie_derecho->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_pie_derecho->setModelo(AssetConstants::ModelNames::CUPHEAD_PIE_DERECHO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_PIE_DERECHO));
    cuphead_pie_derecho->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 11. Crear muslo izquierdo (hijo del torso)
    Entidad* cuphead_muslo_izquierdo = new Entidad("cuphead_muslo_izquierdo",
        glm::vec3(0.1f, 0.0f, -0.25f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_muslo_izquierdo->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_muslo_izquierdo->setModelo(AssetConstants::ModelNames::CUPHEAD_MUSLO_IZQUIERDO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_MUSLO_IZQUIERDO));
    cuphead_muslo_izquierdo->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 12. Crear pie izquierdo (hijo del muslo izquierdo)
    Entidad* cuphead_pie_izquierdo = new Entidad("cuphead_pie_izquierdo",
        glm::vec3(0.0f, 0.0f, -0.1f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    cuphead_pie_izquierdo->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_pie_izquierdo->setModelo(AssetConstants::ModelNames::CUPHEAD_PIE_IZQUIERDO, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_PIE_IZQUIERDO));
    cuphead_pie_izquierdo->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // Construir la jerarquía según el árbol especificado

    // Cabeza -> Leche -> Popote
    cuphead_leche->agregarHijo(cuphead_popote);
    cuphead_cabeza->agregarHijo(cuphead_leche);

    // Brazo derecho -> Antebrazo derecho
    cuphead_brazo_derecho->agregarHijo(cuphead_antebrazo_derecho);

    // Brazo izquierdo -> Antebrazo izquierdo
    cuphead_brazo_izquierdo->agregarHijo(cuphead_antebrazo_izquierdo);

    // Muslo derecho -> Pie derecho
    cuphead_muslo_derecho->agregarHijo(cuphead_pie_derecho);

    // Muslo izquierdo -> Pie izquierdo
    cuphead_muslo_izquierdo->agregarHijo(cuphead_pie_izquierdo);

    // Torso como padre de todo
    cuphead_torso->agregarHijo(cuphead_cabeza);
    cuphead_torso->agregarHijo(cuphead_brazo_derecho);
    cuphead_torso->agregarHijo(cuphead_brazo_izquierdo);
    cuphead_torso->agregarHijo(cuphead_muslo_derecho);
    cuphead_torso->agregarHijo(cuphead_muslo_izquierdo);

    // Actualizar transformaciones
    cuphead_torso->actualizarTransformacion();

    // Agregar a la escena (solo el padre, los hijos se renderizarán automáticamente)
    agregarEntidad(cuphead_torso);

    // Configurar la cámara en tercera persona siguiendo al personaje
    camera.setThirdPersonTarget(cuphead_torso);
}

// Crea al personaje de Isaac
void SceneInformation::crearIsaac()
{
    // Crear entidad de Isaac ya con Jerarquia
    Entidad* isaac_cuerpo = new Entidad("isaac_cuerpo",
        glm::vec3(0.0f, -1.15f, 10.0f),      // Posición inicial
        glm::vec3(0.0f, 180.0f, 0.0f),     // Rotación
        glm::vec3(0.8f, 0.8f, 0.8f));      // Escala

    Entidad* isaac_cabeza = new Entidad("isaac_cabeza",
        glm::vec3(0.0f, 1.5f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    Entidad* isaac_brazo_izquierdo = new Entidad("isaac_brazo_izquierdo",
        glm::vec3(-0.586f, 1.31f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    Entidad* isaac_brazo_derecho = new Entidad("isaac_brazo_derecho",
        glm::vec3(0.61f, 1.33f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    Entidad* isaac_pierna_izquierda = new Entidad("isaac_pierna_izquierda",
        glm::vec3(-0.48f, 0.7f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    Entidad* isaac_pierna_derecha = new Entidad("isaac_pierna_derecha",
        glm::vec3(0.455f, 0.7f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    isaac_cuerpo->setTipoObjeto(TipoObjeto::MODELO);
    isaac_cuerpo->nombreModelo = AssetConstants::ModelNames::ISAAC_CUERPO;
    isaac_cuerpo->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    // Crear y configurar componente de física
    isaac_cuerpo->fisica = new ComponenteFisico();
    isaac_cuerpo->fisica->habilitar(true);
    isaac_cuerpo->fisica->gravedad = -0.5f;

    // Crear y configurar componente de animación
    isaac_cuerpo->animacion = new ComponenteAnimacion(isaac_cuerpo);

    isaac_brazo_derecho->setTipoObjeto(TipoObjeto::MODELO);
    isaac_brazo_derecho->nombreModelo = AssetConstants::ModelNames::ISAAC_BRAZO_DERECHO;
    isaac_brazo_derecho->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    isaac_brazo_izquierdo->setTipoObjeto(TipoObjeto::MODELO);
    isaac_brazo_izquierdo->nombreModelo = AssetConstants::ModelNames::ISAAC_BRAZO_IZQUIERDO;
    isaac_brazo_izquierdo->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    isaac_cabeza->setTipoObjeto(TipoObjeto::MODELO);
    isaac_cabeza->nombreModelo = AssetConstants::ModelNames::ISAAC_CABEZA;
    isaac_cabeza->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    isaac_pierna_derecha->setTipoObjeto(TipoObjeto::MODELO);
    isaac_pierna_derecha->nombreModelo = AssetConstants::ModelNames::ISAAC_PIERNA_DERECHA;
    isaac_pierna_derecha->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    isaac_pierna_izquierda->setTipoObjeto(TipoObjeto::MODELO);
    isaac_pierna_izquierda->nombreModelo = AssetConstants::ModelNames::ISAAC_PIERNA_IZQUIERDA;
    isaac_pierna_izquierda->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    isaac_cuerpo->agregarHijo(isaac_cabeza);
    isaac_cuerpo->agregarHijo(isaac_brazo_izquierdo);
    isaac_cuerpo->agregarHijo(isaac_brazo_derecho);
    isaac_cuerpo->agregarHijo(isaac_pierna_izquierda);
    isaac_cuerpo->agregarHijo(isaac_pierna_derecha);

    agregarEntidad(isaac_cuerpo);
}

// Crear al luchador con modelo jerárquico
void SceneInformation::crearLuchador()
{
    // 1. Crear el torso (raíz)
    // Posicionar cerca de la pirámide
    Entidad* luchador_torso = new Entidad("luchador_torso",
        glm::vec3(-3.0f, 41.0f, -155.0f),
        glm::vec3(0.0f, -135.0f, 0.0f),
        glm::vec3(1.2f, 1.2f, 1.2f));

    luchador_torso->setTipoObjeto(TipoObjeto::MODELO);
    luchador_torso->setModelo(AssetConstants::ModelNames::LUCHADOR_TORSO,
        modelManager.getModel(AssetConstants::ModelNames::LUCHADOR_TORSO));
    luchador_torso->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    luchador_torso->animacion = new ComponenteAnimacion(luchador_torso);
    luchador_torso->animacion->activarAnimacion(0);  // Activate luchador animation

    // No necesita física ni animación ya que es estático

    // 2. Crear brazo derecho (hijo del torso)
    // Nota: El pivote está en la articulación
    Entidad* luchador_brazo_derecho = new Entidad("luchador_brazo_derecho",
        glm::vec3(0.0f, 0.0f, 0.0f),       // Posición relativa (ajustar según el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),       // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    luchador_brazo_derecho->setTipoObjeto(TipoObjeto::MODELO);
    luchador_brazo_derecho->setModelo(AssetConstants::ModelNames::LUCHADOR_BRAZO_DERECHO,
        modelManager.getModel(AssetConstants::ModelNames::LUCHADOR_BRAZO_DERECHO));
    luchador_brazo_derecho->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 3. Crear brazo izquierdo (hijo del torso)
    // Nota: El pivote está en la articulación
    Entidad* luchador_brazo_izquierdo = new Entidad("luchador_brazo_izquierdo",
        glm::vec3(0.0f, 0.0f, 0.0f),       // Posición relativa (ajustar según el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),       // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    luchador_brazo_izquierdo->setTipoObjeto(TipoObjeto::MODELO);
    luchador_brazo_izquierdo->setModelo(AssetConstants::ModelNames::LUCHADOR_BRAZO_IZQUIERDO,
        modelManager.getModel(AssetConstants::ModelNames::LUCHADOR_BRAZO_IZQUIERDO));
    luchador_brazo_izquierdo->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 4. Crear antebrazo izquierdo (hijo del brazo izquierdo)
    // Nota: El pivote está en la articulación
    Entidad* luchador_antebrazo_izquierdo = new Entidad("luchador_antebrazo_izquierdo",
        glm::vec3(0.0f, 0.0f, 0.0f),       // Posición relativa (ajustar según el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),       // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    luchador_antebrazo_izquierdo->setTipoObjeto(TipoObjeto::MODELO);
    luchador_antebrazo_izquierdo->setModelo(AssetConstants::ModelNames::LUCHADOR_ANTEBRAZO_IZQUIERDO,
        modelManager.getModel(AssetConstants::ModelNames::LUCHADOR_ANTEBRAZO_IZQUIERDO));
    luchador_antebrazo_izquierdo->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 5. Crear muslos (hijo del torso)
    Entidad* luchador_muslos = new Entidad("luchador_muslos",
        glm::vec3(0.0f, 0.0f, 0.0f),       // Posición relativa
        glm::vec3(0.0f, 0.0f, 0.0f),       // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    luchador_muslos->setTipoObjeto(TipoObjeto::MODELO);
    luchador_muslos->setModelo(AssetConstants::ModelNames::LUCHADOR_MUSLOS,
        modelManager.getModel(AssetConstants::ModelNames::LUCHADOR_MUSLOS));
    luchador_muslos->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // 6. Crear piernas (hijo de muslos)
    Entidad* luchador_piernas = new Entidad("luchador_piernas",
        glm::vec3(0.0f, 0.0f, 0.0f),       // Posición relativa
        glm::vec3(0.0f, 0.0f, 0.0f),       // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    luchador_piernas->setTipoObjeto(TipoObjeto::MODELO);
    luchador_piernas->setModelo(AssetConstants::ModelNames::LUCHADOR_PIERNAS,
        modelManager.getModel(AssetConstants::ModelNames::LUCHADOR_PIERNAS));
    luchador_piernas->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // Construir la jerarquía
    // Brazo izquierdo -> Antebrazo izquierdo
    luchador_brazo_izquierdo->agregarHijo(luchador_antebrazo_izquierdo);

    // Muslos -> Piernas
    luchador_muslos->agregarHijo(luchador_piernas);

    // Torso como padre de: brazo derecho, brazo izquierdo y muslos
    luchador_torso->agregarHijo(luchador_brazo_derecho);
    luchador_torso->agregarHijo(luchador_brazo_izquierdo);
    luchador_torso->agregarHijo(luchador_muslos);

    // Actualizar transformaciones
    luchador_torso->actualizarTransformacion();



    // Agregar a la escena (solo el padre, los hijos se renderizarán automáticamente)
    agregarEntidad(luchador_torso);
}

// Crear la boss room
void SceneInformation::crearBossRoom()
{
    // Crear entidad de la boss room
    Entidad* room = new Entidad("boss_room",
        glm::vec3(150.0f, 17.45f, -125.0f),      // Posición inicial
        glm::vec3(0.0f, 180.0f, 0.0f),     // Rotación
        glm::vec3(10.0f, 10.0f, 10.0f));      // Escala

    room->setTipoObjeto(TipoObjeto::MODELO);
    room->nombreModelo = AssetConstants::ModelNames::BOSS_ROOM;
    room->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    room->actualizarTransformacion();

    agregarEntidad(room);
}

// Crear la secret room
void SceneInformation::crearSecretRoom() {
    Entidad* room = new Entidad("secret room",
        glm::vec3(180.0f, 8.25f, 200.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(5.0f, 5.0f, 5.0f));      // Escala

    room->setTipoObjeto(TipoObjeto::MODELO);
    room->nombreModelo = AssetConstants::ModelNames::SECRET_ROOM;
    room->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    agregarEntidad(room);
}

// Crear puerta secreta
void SceneInformation::crearPuertaSecreta() {
    Entidad* puerta = new Entidad("puerta_secret_room",
        glm::vec3(180.2f, 1.3f, 183.7f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(5.0f, 5.0f, 5.0f));      // Escala

    puerta->setTipoObjeto(TipoObjeto::MODELO);
    puerta->nombreModelo = AssetConstants::ModelNames::PUERTA_SECRET_ROOM;
    puerta->nombreMaterial = AssetConstants::MaterialNames::OPACO;

    puerta->animacion = new ComponenteAnimacion(puerta);

    agregarEntidad(puerta);
}

// Crear sala del diablo
void SceneInformation::crearSalaDiablo() {
    // Las chinampas están en (-150.0f, -1.35f, -150.0f)
    // La boss_room está en (150.0f, 17.45f, -125.0f) con escala (10.0f, 10.0f, 10.0f) y rotación 180°
// Vamos a posicionar la sala del diablo detrás de las chinampas (Z más negativo)
// Ponemos la sala en Z = -230.0f (80 unidades detrás de las chinampas)

    Entidad* salaDiablo = new Entidad("sala_diablo",
        glm::vec3(-150.0f, 17.45f, -230.0f),   // Posición detrás de las chinampas
        glm::vec3(0.0f, 0.0f, 0.0f),           // Sin rotación (mirando al frente)
        glm::vec3(10.0f, 10.0f, 10.0f));       // Mismas dimensiones que boss_room

    salaDiablo->setTipoObjeto(TipoObjeto::MODELO);
    salaDiablo->nombreModelo = AssetConstants::ModelNames::SALA_DIABLO;
    salaDiablo->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    salaDiablo->actualizarTransformacion();

    agregarEntidad(salaDiablo);
}

// Funcion para crear el diablo
void SceneInformation::crearDiablo()
{
    // La sala del diablo está en (-150.0f, 17.45f, -230.0f) con escala (10.0f, 10.0f, 10.0f)
    // Posicionamos al diablo en el centro de la sala, flotando un poco
    
    Entidad* diablo = new Entidad("diablo",
        glm::vec3(-150.0f,-0.5f, -230.0f),    // Centro de la sala, elevado
        glm::vec3(0.0f, 180.0f, 0.0f),         // Rotación para que mire hacia el frente
        glm::vec3(0.2f, 0.2f, 0.2f));          // Escala para que sea imponente

    diablo->setTipoObjeto(TipoObjeto::MODELO);
    diablo->setModelo(AssetConstants::ModelNames::DIABLO, 
        modelManager.getModel(AssetConstants::ModelNames::DIABLO));
    diablo->setMaterial(AssetConstants::MaterialNames::OPACO, 
        materialManager.getMaterial(AssetConstants::MaterialNames::OPACO));
    diablo->actualizarTransformacion();

    agregarEntidad(diablo);
}

void SceneInformation::crearFogatas() {
    Entidad* fogata1 = new Entidad("fuego_rojo",
        glm::vec3(-50.0f, -1.0f, 50.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(0.5f, 0.5f, 0.5f));      // Escala

    fogata1->setTipoObjeto(TipoObjeto::MODELO);
    fogata1->nombreModelo = AssetConstants::ModelNames::FUEGO_ROJO;
    fogata1->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    agregarEntidad(fogata1);

    Entidad* fogata2 = new Entidad("fuego_azul",
        glm::vec3(195.0f, -1.0f, 210.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(0.3f, 0.3f, 0.3f));      // Escala

    fogata2->setTipoObjeto(TipoObjeto::MODELO);
    fogata2->nombreModelo = AssetConstants::ModelNames::FUEGO_AZUL;
    fogata2->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    agregarEntidad(fogata2);

    Entidad* fogata4 = new Entidad("fuego_azul2",
        glm::vec3(165.0f, -1.0f, 210.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(0.3f, 0.3f, 0.3f));      // Escala

    fogata4->setTipoObjeto(TipoObjeto::MODELO);
    fogata4->nombreModelo = AssetConstants::ModelNames::FUEGO_AZUL;
    fogata4->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    agregarEntidad(fogata4);

    Entidad* fogata3 = new Entidad("fuego_morado",
        glm::vec3(0.0f, -1.0f, -50.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(0.3f, 0.5f, 0.5f));      // Escala

    fogata3->setTipoObjeto(TipoObjeto::MODELO);
    fogata3->nombreModelo = AssetConstants::ModelNames::FUEGO_MORADO;
    fogata3->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    agregarEntidad(fogata3);

}

// Crea el objeto flotante de RKey de Isaac para posicionarlo en la secret room
void SceneInformation::crearRKey() {
    Entidad* rkey = new Entidad("rkey",
        glm::vec3(0.0f, 15.0f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    rkey->setTipoObjeto(TipoObjeto::MODELO);
    rkey->nombreModelo = AssetConstants::ModelNames::R_KEY;
    rkey->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    agregarEntidad(rkey);


}

// Crear item de isaac
void SceneInformation::crearComidaPerro() {

    Entidad* pedestal = new Entidad("pedestal_piedra",
        glm::vec3(180.0f, -1.0f, 200.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(2.0f, 2.0f, 2.0f));      // Escala

    pedestal->setTipoObjeto(TipoObjeto::MODELO);
    pedestal->nombreModelo = AssetConstants::ModelNames::PEDESTAL_PIEDRA;
    pedestal->nombreMaterial = AssetConstants::MaterialNames::OPACO;


    Entidad* comida = new Entidad("comida_perro",
        glm::vec3(0.0f, 0.0f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 180.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala
    comida->setTipoObjeto(TipoObjeto::MODELO);
    comida->nombreModelo = AssetConstants::ModelNames::COMIDA_PERRO;
    comida->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    comida->animacion = new ComponenteAnimacion(comida);

    pedestal->agregarHijo(comida);

    agregarEntidad(pedestal);
}

// Crear entidad de la cabeza olmeca
void SceneInformation::crearCabezaOlmeca()
{
    // Cabeza olmeca 1
    Entidad* cabezaOlmeca = new Entidad("cabezaOlmeca1",
        glm::vec3(20.0f, -1.0f, 200.0f),      // Posición inicial
        glm::vec3(0.0f, 45.0f, 0.0f),     // Rotación
        glm::vec3(3.0f, 3.0f, 3.0f));      // Escala

    cabezaOlmeca->setTipoObjeto(TipoObjeto::MODELO);
    cabezaOlmeca->nombreModelo = AssetConstants::ModelNames::CABEZA_OLMECA;
    cabezaOlmeca->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    cabezaOlmeca->actualizarTransformacion();
    agregarEntidad(cabezaOlmeca);

    // Cabeza olmeca 2
    cabezaOlmeca = new Entidad("cabezaOlmeca2",
        glm::vec3(-20.0f, -1.0f, 200.0f),      // Posición inicial
        glm::vec3(0.0f, 115.0f, 0.0f),     // Rotación
        glm::vec3(3.0f, 3.0f, 3.0f));      // Escala

    cabezaOlmeca->setTipoObjeto(TipoObjeto::MODELO);
    cabezaOlmeca->nombreModelo = AssetConstants::ModelNames::CABEZA_OLMECA;
    cabezaOlmeca->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    cabezaOlmeca->actualizarTransformacion();
    agregarEntidad(cabezaOlmeca);
}

// Crear entidad de la pirámide
void SceneInformation::crearPiramide()
{
    // Pirámide 1
    Entidad* piramide = new Entidad("piramide1",
        glm::vec3(0.0f, -4.0f, -150.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),         // Rotación
        glm::vec3(1.2f, 1.2f, 1.2f));        // Escala

    piramide->setTipoObjeto(TipoObjeto::MODELO);
    piramide->nombreModelo = AssetConstants::ModelNames::PIRAMIDE;
    piramide->nombreMaterial = AssetConstants::MaterialNames::OPACO;

    Entidad* ring = new Entidad("ring_pelea",
        glm::vec3(2.0f, 36.2f, 0.5f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),         // Rotación
        glm::vec3(0.9f, 0.9f, 0.9f));        // Escala

    ring->setTipoObjeto(TipoObjeto::MODELO);
    ring->nombreModelo = AssetConstants::ModelNames::RING_PELEA;
    ring->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;

    piramide->agregarHijo(ring);
    agregarEntidad(piramide);

}

// Crear entidad del camino empedrado
void SceneInformation::crearCamino()
{
    // Crear el camino empedrado que une las cabezas olmecas con la pirámide
    Entidad* camino = new Entidad("camino_empedrado",
        glm::vec3(2.0f, -1.0f, 25.0f),      // Posición centrada entre olmecas y pirámide
        glm::vec3(0.0f, 0.0f, 0.0f),        // Sin rotación
        glm::vec3(1.8f, 1.0f, 1.0f));       // Escala normal (el mesh ya tiene el tamaño correcto)

    camino->setTipoObjeto(TipoObjeto::MESH);
    camino->nombreMesh = AssetConstants::MeshNames::CAMINO;
    camino->nombreTextura = AssetConstants::TextureNames::EMPEDRADO;
    camino->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    camino->actualizarTransformacion();
    agregarEntidad(camino);
}

// Crear entidad del prisma cuadrado con textura de agua
void SceneInformation::crearChinampaAgua()
{
    // Crear el prisma cuadrado con textura de agua
    Entidad* chinampaAgua = new Entidad("chinampa_agua",
        glm::vec3(-150.0f, -1.35f, -150.0f),     // Posición a la izquierda del camino
        glm::vec3(0.0f, 0.0f, 0.0f),        // Sin rotación
        glm::vec3(8.0f, 8.0f, 8.0f));       // Escala normal

    chinampaAgua->setTipoObjeto(TipoObjeto::MESH);
    chinampaAgua->nombreMesh = AssetConstants::MeshNames::CHINAMPA_AGUA;
    chinampaAgua->nombreTextura = AssetConstants::TextureNames::AGUA;
    chinampaAgua->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    chinampaAgua->actualizarTransformacion();
    agregarEntidad(chinampaAgua);
}

// Crear 9 prismas pequeños distribuidos en cuadrícula 3x3 sobre el prisma de agua
void SceneInformation::crearIslas()
{
    // Posición base del prisma de agua (sin cambios)
    glm::vec3 posicionBase(-150.0f, -1.35f, -150.0f);
    float escalaBase = 8.0f;

    // Calcular altura sobre el prisma de agua
    // El prisma de agua tiene altura 0.1 * escala = 0.8
    // Los prismas pequeños deben estar justo encima
    float alturaAguaEscalada = 0.1f * escalaBase;
    float alturaPrismasPequenos = 0.1f * 2.0f; // escala de prismas pequeños será 2.0
    float yPrismas = posicionBase.y + alturaAguaEscalada + alturaPrismasPequenos;

    // Dimensiones del prisma de agua escalado: 10 * 8 = 80 unidades
    // Queremos distribuir 9 prismas en 3x3
    // Espacio total disponible: 80 unidades
    // Espacio entre prismas: dividir en 3 secciones con espaciado uniforme

    // Tamaño de cada prisma pequeño escalado: 2 * 2 = 4 unidades
    // Espacio disponible para los 3 prismas en una fila: 80 unidades
    // Espaciado entre centros: 80 / 3 = 26.67 unidades aproximadamente
    float espaciado = 26.67f;

    // Desplazamiento desde el centro del prisma base
    float desplazamientoInicial = -espaciado; // Comenzar desde la izquierda

    // Semilla para generación aleatoria (usando tiempo actual)
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int contador = 0;
    for (int fila = 0; fila < 3; fila++) {
        for (int columna = 0; columna < 3; columna++) {
            // Calcular posición de cada prisma pequeño
            float xPos = posicionBase.x + desplazamientoInicial + (columna * espaciado);
            float zPos = posicionBase.z + desplazamientoInicial + (fila * espaciado);

            // Crear el prisma pequeño (chinampa isla)
            std::string nombrePrisma = "chinampa_isla_" + std::to_string(contador);
            Entidad* chinampaIsla = new Entidad(nombrePrisma,
                glm::vec3(xPos, yPrismas, zPos),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(10.0f, 5.0f, 10.0f));

            chinampaIsla->setTipoObjeto(TipoObjeto::MESH);
            chinampaIsla->nombreMesh = AssetConstants::MeshNames::CHINAMPA_ISLA;
            if (columna == 1) {
                chinampaIsla->nombreTextura = AssetConstants::TextureNames::PASTO;
            }
            else {
                chinampaIsla->nombreTextura = AssetConstants::TextureNames::TIERRA;
            }
            chinampaIsla->nombreMaterial = AssetConstants::MaterialNames::OPACO;
            chinampaIsla->actualizarTransformacion();

            // Si es columna 1, agregar 3 maíces como hijos con posiciones aleatorias
            if (columna == 1) {
                // Dimensiones de la chinampa isla en su espacio local
                // El mesh va de -1 a 1 en x y z (2 unidades base)
                // Con escala 10.0f, el rango real es de -10 y 10 en x y z
                float rangoX = 1.5f;
                float rangoZ = 1.5f;

                for (int i = 0; i < 10; i++) {
                    // Generar posición aleatoria dentro de la chinampa
                    // Reducir el rango un poco para que no queden en los bordes
                    float xAleatorio = ((std::rand() % 100) / 100.0f - 0.5f) * rangoX * 0.9f;
                    float zAleatorio = ((std::rand() % 100) / 100.0f - 0.5f) * rangoZ * 0.9f;

                    // Crear entidad de maíz como hijo
                    std::string nombreMaiz = "maiz_" + std::to_string(contador) + "_" + std::to_string(i);
                    Entidad* maiz = new Entidad(nombreMaiz,
                        glm::vec3(xAleatorio, 0.0f, zAleatorio),  // Posición aleatoria relativa al prisma
                        glm::vec3(-90.0f, 0.0f, 0.0f),            // Rotación que ajustaste
                        glm::vec3(0.009f, 0.009f, 0.009f));       // Escala que ajustaste
                    maiz->setTipoObjeto(TipoObjeto::MODELO);
                    maiz->nombreModelo = AssetConstants::ModelNames::MAIZ;
                    maiz->nombreMaterial = AssetConstants::MaterialNames::OPACO;
                    maiz->actualizarTransformacion();

                    // Agregar maíz como hijo del prisma
                    chinampaIsla->agregarHijo(maiz);
                }
            }

            agregarEntidad(chinampaIsla);

            contador++;
        }
    }
}

// Crear árbol con parámetros configurables
void SceneInformation::crearArbol(const std::string& tipoArbol,
    const glm::vec3& posicion,
    const glm::vec3& rotacion,
    const glm::vec3& escala,
    const std::string& nombre)
{
    // Determinar el modelo de árbol a usar
    std::string modeloArbol;
    std::string nombreArbol;

    if (tipoArbol == "A") {
        modeloArbol = AssetConstants::ModelNames::ARBOL_A;
        nombreArbol = nombre.empty() ? "arbol_a" : nombre;
    }
    else if (tipoArbol == "B") {
        modeloArbol = AssetConstants::ModelNames::ARBOL_B;
        nombreArbol = nombre.empty() ? "arbol_b" : nombre;
    }
    else if (tipoArbol == "C") {
        modeloArbol = AssetConstants::ModelNames::ARBOL_C;
        nombreArbol = nombre.empty() ? "arbol_c" : nombre;
    }
    else {
        // Por defecto usar árbol A
        modeloArbol = AssetConstants::ModelNames::ARBOL_A;
        nombreArbol = nombre.empty() ? "arbol_desconocido" : nombre;
    }

    // Crear entidad del árbol
    Entidad* arbol = new Entidad(nombreArbol,
        posicion,
        rotacion,
        escala);

    arbol->setTipoObjeto(TipoObjeto::MODELO);
    arbol->nombreModelo = modeloArbol;
    arbol->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    arbol->actualizarTransformacion();
    agregarEntidad(arbol);
}

// Crear 24 árboles alrededor de la chinampa de agua (6 por lado)
void SceneInformation::crearArbolesAlrededorChinampa()
{
    // Posición y escala de la chinampa de agua
    glm::vec3 posicionChinampa(-150.0f, -1.35f, -150.0f);
    float escalaChinampa = 8.0f;

    // Dimensiones de la chinampa escalada (el mesh base es 10x10)
    float anchoChinampa = 10.0f * escalaChinampa; // 80 unidades
    float altoChinampa = 10.0f * escalaChinampa;  // 80 unidades

    // Distancia desde el borde de la chinampa
    float distanciaBorde = 8.0f;

    // Número de árboles por lado
    int arbolesPorLado = 6;

    // Espaciado entre árboles
    float espaciado = anchoChinampa / (arbolesPorLado - 1);

    // Array de tipos de árbol para selección aleatoria
    std::string tiposArbol[] = { "A", "B", "C" };
    int numTipos = 3;

    // Inicializar semilla aleatoria si no se ha hecho
    static bool semillaInicializada = false;
    if (!semillaInicializada) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        semillaInicializada = true;
    }

    int contadorArboles = 0;

    // Lado NORTE (arriba, Z negativo)
    for (int i = 0; i < arbolesPorLado; i++) {
        float x = posicionChinampa.x - anchoChinampa / 2.0f + (i * espaciado);
        float z = posicionChinampa.z - altoChinampa / 2.0f - distanciaBorde;

        // Seleccionar tipo aleatorio
        std::string tipo = tiposArbol[std::rand() % numTipos];

        // Rotación aleatoria en Y (0-360 grados)
        float rotY = static_cast<float>(std::rand() % 360);

        // Escala base con variación aleatoria (0.9 a 1.1)
        float variacionEscala = 0.9f + (std::rand() % 20) / 100.0f;

        // Aplicar escala específica según el tipo de árbol
        float escalaFinal;
        if (tipo == "A") {
            escalaFinal = 8.0f * variacionEscala;
        }
        else if (tipo == "B") {
            escalaFinal = 1.5f * variacionEscala;
        }
        else { // tipo == "C"
            escalaFinal = 30.0f * variacionEscala;
        }

        std::string nombre = "arbol_norte_" + std::to_string(i);
        crearArbol(tipo,
            glm::vec3(x, -1.0f, z),
            glm::vec3(0.0f, rotY, 0.0f),
            glm::vec3(escalaFinal),
            nombre);
        contadorArboles++;
    }

    // Lado SUR (abajo, Z positivo)
    for (int i = 0; i < arbolesPorLado; i++) {
        float x = posicionChinampa.x - anchoChinampa / 2.0f + (i * espaciado);
        float z = posicionChinampa.z + altoChinampa / 2.0f + distanciaBorde;

        std::string tipo = tiposArbol[std::rand() % numTipos];
        float rotY = static_cast<float>(std::rand() % 360);
        float variacionEscala = 0.9f + (std::rand() % 20) / 100.0f;

        // Aplicar escala específica según el tipo de árbol
        float escalaFinal;
        if (tipo == "A") {
            escalaFinal = 8.0f * variacionEscala;
        }
        else if (tipo == "B") {
            escalaFinal = 1.5f * variacionEscala;
        }
        else { // tipo == "C"
            escalaFinal = 30.0f * variacionEscala;
        }

        std::string nombre = "arbol_sur_" + std::to_string(i);
        crearArbol(tipo,
            glm::vec3(x, -1.0f, z),
            glm::vec3(0.0f, rotY, 0.0f),
            glm::vec3(escalaFinal),
            nombre);
        contadorArboles++;
    }

    // Lado OESTE (izquierda, X negativo)
    for (int i = 0; i < arbolesPorLado; i++) {
        float x = posicionChinampa.x - anchoChinampa / 2.0f - distanciaBorde;
        float z = posicionChinampa.z - altoChinampa / 2.0f + (i * espaciado);

        std::string tipo = tiposArbol[std::rand() % numTipos];
        float rotY = static_cast<float>(std::rand() % 360);
        float variacionEscala = 0.9f + (std::rand() % 20) / 100.0f;

        // Aplicar escala específica según el tipo de árbol
        float escalaFinal;
        if (tipo == "A") {
            escalaFinal = 8.0f * variacionEscala;
        }
        else if (tipo == "B") {
            escalaFinal = 1.5f * variacionEscala;
        }
        else { // tipo == "C"
            escalaFinal = 30.0f * variacionEscala;
        }

        std::string nombre = "arbol_oeste_" + std::to_string(i);
        crearArbol(tipo,
            glm::vec3(x, -1.0f, z),
            glm::vec3(0.0f, rotY, 0.0f),
            glm::vec3(escalaFinal),
            nombre);
        contadorArboles++;
    }

    // Lado ESTE (derecha, X positivo)
    for (int i = 0; i < arbolesPorLado; i++) {
        float x = posicionChinampa.x + anchoChinampa / 2.0f + distanciaBorde;
        float z = posicionChinampa.z - altoChinampa / 2.0f + (i * espaciado);

        std::string tipo = tiposArbol[std::rand() % numTipos];
        float rotY = static_cast<float>(std::rand() % 360);
        float variacionEscala = 0.9f + (std::rand() % 20) / 100.0f;

        // Aplicar escala específica según el tipo de árbol
        float escalaFinal;
        if (tipo == "A") {
            escalaFinal = 8.0f * variacionEscala;
        }
        else if (tipo == "B") {
            escalaFinal = 1.5f * variacionEscala;
        }
        else { // tipo == "C"
            escalaFinal = 30.0f * variacionEscala;
        }

        std::string nombre = "arbol_este_" + std::to_string(i);
        crearArbol(tipo,
            glm::vec3(x, -1.0f, z),
            glm::vec3(0.0f, rotY, 0.0f),
            glm::vec3(escalaFinal),
            nombre);
        contadorArboles++;
    }
}

// Crear canoa con maya jerárquica que navega alrededor de la chinampa del centro
void SceneInformation::crearCanoa()
{
    // Posición y escala de la chinampa de agua
    glm::vec3 posicionChinampa(-150.0f, -1.35f, -150.0f);
    float escalaChinampa = 8.0f;

    // Espaciado de las chinampas islas (igual que en crearIslas)
    float espaciado = 26.67f;
    float desplazamientoInicial = -espaciado;
    
    // Calcular posición de la chinampa del centro (fila 1, columna 1)
    float xCentro = posicionChinampa.x + desplazamientoInicial + (1 * espaciado);
    float zCentro = posicionChinampa.z + desplazamientoInicial + (1 * espaciado);
    
    // Altura del agua (encima del prisma de agua)
    float alturaAgua = posicionChinampa.y + 0.1f * escalaChinampa + 0.2f;

    // Margen alrededor de la chinampa del centro (radio de la órbita)
    float radioOrbita = 14.0f; // Distancia desde el centro de la chinampa

    // Posición inicial: esquina norte (mirando hacia Z positivo)
    glm::vec3 posicionInicial(
        xCentro - radioOrbita,
        alturaAgua,
        zCentro - radioOrbita
    );

    // Crear entidad de la canoa (padre)
    Entidad* canoa = new Entidad("canoa",
        posicionInicial,
        glm::vec3(0.0f, 0.0f, 0.0f),  // Inicialmente mirando al frente (hacia Z+)
        glm::vec3(1.5f, 1.5f, 1.5f));  // Escala

    canoa->setTipoObjeto(TipoObjeto::MODELO);
    canoa->nombreModelo = AssetConstants::ModelNames::CANOA;
    canoa->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    canoa->animacion = new ComponenteAnimacion(canoa);
    canoa->animacion->activarAnimacion(0);  // Activar animación de la canoa
    canoa->actualizarTransformacion();

    // Crear maya como hijo de la canoa
    Entidad* maya = new Entidad("maya_canoa",
        glm::vec3(0.0f, 0.3f, 0.0f),  // Posición relativa encima de la canoa
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    maya->setTipoObjeto(TipoObjeto::MODELO);
    maya->nombreModelo = AssetConstants::ModelNames::MAYA_CANOA;
    maya->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    maya->actualizarTransformacion();

    // Agregar maya como hijo de la canoa
    canoa->agregarHijo(maya);

    // Agregar canoa a la escena
    agregarEntidad(canoa);

   
}

// Crear cancha de juego de pelota Maya con jerarquía
void SceneInformation::crearCanchaPelotaMaya()
{
    // Posición central de la cancha (entre las chinampas y la pirámide)
    glm::vec3 posicionCentroCancha(100.0f, -1.0f, 100.0f);
    float separacionParedes = 100.0f; // Distancia entre las dos paredes paralelas

    // Crear pared rectangular izquierda 
    Entidad* paredIzquierda = new Entidad("cancha_pared_izquierda",
        glm::vec3(posicionCentroCancha.x - separacionParedes / 2.0f,
            posicionCentroCancha.y,
            posicionCentroCancha.z),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(5.0f, 5.0f, 5.0f));

    paredIzquierda->setTipoObjeto(TipoObjeto::MESH);
    paredIzquierda->nombreMesh = AssetConstants::MeshNames::CANCHA_PARED;
    paredIzquierda->nombreTextura = AssetConstants::TextureNames::MAYAN_BRICKS;
    paredIzquierda->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    paredIzquierda->actualizarTransformacion();

    // Crear techo triangular izquierdo
    Entidad* techoIzquierdo = new Entidad("cancha_techo_izquierdo",
        glm::vec3(-1.5f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    techoIzquierdo->setTipoObjeto(TipoObjeto::MESH);
    techoIzquierdo->nombreMesh = AssetConstants::MeshNames::CANCHA_TECHO;
    techoIzquierdo->nombreTextura = AssetConstants::TextureNames::MAYAN_BRICKS;
    techoIzquierdo->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    techoIzquierdo->actualizarTransformacion();

    // Agregar techo como hijo de la pared izquierda
    paredIzquierda->agregarHijo(techoIzquierdo);

    // Agregar pared izquierda a la escena
    agregarEntidad(paredIzquierda);


    // Crear pared rectangular derecha 
    Entidad* paredDerecha = new Entidad("cancha_pared_derecha",
        glm::vec3(posicionCentroCancha.x + separacionParedes / 2.0f,
            posicionCentroCancha.y,
            posicionCentroCancha.z),
        glm::vec3(0.0f, 180.0f, 0.0f),     // Rotación de 180 grados para que mire hacia la izquierda
        glm::vec3(5.0f, 5.0f, 5.0f));      // Escala

    paredDerecha->setTipoObjeto(TipoObjeto::MESH);
    paredDerecha->nombreMesh = AssetConstants::MeshNames::CANCHA_PARED;
    paredDerecha->nombreTextura = AssetConstants::TextureNames::MAYAN_BRICKS;
    paredDerecha->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    paredDerecha->actualizarTransformacion();

    // Crear techo triangular derecho 
    Entidad* techoDerecho = new Entidad("cancha_techo_derecho",
        glm::vec3(-1.5f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    techoDerecho->setTipoObjeto(TipoObjeto::MESH);
    techoDerecho->nombreMesh = AssetConstants::MeshNames::CANCHA_TECHO;
    techoDerecho->nombreTextura = AssetConstants::TextureNames::MAYAN_BRICKS;
    techoDerecho->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    techoDerecho->actualizarTransformacion();

    Entidad* aroCancha = new Entidad("cancha_aro",
        glm::vec3(2.0f, 3.5f, 0.0f),
        glm::vec3(90.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));
    aroCancha->setTipoObjeto(TipoObjeto::MESH);
    aroCancha->nombreMesh = AssetConstants::MeshNames::TOROIDE;
    aroCancha->nombreTextura = AssetConstants::TextureNames::MAYAN_BRICKS;
    aroCancha->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    aroCancha->actualizarTransformacion();

    // Agregar techo y aro como hijos de la pared derecha
    paredDerecha->agregarHijo(techoDerecho);
    paredDerecha->agregarHijo(aroCancha);

    // Agregar pared derecha a la escena
    agregarEntidad(paredDerecha);
}
void SceneInformation::crearHollow() {
    Entidad* cabeza_hollow = new Entidad("hollow",
        glm::vec3(155.0f, 4.0f, -125.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(0.3f, 0.3f, 0.3f));      // Escala

    cabeza_hollow->setTipoObjeto(TipoObjeto::MODELO);
    cabeza_hollow->nombreModelo = AssetConstants::ModelNames::CABEZA_HOLLOW;
    cabeza_hollow->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    cabeza_hollow->actualizarTransformacion();

    // Crear y configurar componente de animación
    cabeza_hollow->animacion = new ComponenteAnimacion(cabeza_hollow);


    Entidad* cuerpo_hollow1 = new Entidad("cuerpo_hollow1",
        glm::vec3(0.0f, 0.0f, -16.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    cuerpo_hollow1->setTipoObjeto(TipoObjeto::MODELO);
    cuerpo_hollow1->nombreModelo = AssetConstants::ModelNames::CUERPO2_HOLLOW;
    cuerpo_hollow1->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    cuerpo_hollow1->actualizarTransformacion();

    Entidad* cuerpo_hollow2 = new Entidad("cuerpo_hollow2",
        glm::vec3(1.0f, 2.0f, -14.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    cuerpo_hollow2->setTipoObjeto(TipoObjeto::MODELO);
    cuerpo_hollow2->nombreModelo = AssetConstants::ModelNames::CUERPO1_HOLLOW;
    cuerpo_hollow2->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    cuerpo_hollow2->actualizarTransformacion();


    Entidad* cuerpo_hollow3 = new Entidad("cuerpo_hollow3",
        glm::vec3(1.0f, 1.0f, -14.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    cuerpo_hollow3->setTipoObjeto(TipoObjeto::MODELO);
    cuerpo_hollow3->nombreModelo = AssetConstants::ModelNames::CUERPO1_HOLLOW;
    cuerpo_hollow3->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    cuerpo_hollow3->actualizarTransformacion();


    Entidad* cuerpo_hollow4 = new Entidad("cuerpo_hollow4",
        glm::vec3(0.0f, 0.0f, -10.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    cuerpo_hollow4->setTipoObjeto(TipoObjeto::MODELO);
    cuerpo_hollow4->nombreModelo = AssetConstants::ModelNames::CUERPO3_HOLLOW;
    cuerpo_hollow4->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    cuerpo_hollow4->actualizarTransformacion();

    cabeza_hollow->agregarHijo(cuerpo_hollow1);
    cuerpo_hollow1->agregarHijo(cuerpo_hollow2);
    cuerpo_hollow2->agregarHijo(cuerpo_hollow3);
    cuerpo_hollow3->agregarHijo(cuerpo_hollow4);

    agregarEntidad(cabeza_hollow);
}

// Nuevo: Crear jerarquía Gojo (pruebagojo y sus partes)
void SceneInformation::crearGojo()
{
    Entidad* gojo_cuerpo = new Entidad("gojo",
        glm::vec3(3.0f, -1.0f, 10.0f),
        glm::vec3(0.0f, 180.0f, 0.0f),
        glm::vec3(5.0f, 5.0f, 5.0f));

    Entidad* gojo_brazo_izq = new Entidad("gojobrazoizq",
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    Entidad* gojo_brazo_der = new Entidad("gojobrazoder",
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    Entidad* gojo_pierna_izquierda = new Entidad("gojopiernaizq",
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    Entidad* gojo_pierna_der = new Entidad("gojopiernader",
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    Entidad* gojo_rodilla_izq = new Entidad("gojorodillaizq",
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    Entidad* gojo_rodilla_der = new Entidad("gojorodillader",
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    // Configurar tipos y recursos
    gojo_cuerpo->setTipoObjeto(TipoObjeto::MODELO);
    gojo_cuerpo->setModelo(AssetConstants::ModelNames::GOJO, modelManager.getModel(AssetConstants::ModelNames::GOJO));
    gojo_cuerpo->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    gojo_cuerpo->actualizarTransformacion();

    gojo_brazo_izq->setTipoObjeto(TipoObjeto::MODELO);
    gojo_brazo_izq->setModelo(AssetConstants::ModelNames::GOJO_BRAZO_IZQ, modelManager.getModel(AssetConstants::ModelNames::GOJO_BRAZO_IZQ));
    gojo_brazo_izq->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    gojo_brazo_der->setTipoObjeto(TipoObjeto::MODELO);
    gojo_brazo_der->setModelo(AssetConstants::ModelNames::GOJO_BRAZO_DER, modelManager.getModel(AssetConstants::ModelNames::GOJO_BRAZO_DER));
    gojo_brazo_der->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    gojo_pierna_izquierda->setTipoObjeto(TipoObjeto::MODELO);
    gojo_pierna_izquierda->setModelo(AssetConstants::ModelNames::GOJO_PIERNA_IZQ, modelManager.getModel(AssetConstants::ModelNames::GOJO_PIERNA_IZQ));
    gojo_pierna_izquierda->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    gojo_pierna_der->setTipoObjeto(TipoObjeto::MODELO);
    gojo_pierna_der->setModelo(AssetConstants::ModelNames::GOJO_PIERNA_DER, modelManager.getModel(AssetConstants::ModelNames::GOJO_PIERNA_DER));
    gojo_pierna_der->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    gojo_rodilla_izq->setTipoObjeto(TipoObjeto::MODELO);
    gojo_rodilla_izq->setModelo(AssetConstants::ModelNames::GOJO_RODILLA_IZQ, modelManager.getModel(AssetConstants::ModelNames::GOJO_RODILLA_IZQ));
    gojo_rodilla_izq->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    gojo_rodilla_der->setTipoObjeto(TipoObjeto::MODELO);
    gojo_rodilla_der->setModelo(AssetConstants::ModelNames::GOJO_RODILLA_DER, modelManager.getModel(AssetConstants::ModelNames::GOJO_RODILLA_DER));
    gojo_rodilla_der->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // Armar jerarquía
    gojo_pierna_izquierda->agregarHijo(gojo_rodilla_izq);
    gojo_pierna_der->agregarHijo(gojo_rodilla_der);
    gojo_cuerpo->agregarHijo(gojo_pierna_izquierda);
    gojo_cuerpo->agregarHijo(gojo_pierna_der);
    gojo_cuerpo->agregarHijo(gojo_brazo_izq);
    gojo_cuerpo->agregarHijo(gojo_brazo_der);


    // Agregar componentes de física
    gojo_cuerpo->fisica = new ComponenteFisico();
    gojo_cuerpo->fisica->habilitar(true);
    gojo_cuerpo->fisica->gravedad = -0.5f;

    // Agregar a la escena
    agregarEntidad(gojo_cuerpo);
}

// BLACKHOLE
void SceneInformation::crearBlackHole()
{
    Entidad* blackhole = new Entidad("blackhole",
        glm::vec3(-160.0f, 30.0f, 70.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.6f, 0.6f, 0.6f));


    blackhole->setTipoObjeto(TipoObjeto::MODELO);
    blackhole->setModelo(AssetConstants::ModelNames::BLACKHOLE, modelManager.getModel(AssetConstants::ModelNames::BLACKHOLE));
    blackhole->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    blackhole->actualizarTransformacion();

    agregarEntidad(blackhole);
}

void SceneInformation::crearPyramideMuseo()
{
    Entidad* pyramidemuseo = new Entidad("pyramidemuseo",
        glm::vec3(-160.0f, -1.0f, 150.0f),
        glm::vec3(0.0f, 270.0f, 0.0f),
        glm::vec3(9.0f, 9.0f, 9.0f));

    Entidad* pyramidemuseo1 = new Entidad("pyramidemuseo",
        glm::vec3(-160.0f, -1.0f, 70.0f),
        glm::vec3(0.0f, 270.0f, 0.0f),
        glm::vec3(9.0f, 9.0f, 9.0f));

    pyramidemuseo->setTipoObjeto(TipoObjeto::MODELO);
    pyramidemuseo->setModelo(AssetConstants::ModelNames::PYRAMIDEMUSEO, modelManager.getModel(AssetConstants::ModelNames::PYRAMIDEMUSEO));
    pyramidemuseo->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    pyramidemuseo->actualizarTransformacion();

    pyramidemuseo1->setTipoObjeto(TipoObjeto::MODELO);
    pyramidemuseo1->setModelo(AssetConstants::ModelNames::PYRAMIDEMUSEO, modelManager.getModel(AssetConstants::ModelNames::PYRAMIDEMUSEO));
    pyramidemuseo1->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    pyramidemuseo1->actualizarTransformacion();

    agregarEntidad(pyramidemuseo);
    agregarEntidad(pyramidemuseo1);
}
// Crear un pez en el agua con animación por keyframes
void SceneInformation::crearPez() {
    // Posición base del prisma de agua
    glm::vec3 posicionBase(-150.0f, -1.35f, -150.0f);
    float escalaBase = 8.0f;

    // Calcular altura sobre el prisma de agua
    float alturaAguaEscalada = 0.1f * escalaBase;
    float yPez = posicionBase.y + alturaAguaEscalada - 0.3;

    // Posición inicial del pez (centro del agua)
    glm::vec3 posicionInicial(posicionBase.x  +8.5f, yPez, posicionBase.z-13.0f);

    // Crear entidad del pez
    Entidad* pez = new Entidad("pez",
        posicionInicial,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));

    pez->setTipoObjeto(TipoObjeto::MODELO);
    pez->setModelo(AssetConstants::ModelNames::PEZ,
        modelManager.getModel(AssetConstants::ModelNames::PEZ));
    pez->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    pez->actualizarTransformacion();

    // Crear y configurar componente de animación
    pez->animacion = new ComponenteAnimacion(pez);

    // Cargar keyframes para la animación del pez (similar a la pelota)
    pez->animacion->cargarKeyframes();

    agregarEntidad(pez);
}

// Mercado
// Mercado
void SceneInformation::creaCarpa()
{
    // Estructura para almacenar datos de configuración de cada entidad del mercado
    struct ConfigEntidadMercado {
        std::string tipo;           // "puestokekas", "carpaymesa", "carpavacia", "carpabuena", "puestopescados"
        glm::vec3 posicion;
        glm::vec3 rotacion;
        glm::vec3 escala;
    };

    // Definir todas las entidades del mercado en un vector
    std::vector<ConfigEntidadMercado> configuraciones = {
        // Primera fila
        {"puestokekas", glm::vec3(-33.0f, -1.0f, 23.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.7f, 3.7f, 3.7f)},
        {"carpaymesa", glm::vec3(-33.0f, -1.0f, 39.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"carpavacia", glm::vec3(-34.0f, -1.0f, 53.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"carpaymesa", glm::vec3(-33.0f, -1.0f, 67.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"carpabuena", glm::vec3(-32.0f, -1.0f, 84.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"puestopescados", glm::vec3(-20.0f, -1.0f, 81.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.5f, 3.5f, 3.5f)},
        {"puestopescados", glm::vec3(-20.0f, -1.0f, 85.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.5f, 3.5f, 3.5f)},
        {"puestokekas", glm::vec3(-32.0f, -1.0f, 98.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(3.5f, 3.5f, 3.5f)},
        {"carpavacia", glm::vec3(-34.0f, -1.0f, 110.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"carpabuena", glm::vec3(-32.0f, -1.0f, 124.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},

        // Segunda fila
        {"carpavacia", glm::vec3(-54.0f, -1.0f, 23.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"carpabuena", glm::vec3(-52.0f, -1.0f, 37.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"puestopescados", glm::vec3(-33.0f, -1.0f, 37.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"puestokekas", glm::vec3(-53.0f, -1.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.7f, 3.7f, 3.7f)},
        {"carpavacia", glm::vec3(-54.0f, -1.0f, 63.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"carpaymesa", glm::vec3(-53.0f, -1.0f, 80.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"puestokekas", glm::vec3(-52.0f, -1.0f, 93.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(3.5f, 3.5f, 3.5f)},
        {"carpabuena", glm::vec3(-52.0f, -1.0f, 105.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"puestopescados", glm::vec3(-35.0f, -1.0f, 105.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"carpaymesa", glm::vec3(-53.0f, -1.0f, 125.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},

        // Tercera fila
        {"carpaymesa", glm::vec3(-73.0f, -1.0f, 23.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"carpavacia", glm::vec3(-74.0f, -1.0f, 38.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"puestokekas", glm::vec3(-73.0f, -1.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.7f, 3.7f, 3.7f)},
        {"carpabuena", glm::vec3(-72.0f, -1.0f, 65.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"carpaymesa", glm::vec3(-73.0f, -1.0f, 80.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"carpavacia", glm::vec3(-74.0f, -1.0f, 95.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f)},
        {"puestopescados", glm::vec3(-55.0f, -1.0f, 105.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"puestopescados", glm::vec3(-55.0f, -1.0f, 110.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"puestopescados", glm::vec3(-55.0f, -1.0f, 115.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)},
        {"carpabuena", glm::vec3(-72.0f, -1.0f, 126.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)}
    };

    // Función auxiliar para crear y configurar una entidad
    auto crearEntidadMercado = [this](const std::string& tipo, const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& esc, int indice) {
        // Generar nombre único para la entidad
        std::string nombreEntidad = tipo + "_" + std::to_string(indice);

        // Crear la entidad
        Entidad* entidad = new Entidad(nombreEntidad, pos, rot, esc);
        entidad->setTipoObjeto(TipoObjeto::MODELO);

        // Asignar el modelo correspondiente según el tipo
        if (tipo == "carpavacia") {
            entidad->setModelo(AssetConstants::ModelNames::CARPAVACIA,
                modelManager.getModel(AssetConstants::ModelNames::CARPAVACIA));
        }
        else if (tipo == "carpaymesa") {
            entidad->setModelo(AssetConstants::ModelNames::CARPAYMESA,
                modelManager.getModel(AssetConstants::ModelNames::CARPAYMESA));
        }
        else if (tipo == "carpabuena") {
            entidad->setModelo(AssetConstants::ModelNames::CARPABUENA,
                modelManager.getModel(AssetConstants::ModelNames::CARPABUENA));
        }
        else if (tipo == "puestopescados") {
            entidad->setModelo(AssetConstants::ModelNames::PUESTOPESCADOS,
                modelManager.getModel(AssetConstants::ModelNames::PUESTOPESCADOS));
        }
        else if (tipo == "puestokekas") {
            entidad->setModelo(AssetConstants::ModelNames::PUESTOKEKAS,
                modelManager.getModel(AssetConstants::ModelNames::PUESTOKEKAS));
        }

        entidad->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
            materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
        entidad->actualizarTransformacion();

        return entidad;
        };

    // Crear todas las entidades usando un ciclo
    for (size_t i = 0; i < configuraciones.size(); i++) {
        const auto& config = configuraciones[i];
        Entidad* entidad = crearEntidadMercado(config.tipo, config.posicion, config.rotacion, config.escala, i);
        agregarEntidad(entidad);
    }
}
// Crear pelota del juego de pelota
void SceneInformation::crearPelotaDeJuegoDePelota() {

    Entidad* pelota = new Entidad("pelota",
        glm::vec3(132.0f, 6.0f, 75.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    pelota->setTipoObjeto(TipoObjeto::MESH);
    pelota->nombreMesh = AssetConstants::MeshNames::ESFERA;
    pelota->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    pelota->nombreTextura = AssetConstants::TextureNames::CAUCHO;
    // Crear y configurar componente de animación
    pelota->animacion = new ComponenteAnimacion(pelota);
    pelota->animacion->cargarKeyframes();
    agregarEntidad(pelota);

}

void SceneInformation::crearObjetosGeometricos()
{
    // Crear esfera de prueba 1 - Flotando en el aire
    Entidad* esfera1 = new Entidad("esfera1",
        glm::vec3(5.0f, 3.0f, 0.0f),      // Posición: a la derecha y elevada
        glm::vec3(0.0f, 0.0f, 0.0f),       // Sin rotación
        glm::vec3(2.0f, 2.0f, 2.0f));      // Escala 2x

    esfera1->setTipoObjeto(TipoObjeto::MESH);
    esfera1->nombreMesh = AssetConstants::MeshNames::ESFERA;
    esfera1->nombreTextura = AssetConstants::TextureNames::TIERRA;
    esfera1->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    esfera1->actualizarTransformacion();
    agregarEntidad(esfera1);

    // Crear esfera de prueba 2 - En el suelo
    Entidad* esfera2 = new Entidad("esfera2",
        glm::vec3(-5.0f, 1.0f, 5.0f),     // Posición: a la izquierda
        glm::vec3(0.0f, 0.0f, 0.0f),      // Rotación
        glm::vec3(1.5f, 1.5f, 1.5f));      // Escala

    esfera2->setTipoObjeto(TipoObjeto::MESH);
    esfera2->nombreMesh = AssetConstants::MeshNames::ESFERA;
    esfera2->nombreTextura = AssetConstants::TextureNames::LADRILLO;
    esfera2->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    esfera2->actualizarTransformacion();
    agregarEntidad(esfera2);

    // Crear esfera de prueba 3 - Pequeña y cercana
    Entidad* esfera3 = new Entidad("esfera3",
        glm::vec3(0.0f, 2.0f, -8.0f),      // Posición: al frente
        glm::vec3(0.0f, 0.0f, 0.0f),       // Sin rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala normal

    esfera3->setTipoObjeto(TipoObjeto::MESH);
    esfera3->nombreMesh = AssetConstants::MeshNames::ESFERA;
    esfera3->nombreTextura = AssetConstants::TextureNames::PASTO;
    esfera3->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    esfera3->actualizarTransformacion();
    agregarEntidad(esfera3);
}

// Crear entidad de Primo cerca de la pirámide
void SceneInformation::crearPrimo()
{
    Entidad* primoEntidad = new Entidad("primo",
        glm::vec3(3.0f, 39.0f, -152.0f),
        glm::vec3(0.0f, 45.0f, 0.0f),
        glm::vec3(1.5f, 1.5f, 1.5f));

    primoEntidad->setTipoObjeto(TipoObjeto::MODELO);
    primoEntidad->nombreModelo = AssetConstants::ModelNames::PRIMO;
    primoEntidad->nombreMaterial = AssetConstants::MaterialNames::BRILLANTE;
    primoEntidad->actualizarTransformacion();

    agregarEntidad(primoEntidad);
}


void SceneInformation::crearPoblacionMaya() {
    // Definir área del mercado basándose en las posiciones de los puestos
    float xMin = -80.0f;
    float xMax = -25.0f;
    float zMin = 20.0f;
    float zMax = 130.0f;
    float y = -1.0f;

    // Crear 3 traders en posiciones aleatorias
    for (int i = 0; i < 3; i++) {
        // Generar posición aleatoria dentro del área del mercado
        float xAleatorio = xMin + (std::rand() % 100) / 100.0f * (xMax - xMin);
        float zAleatorio = zMin + (std::rand() % 100) / 100.0f * (zMax - zMin);

        // Generar rotación aleatoria en Y (0-360 grados)
        float rotYAleatorio = static_cast<float>(std::rand() % 360);

        // Generar escala con ligera variación 
        float variacionEscala = 2.3f + (std::rand() % 20) / 100.0f;
        float escalaFinal = 1.0f * variacionEscala;

        // Crear entidad trader
        std::string nombreTrader = "trader_" + std::to_string(i);
        Entidad* trader = new Entidad(nombreTrader,
            glm::vec3(xAleatorio, y, zAleatorio),
            glm::vec3(0.0f, rotYAleatorio, 0.0f),
            glm::vec3(escalaFinal, escalaFinal, escalaFinal));

        trader->setTipoObjeto(TipoObjeto::MODELO);
        trader->setModelo(AssetConstants::ModelNames::TRADER,
            modelManager.getModel(AssetConstants::ModelNames::TRADER));
        trader->setMaterial(AssetConstants::MaterialNames::OPACO,
            materialManager.getMaterial(AssetConstants::MaterialNames::OPACO));
        trader->actualizarTransformacion();

        agregarEntidad(trader);
    }

    // Crear 3 mayas de canoa en posiciones aleatorias
    for (int i = 0; i < 3; i++) {
        // Generar posición aleatoria dentro del área del mercado
        float xAleatorio = xMin + (std::rand() % 100) / 100.0f * (xMax - xMin);
        float zAleatorio = zMin + (std::rand() % 100) / 100.0f * (zMax - zMin);

        // Generar rotación aleatoria en Y (0-360 grados)
        float rotYAleatorio = static_cast<float>(std::rand() % 360);

        // Generar escala con ligera variación
        float variacionEscala = 2.3f + (std::rand() % 20) / 100.0f;
        float escalaFinal = 1.0f * variacionEscala;

        // Crear entidad trader
        std::string nombreCanoa = "mayacanoa_" + std::to_string(i);
        Entidad* canoa = new Entidad(nombreCanoa,
            glm::vec3(xAleatorio, y, zAleatorio),
            glm::vec3(0.0f, rotYAleatorio, 0.0f),
            glm::vec3(escalaFinal, escalaFinal, escalaFinal));

        canoa->setTipoObjeto(TipoObjeto::MODELO);
        canoa->setModelo(AssetConstants::ModelNames::MAYA_CANOA,
            modelManager.getModel(AssetConstants::ModelNames::MAYA_CANOA));
        canoa->setMaterial(AssetConstants::MaterialNames::OPACO,
            materialManager.getMaterial(AssetConstants::MaterialNames::OPACO));
        canoa->actualizarTransformacion();

        agregarEntidad(canoa);
    }

    // Crear 3 mayas merchant en posiciones aleatorias
    for (int i = 0; i < 3; i++) {
        // Generar posición aleatoria dentro del área del mercado
        float xAleatorio = xMin + (std::rand() % 100) / 100.0f * (xMax - xMin);
        float zAleatorio = zMin + (std::rand() % 100) / 100.0f * (zMax - zMin);

        // Generar rotación aleatoria en Y (0-360 grados)
        float rotYAleatorio = static_cast<float>(std::rand() % 360);

        // Generar escala con ligera variación
        float variacionEscala = 2.3f + (std::rand() % 20) / 100.0f;
        float escalaFinal = 1.0f * variacionEscala;

        // Crear entidad trader
        std::string nombreMerchant = "merchant_" + std::to_string(i);
        Entidad* merchant = new Entidad(nombreMerchant,
            glm::vec3(xAleatorio, y, zAleatorio),
            glm::vec3(0.0f, rotYAleatorio, 0.0f),
            glm::vec3(escalaFinal, escalaFinal, escalaFinal));

        merchant->setTipoObjeto(TipoObjeto::MODELO);
        merchant->setModelo(AssetConstants::ModelNames::MERCHANT,
            modelManager.getModel(AssetConstants::ModelNames::MERCHANT));
        merchant->setMaterial(AssetConstants::MaterialNames::OPACO,
            materialManager.getMaterial(AssetConstants::MaterialNames::OPACO));
        merchant->actualizarTransformacion();

        agregarEntidad(merchant);
    }

    // Crear 3 pipilas en posiciones aleatorias
    for (int i = 0; i < 3; i++) {
        // Generar posición aleatoria dentro del área del mercado
        float xAleatorio = xMin + (std::rand() % 100) / 100.0f * (xMax - xMin);
        float zAleatorio = zMin + (std::rand() % 100) / 100.0f * (zMax - zMin);

        // Generar rotación aleatoria en Y (0-360 grados)
        float rotYAleatorio = static_cast<float>(std::rand() % 360);

        // Generar escala con ligera variación
        float variacionEscala = 2.3f + (std::rand() % 20) / 100.0f;
        float escalaFinal = 1.0f * variacionEscala;

        // Crear entidad trader
        std::string nombrePipila = "pipila_" + std::to_string(i);
        Entidad* pipila = new Entidad(nombrePipila,
            glm::vec3(xAleatorio, y, zAleatorio),
            glm::vec3(0.0f, rotYAleatorio, 0.0f),
            glm::vec3(escalaFinal, escalaFinal, escalaFinal));

        pipila->setTipoObjeto(TipoObjeto::MODELO);
        pipila->setModelo(AssetConstants::ModelNames::PIPILA,
            modelManager.getModel(AssetConstants::ModelNames::PIPILA));
        pipila->setMaterial(AssetConstants::MaterialNames::OPACO,
            materialManager.getMaterial(AssetConstants::MaterialNames::OPACO));
        pipila->actualizarTransformacion();

        agregarEntidad(pipila);
    }
}

// Crear lámparas de calle a lo largo del camino empedrado
void SceneInformation::crearLamparasCalles()
{
    // Posición inicial: 20 unidades desde las cabezas olmecas (Z=200)
    // Las cabezas olmecas están en Z=200, entonces empezamos en Z=180
    float zInicio = 180.0f;

    // Posición final: la pirámide está en Z=-150
    float zFin = -150.0f;

    // Separación entre lámparas
    float separacion = 40.0f;

    // El camino está centrado aproximadamente en X=2.0f
    // Colocar lámparas a ambos lados del camino
    float xCamino = 2.0f;
    float distanciaLateral = 12.0f; // Distancia desde el centro del camino

    // Altura de la lámpara sobre el suelo
    float yLampara = -1.0f;

    // Escala de las lámparas
    glm::vec3 escalaLampara(1.5f, 1.5f, 1.5f);

    // Contador de lámparas
    int contadorLampara = 0;

    // Generar lámparas cada 40 unidades desde zInicio hasta zFin
    for (float z = zInicio; z >= zFin; z -= separacion) {
        // Lámpara lado izquierdo (X negativo)
        std::string nombreIzq = "lampara_izq_" + std::to_string(contadorLampara);
        Entidad* lamparaIzq = new Entidad(nombreIzq,
            glm::vec3(xCamino - distanciaLateral, yLampara, z),
            glm::vec3(0.0f, 0.0f, 0.0f),
            escalaLampara);

        lamparaIzq->setTipoObjeto(TipoObjeto::MODELO);
        lamparaIzq->setModelo(AssetConstants::ModelNames::STREET_LAMP,
            modelManager.getModel(AssetConstants::ModelNames::STREET_LAMP));
        lamparaIzq->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
            materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

        // Crear luz puntual como hijo de la lámpara
        // Posición relativa: encima de la lámpara (altura ajustada)
        Entidad* luzIzq = new Entidad(nombreIzq + "_luz",
            glm::vec3(0.0f, 5.0f, 0.0f), // Posición relativa respecto a la lámpara
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f));

        luzIzq->nombreObjeto = "punto_luz";

        lamparaIzq->agregarHijo(luzIzq);
        lamparaIzq->actualizarTransformacion();
        agregarEntidad(lamparaIzq);

        // Lámpara lado derecho (X positivo)
        std::string nombreDer = "lampara_der_" + std::to_string(contadorLampara);
        Entidad* lamparaDer = new Entidad(nombreDer,
            glm::vec3(xCamino + distanciaLateral, yLampara, z),
            glm::vec3(0.0f, 0.0f, 0.0f),
            escalaLampara);

        lamparaDer->setTipoObjeto(TipoObjeto::MODELO);
        lamparaDer->setModelo(AssetConstants::ModelNames::STREET_LAMP,
            modelManager.getModel(AssetConstants::ModelNames::STREET_LAMP));
        lamparaDer->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
            materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

        // Crear luz puntual como hijo de la lámpara
        Entidad* luzDer = new Entidad(nombreDer + "_luz",
            glm::vec3(0.0f, 5.0f, 0.0f), // Posición relativa respecto a la lámpara
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f));

        luzDer->nombreObjeto = "punto_luz";

        lamparaDer->agregarHijo(luzDer);
        lamparaDer->actualizarTransformacion();
        agregarEntidad(lamparaDer);

        contadorLampara++;
    }
}

// Crear lámparas sobre la pirámide iluminando el ring
void SceneInformation::crearLamparasRing()
{
    // La pirámide está en (0.0f, -4.0f, -150.0f)
    // El ring está en la cima de la pirámide en posición relativa (2.0f, 36.2f, 0.5f)
    // Posición mundial del ring: (2.0f, 32.2f, -149.5f)

    glm::vec3 posicionPiramide(0.0f, -4.0f, -150.0f);
    glm::vec3 posicionRelativaRing(2.0f, 36.2f, 0.5f);
    glm::vec3 posicionRing = posicionPiramide + posicionRelativaRing;

    // Altura de las lámparas sobre el ring
    float alturaLampara = 12.0f;

    // Distancia lateral desde el centro del ring
    float distanciaLateral = 6.0f;

    // Escala de la base (cilindro)
    glm::vec3 escalaCilindro(1.0f, 3.0f, 1.0f);

    // Escala del lamp_ring
    glm::vec3 escalaLampRing(1.5f, 1.5f, 1.5f);

    // Crear lámpara 1 (lado izquierdo del ring)
    Entidad* baseLamp1 = new Entidad("base_light_1",
        glm::vec3(posicionRing.x - distanciaLateral, posicionRing.y + alturaLampara - 2.0, posicionRing.z - 6.0),
        glm::vec3(0.0f, 0.0f, 0.0f),
        escalaCilindro);

    baseLamp1->setTipoObjeto(TipoObjeto::MESH);
    baseLamp1->setMesh(AssetConstants::MeshNames::CILINDRO,
        meshManager.getMesh(AssetConstants::MeshNames::CILINDRO));
    baseLamp1->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // Crear lamp_ring como hijo
    Entidad* lampRing1 = new Entidad("lamp_ring_1",
        glm::vec3(0.0f, 1.5f, 0.0f), // Arriba del cilindro
        glm::vec3(0.0f, 0.0f, 0.0f),
        escalaLampRing);

    lampRing1->setTipoObjeto(TipoObjeto::MODELO);
    lampRing1->setModelo(AssetConstants::ModelNames::LAMP_RING,
        modelManager.getModel(AssetConstants::ModelNames::LAMP_RING));
    lampRing1->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // Crear spotlight como hijo de lamp_ring
    Entidad* spotlight1 = new Entidad("spotlight_ring_1",
        glm::vec3(0.0f, 0.0f, 0.0f), // En el centro del lamp_ring
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    spotlight1->nombreObjeto = "spotlight_ring";

    lampRing1->agregarHijo(spotlight1);
    baseLamp1->agregarHijo(lampRing1);
    baseLamp1->actualizarTransformacion();
    agregarEntidad(baseLamp1);

    // Crear lámpara 2 (lado derecho del ring)
    Entidad* baseLamp2 = new Entidad("base_light_2",
        glm::vec3(posicionRing.x + distanciaLateral, posicionRing.y + alturaLampara - 2.0, posicionRing.z + 6.0),
        glm::vec3(0.0f, 0.0f, 0.0f),
        escalaCilindro);

    baseLamp2->setTipoObjeto(TipoObjeto::MESH);
    baseLamp2->setMesh(AssetConstants::MeshNames::CILINDRO,
        meshManager.getMesh(AssetConstants::MeshNames::CILINDRO));
    baseLamp2->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // Crear lamp_ring como hijo
    Entidad* lampRing2 = new Entidad("lamp_ring_2",
        glm::vec3(0.0f, 1.5f, 0.0f), // Arriba del cilindro
        glm::vec3(0.0f, 180.0f, 0.0f),
        escalaLampRing);

    lampRing2->setTipoObjeto(TipoObjeto::MODELO);
    lampRing2->setModelo(AssetConstants::ModelNames::LAMP_RING,
        modelManager.getModel(AssetConstants::ModelNames::LAMP_RING));
    lampRing2->setMaterial(AssetConstants::MaterialNames::BRILLANTE,
        materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

    // Crear spotlight como hijo de lamp_ring
    Entidad* spotlight2 = new Entidad("spotlight_ring_2",
        glm::vec3(0.0f, 0.0f, 0.0f), // En el centro del lamp_ring
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    spotlight2->nombreObjeto = "spotlight_ring";

    lampRing2->agregarHijo(spotlight2);
    baseLamp2->agregarHijo(lampRing2);
    baseLamp2->actualizarTransformacion();
    agregarEntidad(baseLamp2);
}
void SceneInformation::agregarEntidad(Entidad* entidad)
{
    if (entidad != nullptr) {
        // Vincular recursos (modelos, meshes y texturas) antes de agregar
        vincularRecursos(entidad);
        entidades.push_back(entidad);
    }
}

void SceneInformation::removerEntidad(Entidad* entidad)
{
    if (entidad == nullptr) return;

    // Buscar y remover la entidad del vector
    auto it = std::find(entidades.begin(), entidades.end(), entidad);
    if (it != entidades.end()) {
        entidades.erase(it);
    }
    // Nota: NO se elimina la entidad, solo se elimina del vector
}

Entidad* SceneInformation::buscarEntidad(const std::string& nombre)
{
    // Buscar una entidad por nombre de objeto
    for (auto* entidad : entidades) {
        if (entidad != nullptr) {
            // Buscar primero por nombreObjeto si existe
            if (!entidad->nombreObjeto.empty() && entidad->nombreObjeto == nombre) {
                return entidad;
            }
        }
    }
    return nullptr;
}


void SceneInformation::setSkyboxActual(const std::string& skyboxName)
{
    skyboxActual = skyboxManager.getSkybox(skyboxName);
}

void SceneInformation::setLuzDireccional(const DirectionalLight& light)
{
    luzDireccional = light;
}

// La mas lejana se elimina
bool SceneInformation::agregarLuzPuntualActual(const PointLight& light)
{
    // Si hay espacio disponible, agregar directamente
    if (pointLightCountActual < MAX_POINT_LIGHTS) {
        pointLightsActuales[pointLightCountActual] = light;
        pointLightCountActual++;
        return true;
    }

    // Si no hay espacio, buscar y reemplazar la luz más lejana en un solo paso
    distanciaMaxima = glm::distance(camera.getCameraPosition(), light.GetPosition());
    indiceLuzMasLejana = -1;

    // Buscar la luz más lejana
    for (int i = 0; i < pointLightCountActual; i++) {
        distanciaLuzActual = glm::distance(camera.getCameraPosition(), pointLightsActuales[i].GetPosition());

        if (distanciaLuzActual > distanciaMaxima) {
            distanciaMaxima = distanciaLuzActual;
            indiceLuzMasLejana = i;
        }
    }

    // Si encontramos una luz más lejana, reemplazarla y retornar true
    if (indiceLuzMasLejana != -1) {
        pointLightsActuales[indiceLuzMasLejana] = light;
        return true;
    }

    // La nueva luz está más lejos que todas las existentes, no agregarla
    return false;
}

void SceneInformation::limpiarLucesPuntualesActuales()
{
    pointLightCountActual = 0;
}

bool SceneInformation::agregarSpotLightActual(const SpotLight& light)
{
    if (spotLightCountActual >= MAX_SPOT_LIGHTS) {
        return false; // No hay espacio para más luces actuales
    }

    // Agregar solo al arreglo de luces actuales
    spotLightsActuales[spotLightCountActual] = light;
    spotLightCountActual++;

    return true;
}

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
    audioManager.cargarSoundtrack("cuphead_song", "Audio/Soundtrack/cuphead_song.wav");

    // Cargar efectos de sonido
    audioManager.cargarSonidoAmbiental("abrir_puerta", "Audio/SFX/door_open.mp3");

    // Cargar SFX del remo de la canoa
    audioManager.cargarSonidoAmbiental("remo_canoa", "Audio/SFX/remo.wav");

    // Cargar SFX del pez
    audioManager.cargarSonidoAmbiental("pez", "Audio/SFX/pez.wav");

    // Cargar SFX de caminata
    audioManager.cargarSonidoAmbiental("caminando", "Audio/SFX/caminando.wav");

    // Cargar sonido ambiental del público en el ring
    audioManager.cargarSonidoAmbiental("publico_lucha", "Audio/Environmental/publico_lucha.wav");

    // Cargar sonido ambiental del tianguis (mercado)
    audioManager.cargarSonidoAmbiental("tianguis", "Audio/Environmental/tianguis.wav");

    // Cargar sonido de grillos 10 veces (una por cada posición)
    for (int i = 0; i < 10; i++) {
        std::string nombreGrillo = "grillo_" + std::to_string(i);
        audioManager.cargarSonidoAmbiental(nombreGrillo, "Audio/Environmental/grillo.wav");
    }

    // Reproducir el soundtrack en loop con volumen tenue (30% del máximo)
    if (!audioManager.reproducirSoundtrack("cuphead_song", 0.1f)) {
        std::cerr << "[SceneInformation] Advertencia: No se pudo reproducir el soundtrack" << std::endl;
        std::cerr << "[SceneInformation] Verifica que el archivo existe en: Audio/Soundtrack/cuphead_song.wav" << std::endl;
    }
    else {
        std::cout << "[SceneInformation] Soundtrack reproduciéndose en loop (volumen: 30%)" << std::endl;
    }

    // Establecer volumen maestro
    audioManager.setVolumenMaestro(0.4f); // 40% del volumen maestro
    audioManager.setVolumenSoundtrack(0.1f);

    // Reproducir sonido ambiental del público en el ring
    audioManager.reproducirSonidoAmbiental("publico_lucha", glm::vec3(2.0f, 36.2f, -149.5f), 0.6f, true);
    std::cout << "[SceneInformation] Sonido del público en el ring activado" << std::endl;

    // Generar 10 posiciones aleatorias para los grillos en el escenario
    generarPosicionesGrillos();
}


void SceneInformation::generarPosicionesGrillos()
{
    // Definir el área del escenario donde pueden aparecer grillos
    // Evitamos áreas específicas como el ring, la chinampa de agua, etc.

    struct AreaEscenario {
        float xMin, xMax;
        float zMin, zMax;
        std::string descripcion;
    };

    // Definir varias áreas del escenario
    std::vector<AreaEscenario> areas = {
        // Área alrededor del camino (evitando el camino mismo)
        {-20.0f, 25.0f, -50.0f, 180.0f, "camino"},
        // Área del mercado
        {-80.0f, -20.0f, 20.0f, 130.0f, "mercado"},
        // Área cerca de las chinampas (pero no en el agua)
        {-200.0f, -160.0f, -200.0f, -100.0f, "chinampas"},
        // Área cerca de la cancha de pelota
        {70.0f, 130.0f, 70.0f, 130.0f, "cancha_pelota"},
        // Área entre el camino y la boss room
        {120.0f, 180.0f, -160.0f, -100.0f, "boss_area"}
    };

    // Limpiar vector si ya tenía datos
    posicionesGrillos.clear();

    // Inicializar generador aleatorio si no se ha hecho
    static bool semillaInicializada = false;
    if (!semillaInicializada) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        semillaInicializada = true;
    }

    // Generar 10 posiciones aleatorias
    for (int i = 0; i < 10; i++) {
        // Seleccionar un área aleatoria
        int areaIndex = std::rand() % areas.size();
        const AreaEscenario& area = areas[areaIndex];

        // Generar posición aleatoria dentro del área seleccionada
        float x = area.xMin + (std::rand() % 100) / 100.0f * (area.xMax - area.xMin);
        float z = area.zMin + (std::rand() % 100) / 100.0f * (area.zMax - area.zMin);
        float y = -1.0f; // Altura del suelo

        glm::vec3 posicion(x, y, z);
        posicionesGrillos.push_back(posicion);

        std::cout << "[SceneInformation] Grillo " << i << " posicionado en: ("
            << x << ", " << y << ", " << z << ") - Área: " << area.descripcion << std::endl;
    }
}

void SceneInformation::activarGrillos()
{
    // Reproducir sonido de grillo en cada posición generada
    // IMPORTANTE: Todos usan el MISMO nombre "grillo" que ya fue cargado
    for (size_t i = 0; i < posicionesGrillos.size(); i++) {
        std::string nombreGrillo = "grillo_" + std::to_string(i);

        // CAMBIADO: Usar el nombre base "grillo" que ya fue cargado en inicializarAudio
        audioManager.reproducirSonidoAmbiental(
            "grillos",  // ← Usar siempre el mismo nombre base
            posicionesGrillos[i],
            0.9f,  // Volumen alto
            true   // Loop activado
        );
    }

    std::cout << "[SceneInformation] Grillos activados (noche) - 10 instancias del mismo sonido" << std::endl;
}

void SceneInformation::desactivarGrillos()
{
    // Detener todos los sonidos que contengan "grillo_" en su nombre
    audioManager.detenerSonidosAmbientalesPorPatron("grillo_");

    std::cout << "[SceneInformation] Grillos desactivados (día)" << std::endl;
}

void SceneInformation::limpiarSpotLightsActuales()
{
    spotLightCountActual = 0;
}

void SceneInformation::vincularRecursos(Entidad* entidad)
{
    if (entidad == nullptr) return;

    // Vincular modelo si la entidad usa un modelo
    if (entidad->getTipoObjeto() == TipoObjeto::MODELO) {
        std::string modelName = entidad->nombreModelo;
        if (!modelName.empty()) {
            Model* modelo = modelManager.getModel(modelName);
            if (modelo != nullptr) {
                entidad->setModelo(modelName, modelo);
            }
        }
    }

    // Vincular mesh si la entidad usa un mesh
    if (entidad->getTipoObjeto() == TipoObjeto::MESH) {
        std::string meshName = entidad->nombreMesh;
        if (!meshName.empty()) {
            Mesh* mesh = meshManager.getMesh(meshName);
            if (mesh != nullptr) {
                entidad->setMesh(meshName, mesh);
            }
        }
    }

    // Vincular textura si la entidad tiene un nombre de textura
    std::string textureName = entidad->nombreTextura;
    if (!textureName.empty()) {
        Texture* textura = textureManager.getTexture(textureName);
        if (textura != nullptr) {
            entidad->setTextura(textureName, textura);
        }
    }

    // Vincular material si la entidad tiene un nombre de material
    std::string materialName = entidad->nombreMaterial;
    if (!materialName.empty()) {
        Material* material = materialManager.getMaterial(materialName);
        if (material != nullptr) {
            entidad->setMaterial(materialName, material);
        }
    }

    // Vincular recursos recursivamente para todas las entidades hijas
    for (auto* hijo : entidad->hijos) {
        vincularRecursos(hijo);
    }
}

void SceneInformation::crearPiso()
{
    // Crear el piso
    Entidad* piso = new Entidad("piso");
    piso->setTipoObjeto(TipoObjeto::MESH);
    piso->nombreMesh = AssetConstants::MeshNames::PISO;
    piso->nombreTextura = AssetConstants::TextureNames::PASTO;
    piso->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    piso->posicionLocal = glm::vec3(0.0f, -1.0f, 0.0f);
    piso->escalaLocal = glm::vec3(30.0f, 1.0f, 30.0f);
    piso->actualizarTransformacion();
    agregarEntidad(piso);
}