#include "SceneInformation.h"
#include "ComponenteFisico.h"
#include "ComponenteAnimacion.h"
#include <cstdlib>
#include <ctime>

SceneInformation::SceneInformation()
    : skyboxActual(nullptr), pointLightCountActual(0), spotLightCountActual(0)
{
    // Llamar a las funciones de inicialización separadas
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

// Funcion para actualizar cada frame con las cosas que no dependen del input del usuario
void SceneInformation::actualizarFrame(float deltaTime)
{
    // Actualizar el ciclo dia/noche
    acumuladorTiempoDesdeCambio += deltaTime;
    if (acumuladorTiempoDesdeCambio >= 60.0f / LIMIT_FPS) // 60 segundos = 1 minuto
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

	// Actualizar animaciones de las entidades que tengan componente de animacion
    for (auto* entidad : entidades) {
        if (entidad != nullptr && entidad->animacion != nullptr) {
            if (entidad->nombreObjeto == "hollow") {
                entidad->animacion->actualizarAnimacion(1, deltaTime, 1.0);
            }
        }
    }

	// Actualizar animación de la canoa
	actualizarAnimacionCanoa(deltaTime);
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
	// 3: Cambia el modo tercera persona a Gojo
    if (keys[GLFW_KEY_3]) {
        camera.setThirdPersonTarget(entidades[(int)entidades.size() - 1]);
        personajeActual = 3;

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


}

// Funcion para inicializar la skybox
void SceneInformation::inicializarSkybox()
{
    // Establecer el skybox por defecto
    setSkyboxActual(AssetConstants::SkyboxNames::DAY);
}

// Funcion para inicializar la luz direccional
void SceneInformation::inicializarLuces()
{
    // Obtener la luz del sol desde el LightManager
    DirectionalLight* sunLight = lightManager.getDirectionalLight(AssetConstants::LightNames::SOL);
    
    if (sunLight != nullptr) {
        // Establecer la luz del sol como luz direccional principal
        luzDireccional = *sunLight;
    } else {
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
    crearChinampaAgua();
    crearIslas();
    crearArbolesAlrededorChinampa();
    crearCanoa();
    crearObjetosGeometricos();
	crearCabezaOlmeca();
    crearPiramide();
    crearHollow();
    crearObjetosGeometricos(); 
    crearBossRoom();
	crearSecretRoom();


	// Los personajes deben ser los ultimos en crearse para que la camara facilmente los pueda seguir (estaran en orden al final del vector de entidades)
    // Primero Cuphead
	// Segundo Isaac
    // Tercero Gojo
    crearPersonajePrincipal();
	  crearIsaac();
    crearIsaac(); // se crea por segunda vez en lo que se agrega a gojo


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
	room->actualizarTransformacion();
	agregarEntidad(room);
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
    piramide->actualizarTransformacion();
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
    float desplazamientoInicial = -espaciado; // Comenzar desde la izquierda/atrás
    
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
    float anchoChinampa = 10.0f * escalaChinampa; // 80 unidades
    
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
        glm::vec3(0.5f, 0.5f, 0.5f));  // Escala
    
    canoa->setTipoObjeto(TipoObjeto::MODELO);
    canoa->nombreModelo = AssetConstants::ModelNames::CANOA;
    canoa->nombreMaterial = AssetConstants::MaterialNames::OPACO;
    canoa->actualizarTransformacion();
    
    // Crear maya como hijo de la canoa
    Entidad* maya = new Entidad("maya_canoa",
        glm::vec3(0.0f, 1.0f, 0.0f),  // Posición relativa encima de la canoa
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
    float anchoChinampa = 10.0f * escalaChinampa;
    float alturaAgua = posicionChinampa.y + 0.1f * escalaChinampa + 0.2f;
    float margen = 10.0f;
    
    // Estados:
    // 0: Mover de esquina superior izquierda a inferior izquierda
    // 1: Girar 90 grados (hacia la derecha)
    // 2: Mover de esquina inferior izquierda a inferior derecha
    // 3: Girar 90 grados
    // 4: Mover de esquina inferior derecha a superior derecha
    // 5: Girar 90 grados
    // 6: Mover de esquina superior derecha a superior izquierda
    // 7: Girar 90 grados
    
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
                case 0: // Esquina superior izquierda a inferior izquierda
                    posicionDestinoCanoa = glm::vec3(
                        posicionChinampa.x - anchoChinampa / 2.0f + margen,
                        alturaAgua,
                        posicionChinampa.z + anchoChinampa / 2.0f - margen
                    );
                    break;
                case 2: // Esquina inferior izquierda a inferior derecha
                    posicionDestinoCanoa = glm::vec3(
                        posicionChinampa.x + anchoChinampa / 2.0f - margen,
                        alturaAgua,
                        posicionChinampa.z + anchoChinampa / 2.0f - margen
                    );
                    break;
                case 4: // Esquina inferior derecha a superior derecha
                    posicionDestinoCanoa = glm::vec3(
                        posicionChinampa.x + anchoChinampa / 2.0f - margen,
                        alturaAgua,
                        posicionChinampa.z - anchoChinampa / 2.0f + margen
                    );
                    break;
                case 6: // Esquina superior derecha a superior izquierda
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
        glm::vec3(0.0f, 45.0f, 0.0f),      // Rotación de 45 grados
        glm::vec3(1.5f, 1.5f, 1.5f));      // Escala 1.5x
    
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

bool SceneInformation::agregarLuzPuntualActual(const PointLight& light)
{
    if (pointLightCountActual >= MAX_POINT_LIGHTS) {
        return false; // No hay espacio para más luces actuales
    }
    
    // Agregar al arreglo de luces actuales
    pointLightsActuales[pointLightCountActual] = light;
    pointLightCountActual++;
    
    return true;
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
