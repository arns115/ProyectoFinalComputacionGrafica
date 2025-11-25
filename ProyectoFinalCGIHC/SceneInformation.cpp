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
        esDeDia = !esDeDia; // Cambiar entre dia y noche
        acumuladorTiempoDesdeCambio = 0.0f; // Reiniciar el acumulador
        if (esDeDia)
        {
            // Se pone la skyboxAdecuada y la luz direccional del sol
            setSkyboxActual(AssetConstants::SkyboxNames::DAY);
            luzDireccional = *lightManager.getDirectionalLight(AssetConstants::LightNames::SOL);
        }
        else
        {
            // Se pone la skyboxAdecuada y la luz direccional de las estrellas
            setSkyboxActual(AssetConstants::SkyboxNames::NIGHT);
            luzDireccional = *lightManager.getDirectionalLight(AssetConstants::LightNames::ESTRELLAS);

        }
    }
    
	// Se posiciona linterna en la posicion y direccion de la camara
    spotLightActual = *lightManager.getSpotLight(AssetConstants::LightNames::LINTERNA);
	posicionLuzActual = camera.getCameraPosition();
	direccionLuzActual = camera.getCameraDirection();
    spotLightActual.SetFlash(posicionLuzActual, direccionLuzActual);
	agregarSpotLightActual(spotLightActual);

	// Actualizar animación de la canoa
	actualizarAnimacionCanoa(deltaTime);
    
    // Obtener posición del personaje activo
    glm::vec3 posicionPersonajeActivo(0.0f);
    if (entidades.size() >= 3) {
        // Los personajes están al final del vector
        // 1: Cuphead (size - 3), 2: Isaac (size - 2), 3: Gojo (size - 1)
        int indicePersonaje = static_cast<int>(entidades.size()) - (4 - personajeActual);
        if (indicePersonaje >= 0 && indicePersonaje < entidades.size() && entidades[indicePersonaje] != nullptr) {
            posicionPersonajeActivo = entidades[indicePersonaje]->posicionLocal;
        }
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
                lucesTemporales.push_back({pointLightActual, distancia});
            }
            // si esta activa la animacion se llama a la funcion de actualizarala
            if (entidad->nombreObjeto == "puerta_secret_room" && entidad->animacion->estaActiva(0)) {
				entidad->animacion->actualizarAnimacion(0, deltaTime, 1.0);
            }
            if(entidad->nombreObjeto == "pelota") {
                entidad->animacion->animateKeyframes();
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
                        lucesTemporales.push_back({pointLightActual, distancia});
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

	// Actualizar animación de la canoa
	actualizarAnimacionCanoa(deltaTime);

    // Actualizar animación del luchador
    actualizarAnimacionLuchador(deltaTime);
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
    if(keys[GLFW_KEY_1]) {
        camera.setThirdPersonTarget(entidades[(int)entidades.size() - 3]);
        personajeActual = 1;
	}
	// 2: Cambia el modo tercera persona a Isaac
    if (keys[GLFW_KEY_2]) {
        camera.setThirdPersonTarget(entidades[(int)entidades.size() - 2]);
        personajeActual = 2;

    }
	// 3: Cambia el modo tercera persona a Luchador
    if (keys[GLFW_KEY_3]) {
        camera.setThirdPersonTarget(entidades[(int)entidades.size() - 1]);
        personajeActual = 3;

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
    }
    
    // G: Activar/Desactivar animación de la canoa
    static bool teclaGPresionada = false;
    if (keys[GLFW_KEY_G]) {
        if (!teclaGPresionada) {
            animacionCanoaActiva = !animacionCanoaActiva;
            teclaGPresionada = true;
        }
    } else {
        teclaGPresionada = false;
    }
    
    // O: Activar/Desactivar luces del ring
    static bool teclaOPresionada = false;
    if (keys[GLFW_KEY_O]) {
        if (!teclaOPresionada) {
            lucesRingActivas = !lucesRingActivas;
            teclaOPresionada = true;
        }
    } else {
        teclaOPresionada = false;
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

    // Guardar referencia al luchador y posición inicial
    luchador = luchador_torso;
    posicionInicialLuchador = luchador_torso->posicionLocal;
    
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

// Crear fogatas en la escena
void SceneInformation::crearFogatas(){
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
            if(columna == 1){
                chinampaIsla->nombreTextura = AssetConstants::TextureNames::PASTO; 
            } else {
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
    } else if (tipoArbol == "B") {
        modeloArbol = AssetConstants::ModelNames::ARBOL_B;
        nombreArbol = nombre.empty() ? "arbol_b" : nombre;
    } else if (tipoArbol == "C") {
        modeloArbol = AssetConstants::ModelNames::ARBOL_C;
        nombreArbol = nombre.empty() ? "arbol_c" : nombre;
    } else {
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
    std::string tiposArbol[] = {"A", "B", "C"};
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
        } else if (tipo == "B") {
            escalaFinal = 1.5f * variacionEscala;
        } else { // tipo == "C"
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

// Crear canoa con maya jerárquica que navega por la chinampa de agua
void SceneInformation::crearCanoa()
{
    // Posición y escala de la chinampa de agua
    glm::vec3 posicionChinampa(-150.0f, -1.35f, -150.0f);
    float escalaChinampa = 8.0f;
    
    // Dimensiones de la chinampa escalada (el mesh base es 10x10)
    float anchoChinampa = 10.0f * escalaChinampa; // 80 unidades
    float altoChinampa = 10.0f * escalaChinampa;  // 80 unidades
    
    // Altura del agua (encima del prisma de agua)
    float alturaAgua = posicionChinampa.y + 0.1f * escalaChinampa + 0.2f;
    
    // Posición inicial: esquina superior izquierda
    float margen = 10.0f; // Margen desde los bordes
    glm::vec3 posicionInicial(
        posicionChinampa.x - anchoChinampa / 2.0f + margen,
        alturaAgua,
        posicionChinampa.z - anchoChinampa / 2.0f + margen
    );
    
    // Crear entidad de la canoa (padre)
    canoa = new Entidad("canoa",
        posicionInicial,
        glm::vec3(0.0f, 0.0f, 0.0f),  // Inicialmente mirando al frente
        glm::vec3(1.5f, 1.5f, 1.5f));  // Escala
    
    canoa->setTipoObjeto(TipoObjeto::MODELO);
    canoa->nombreModelo = AssetConstants::ModelNames::CANOA;
    canoa->nombreMaterial = AssetConstants::MaterialNames::OPACO;
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
    
    // Inicializar variables de animación
    estadoAnimacionCanoa = 0;
    tiempoAnimacionCanoa = 0.0f;
    posicionInicioCanoa = posicionInicial;
    
    // Calcular primera posición destino (esquina inferior izquierda)
    posicionDestinoCanoa = glm::vec3(
        posicionChinampa.x - anchoChinampa / 2.0f + margen,
        alturaAgua,
        posicionChinampa.z + anchoChinampa / 2.0f - margen
    );
    rotacionObjetivoCanoa = 0.0f;
}

// Actualizar animación cíclica de la canoa
void SceneInformation::actualizarAnimacionCanoa(float deltaTime)
{
    if (canoa == nullptr || !animacionCanoaActiva) return;
    
    // Posición y dimensiones de la chinampa
    glm::vec3 posicionChinampa(-150.0f, -1.35f, -150.0f);
    float escalaChinampa = 8.0f;
    float anchoChinampa = 10.0f * escalaChinampa; // 80 unidades
    float altoChinampa = 10.0f * escalaChinampa;  // 80 unidades
    
    // Altura del agua (encima del prisma de agua)
    float alturaAgua = posicionChinampa.y + 0.1f * escalaChinampa + 0.2f;
    float margen = 10.0f; // Margen desde los bordes
    
    // estados:
    // 0: mover a esquina inferior izquierda
    // 1: girar 90 grados (hacia la derecha)
    // 2: mover a esquina inferior derecha
    // 3: girar 90 grados
    // 4: mover a esquina superior derecha
    // 5: girar 90 grados
    // 6: mover a esquina superior izquierda
    // 7: girar 90 grados
    
    bool estadoPar = (estadoAnimacionCanoa % 2 == 0);
    
    if (estadoPar) {
        // Estados de movimiento (0, 2, 4, 6)
        glm::vec3 direccion = glm::normalize(posicionDestinoCanoa - posicionInicioCanoa);
        float distancia = glm::length(posicionDestinoCanoa - posicionInicioCanoa);
        float distanciaRecorrida = velocidadCanoa * tiempoAnimacionCanoa;
        
        if (distanciaRecorrida >= distancia) {
            // Llegó al destino, cambiar al siguiente estado (girar)
            canoa->posicionLocal = posicionDestinoCanoa;
            estadoAnimacionCanoa++;
            tiempoAnimacionCanoa = 0.0f;
            
            // Calcular nueva rotación objetivo
            rotacionObjetivoCanoa = canoa->rotacionLocal.y + 90.0f;
        } else {
            // Mover hacia el destino
            canoa->posicionLocal = posicionInicioCanoa + direccion * distanciaRecorrida;
            tiempoAnimacionCanoa += deltaTime;
        }
    } else {
        // Estados de rotación (1, 3, 5, 7)
        float rotacionInicial = rotacionObjetivoCanoa - 90.0f;
        float anguloRecorrido = velocidadRotacionCanoa * tiempoAnimacionCanoa;
        
        if (anguloRecorrido >= 90.0f) {
            // Completó el giro, cambiar al siguiente estado (mover)
            canoa->rotacionLocal.y = rotacionObjetivoCanoa;
            if (rotacionObjetivoCanoa >= 360.0f) {
                canoa->rotacionLocal.y -= 360.0f;
                rotacionObjetivoCanoa -= 360.0f;
            }
            
            estadoAnimacionCanoa++;
            if (estadoAnimacionCanoa >= 8) {
                estadoAnimacionCanoa = 0; // Reiniciar ciclo
            }
            tiempoAnimacionCanoa = 0.0f;
            
            // Configurar nuevo movimiento según el estado
            posicionInicioCanoa = canoa->posicionLocal;
            
            switch (estadoAnimacionCanoa) {
                case 0: // Esquina inferior izquierda
                    posicionDestinoCanoa = glm::vec3(
                        posicionChinampa.x - anchoChinampa / 2.0f + margen,
                        alturaAgua,
                        posicionChinampa.z + anchoChinampa / 2.0f - margen
                    );
                    break;
                case 2: // Esquina inferior derecha
                    posicionDestinoCanoa = glm::vec3(
                        posicionChinampa.x + anchoChinampa / 2.0f - margen,
                        alturaAgua,
                        posicionChinampa.z + anchoChinampa / 2.0f - margen
                    );
                    break;
                case 4: // Esquina superior derecha
                    posicionDestinoCanoa = glm::vec3(
                        posicionChinampa.x + anchoChinampa / 2.0f - margen,
                        alturaAgua,
                        posicionChinampa.z - anchoChinampa / 2.0f + margen
                    );
                    break;
                case 6: // Esquina superior izquierda
                    posicionDestinoCanoa = glm::vec3(
                        posicionChinampa.x - anchoChinampa / 2.0f + margen,
                        alturaAgua,
                        posicionChinampa.z - anchoChinampa / 2.0f + margen
                    );
                    break;
            }
        } else {
            // Girar gradualmente
            canoa->rotacionLocal.y = rotacionInicial + anguloRecorrido;
            tiempoAnimacionCanoa += deltaTime;
        }
    }
    
    // Actualizar transformación de la canoa
    canoa->actualizarTransformacion();
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
    audioManager.cargarSonidoAmbiental("abrir_puerta", "Audio/SFX/door_open.mp3");

    // Reproducir el soundtrack en loop con volumen tenue (30% del máximo)
    if (!audioManager.reproducirSoundtrack("cuphead_song", 0.1f)) {
        std::cerr << "[SceneInformation] Advertencia: No se pudo reproducir el soundtrack" << std::endl;
        std::cerr << "[SceneInformation] Verifica que el archivo existe en: Audio/Soundtrack/cuphead_song.wav" << std::endl;
    }
    else {
        std::cout << "[SceneInformation] Soundtrack reproduciéndose en loop (volumen: 30%)" << std::endl;
    }

    // Establecer volumen maestro también bajo para no interferir con efectos futuros
    audioManager.setVolumenMaestro(0.4f); // 80% del volumen maestro
	audioManager.setVolumenSoundtrack(0.1f); // 30% del volumen del soundtrack
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
