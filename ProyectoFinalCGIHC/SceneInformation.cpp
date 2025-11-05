#include "SceneInformation.h"
#include "ComponenteFisico.h"
#include "ComponenteAnimacion.h"

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
}


// Funcion para actualizar cada frame con el input del usuario
void SceneInformation::actualizarFrameInput(bool* keys, GLfloat mouseXChange, GLfloat mouseYChange, GLfloat scrollChange, float deltaTime)
{
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
    crearObjetosGeometricos(); 


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
        glm::vec3(0.0f, 0.0f, 1.0f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));
    
    cuphead_leche->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_leche->setModelo(AssetConstants::ModelNames::CUPHEAD_LECHE, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_LECHE));
    cuphead_leche->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    
    // 4. Crear el popote (hijo de la leche)
    Entidad* cuphead_popote = new Entidad("cuphead_popote",
        glm::vec3(0.0f, 0.0f, 1.0f),       // Posición relativa (ya está en el modelo)
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));
    
    cuphead_popote->setTipoObjeto(TipoObjeto::MODELO);
    cuphead_popote->setModelo(AssetConstants::ModelNames::CUPHEAD_POPOTE, modelManager.getModel(AssetConstants::ModelNames::CUPHEAD_POPOTE));
    cuphead_popote->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
    
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
        glm::vec3(0.0f, -1.0f, 10.0f),      // Posición inicial
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
        glm::vec3(-0.48f, 0.51f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    Entidad* isaac_pierna_derecha = new Entidad("isaac_pierna_derecha",
        glm::vec3(0.455f, 0.51f, 0.0f),      // Posición inicial
        glm::vec3(0.0f, 0.0f, 0.0f),     // Rotación
        glm::vec3(1.0f, 1.0f, 1.0f));      // Escala

    isaac_cuerpo->setTipoObjeto(TipoObjeto::MODELO);
    isaac_cuerpo->setModelo(AssetConstants::ModelNames::ISAAC_CUERPO, modelManager.getModel(AssetConstants::ModelNames::ISAAC_CUERPO));
    if(isaac_cuerpo->nombreModelo.empty())
    {
        std::cout << "Error al cargar el modelo de Isaac Cuerpo" << std::endl;
	}
	isaac_cuerpo->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));
	
	// Crear y configurar componente de física
	isaac_cuerpo->fisica = new ComponenteFisico();
	isaac_cuerpo->fisica->habilitar(true);
	isaac_cuerpo->fisica->gravedad = -0.5f;
	
	// Crear y configurar componente de animación
	isaac_cuerpo->animacion = new ComponenteAnimacion(isaac_cuerpo);

	isaac_brazo_derecho->setTipoObjeto(TipoObjeto::MODELO);
	isaac_brazo_derecho->setModelo(AssetConstants::ModelNames::ISAAC_BRAZO_DERECHO, modelManager.getModel(AssetConstants::ModelNames::ISAAC_BRAZO_DERECHO));
	isaac_brazo_derecho->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

	isaac_brazo_izquierdo->setTipoObjeto(TipoObjeto::MODELO);
	isaac_brazo_izquierdo->setModelo(AssetConstants::ModelNames::ISAAC_BRAZO_IZQUIERDO, modelManager.getModel(AssetConstants::ModelNames::ISAAC_BRAZO_IZQUIERDO));
	isaac_brazo_izquierdo->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

	isaac_cabeza->setTipoObjeto(TipoObjeto::MODELO);
	isaac_cabeza->setModelo(AssetConstants::ModelNames::ISAAC_CABEZA, modelManager.getModel(AssetConstants::ModelNames::ISAAC_CABEZA));
	isaac_cabeza->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

	isaac_pierna_derecha->setTipoObjeto(TipoObjeto::MODELO);
	isaac_pierna_derecha->setModelo(AssetConstants::ModelNames::ISAAC_PIERNA_DERECHA, modelManager.getModel(AssetConstants::ModelNames::ISAAC_PIERNA_DERECHA));
	isaac_pierna_derecha->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));

	isaac_pierna_izquierda->setTipoObjeto(TipoObjeto::MODELO);
	isaac_pierna_izquierda->setModelo(AssetConstants::ModelNames::ISAAC_PIERNA_IZQUIERDA, modelManager.getModel(AssetConstants::ModelNames::ISAAC_PIERNA_IZQUIERDA));
	isaac_pierna_izquierda->setMaterial(AssetConstants::MaterialNames::BRILLANTE, materialManager.getMaterial(AssetConstants::MaterialNames::BRILLANTE));


    isaac_cuerpo->agregarHijo(isaac_cabeza);
	isaac_cuerpo->agregarHijo(isaac_brazo_izquierdo);
	isaac_cuerpo->agregarHijo(isaac_brazo_derecho);
	isaac_cuerpo->agregarHijo(isaac_pierna_izquierda);
	isaac_cuerpo->agregarHijo(isaac_pierna_derecha);


	agregarEntidad(isaac_cuerpo);
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
