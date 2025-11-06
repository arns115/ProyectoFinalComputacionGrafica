#pragma once
#include <string>
#include <vector>

// Namespace para guardar nombres y rutas de assets
namespace AssetConstants {
	
	// Nombres de texturas
	namespace TextureNames{
		const std::string AGAVE = "agave";
		const std::string PASTO = "pasto";
		const std::string LADRILLO = "brick";
		const std::string TIERRA = "dirt";
		const std::string EMPEDRADO = "empedrado";
		const std::string AGUA = "agua";
		// Texturas de Cuphead
		const std::string CUPHEAD_TEXTURE = "cuphead_texture";
		const std::string POPOTE_ROJO = "popote_rojo";
	}


	// Nombres de modelos
	namespace ModelNames{
		const std::string CABEZA_OLMECA = "cabeza_olmeca";
		const std::string PIRAMIDE = "piramide";

		const std::string CUPHEAD = "cuphead";

		const std::string CABEZA_HOLLOW = "cabeza_hollow";
		const std::string CUERPO1_HOLLOW = "cuerpo1_hollow";
		const std::string CUERPO2_HOLLOW = "cuerpo2_hollow";
		const std::string CUERPO3_HOLLOW = "cuerpo3_hollow";

		// Cuphead - modelo jerárquico
		const std::string CUPHEAD_TORSO = "cuphead_torso";
		const std::string CUPHEAD_CABEZA = "cuphead_cabeza";
		const std::string CUPHEAD_LECHE = "cuphead_leche";
		const std::string CUPHEAD_POPOTE = "cuphead_popote";
		const std::string CUPHEAD_BRAZO_IZQUIERDO = "cuphead_brazo_izquierdo";
		const std::string CUPHEAD_BRAZO_DERECHO = "cuphead_brazo_derecho";
		const std::string CUPHEAD_ANTEBRAZO_IZQUIERDO = "cuphead_antebrazo_izquierdo";
		const std::string CUPHEAD_ANTEBRAZO_DERECHO = "cuphead_antebrazo_derecho";
		const std::string CUPHEAD_MUSLO_IZQUIERDO = "cuphead_muslo_izquierdo";
		const std::string CUPHEAD_MUSLO_DERECHO = "cuphead_muslo_derecho";
		const std::string CUPHEAD_PIE_IZQUIERDO = "cuphead_pie_izquierdo";
		const std::string CUPHEAD_PIE_DERECHO = "cuphead_pie_derecho";
		
		// Isaac - modelo jerárquico
		const std::string ISAAC_CUERPO = "isaac_cuerpo";
		const std::string ISAAC_CABEZA = "isaac_cabeza";
		const std::string ISAAC_BRAZO_IZQUIERDO = "isaac_brazo_izquierdo";
		const std::string ISAAC_BRAZO_DERECHO = "isaac_brazo_derecho";
		const std::string ISAAC_PIERNA_IZQUIERDA = "isaac_pierna_izquierda";
		const std::string ISAAC_PIERNA_DERECHA = "isaac_pierna_derecha";

		// Boss room
		const std::string BOSS_ROOM = "boss_room";
		const std::string SECRET_ROOM = "secret_room";
		const std::string R_KEY = "r_key";


		// Fogatas
		const std::string FUEGO_ROJO = "fuego_rojo";
		const std::string FUEGO_MORADO = "fuego_morado";
		const std::string FUEGO_AZUL = "fuego_azul";

		const std::string COMIDA_PERRO = "comida_perro";
	}

	// Nombres de meshes
	namespace MeshNames{
		const std::string PISO = "piso";
		const std::string PIRAMIDE = "piramide";
		const std::string VEGETACION = "vegetacion";
		const std::string ESFERA = "esfera";  
		const std::string CAMINO = "camino";
		const std::string PRISMA_AGUA = "prisma_agua";
		const std::string PRISMA_PEQUENO = "prisma_pequeno";
	}

	// Nombres de materiales
	namespace MaterialNames{
		const std::string BRILLANTE = "brillante";
		const std::string OPACO = "opaco";
	}

	// Nombres de luces
	namespace LightNames{
		// Luces direccionales
		const std::string ESTRELLAS = "estrellas";
		const std::string SOL = "sol";  
		
		// Luces puntuales
		const std::string PUNTUAL_ROJA = "puntual_roja"; 
		
		// Spotlights
		const std::string LINTERNA = "linterna";  
	}

	// Rutas de texturas
	namespace TexturePaths{
		const std::string TEXTURE_PATH = "Textures/";
		const std::string AGAVE_PATH = TEXTURE_PATH + "Agave.tga";
		const std::string PASTO_PATH = TEXTURE_PATH + "textura_pasto.png";
		const std::string LADRILLO_PATH = TEXTURE_PATH + "brick.png";
		const std::string TIERRA_PATH = TEXTURE_PATH + "dirt.png";
		const std::string EMPEDRADO_PATH = TEXTURE_PATH + "empedrado.png";
		const std::string AGUA_PATH = TEXTURE_PATH + "agua.png";
		// Rutas de texturas de Cuphead
		const std::string CUPHEAD_TEXTURE_PATH = TEXTURE_PATH + "cuphead_texture.png";
		const std::string POPOTE_ROJO_PATH = TEXTURE_PATH + "popote_rojo.png";
	}

	// Rutas de modelos
	namespace ModelPaths {
		const std::string MODEL_PATH = "Models/";

		const std::string CABEZA_OLMECA = MODEL_PATH + "cabeza_olmeca.obj";
		const std::string PIRAMIDE = MODEL_PATH + "piramide.obj";

		const std::string CUPHEAD = MODEL_PATH + "cuphead.dae";

		const std::string CABEZA_HOLLOW = MODEL_PATH + "cabeza_hollow.obj";
		const std::string CUERPO1_HOLLOW = MODEL_PATH + "cuerpo1_hollow.obj";
		const std::string CUERPO2_HOLLOW = MODEL_PATH + "cuerpo2_hollow.obj";
		const std::string CUERPO3_HOLLOW = MODEL_PATH + "cuerpo3_hollow.obj";


		
		// Cuphead - modelo jerárquico en formato .dae
		const std::string CUPHEAD_TORSO = MODEL_PATH + "cuphead_torso.dae";
		const std::string CUPHEAD_CABEZA = MODEL_PATH + "cuphead_cabeza.dae";
		const std::string CUPHEAD_LECHE = MODEL_PATH + "cuphead_leche.dae";
		const std::string CUPHEAD_POPOTE = MODEL_PATH + "cuphead_popote.dae";
		const std::string CUPHEAD_BRAZO_IZQUIERDO = MODEL_PATH + "cuphead_brazo_izquierdo.dae";
		const std::string CUPHEAD_BRAZO_DERECHO = MODEL_PATH + "cuphead_brazo_derecho.dae";
		const std::string CUPHEAD_ANTEBRAZO_IZQUIERDO = MODEL_PATH + "cuphead_antebrazo_izquierdo.dae";
		const std::string CUPHEAD_ANTEBRAZO_DERECHO = MODEL_PATH + "cuphead_antebrazo_derecho.dae";
		const std::string CUPHEAD_MUSLO_IZQUIERDO = MODEL_PATH + "cuphead_muslo_izquierdo.dae";
		const std::string CUPHEAD_MUSLO_DERECHO = MODEL_PATH + "cuphead_muslo_derecho.dae";
		const std::string CUPHEAD_PIE_IZQUIERDO = MODEL_PATH + "cuphead_pie_izquierdo.dae";
		const std::string CUPHEAD_PIE_DERECHO = MODEL_PATH + "cuphead_pie_derecho.dae";

		// Isaac - modelo jerárquico
		const std::string ISAAC_CUERPO = MODEL_PATH + "isaac_cuerpo.obj";
		const std::string ISAAC_CABEZA = MODEL_PATH + "isaac_cabeza.obj";
		const std::string ISAAC_BRAZO_IZQUIERDO = MODEL_PATH + "isaac_brazo_izquierdo.obj";
		const std::string ISAAC_BRAZO_DERECHO = MODEL_PATH + "isaac_brazo_derecho.obj";
		const std::string ISAAC_PIERNA_IZQUIERDA = MODEL_PATH + "isaac_pierna_izquierda.obj";
		const std::string ISAAC_PIERNA_DERECHA = MODEL_PATH +"isaac_pierna_derecha.obj";

		// Fogatas
		const std::string FUEGO_ROJO = MODEL_PATH + "fuego_rojo.obj";
		const std::string FUEGO_MORADO = MODEL_PATH + "fuego_morado.obj";
		const std::string FUEGO_AZUL = MODEL_PATH + "fuego_azul.obj";


		// Boss room
		const std::string BOSS_ROOM = MODEL_PATH + "boss_room.obj";
		const std::string SECRET_ROOM = MODEL_PATH + "secret_room.obj";

		const std::string R_KEY = MODEL_PATH + "r_key.obj";

		const std::string COMIDA_PERRO = MODEL_PATH + "comida_perro.obj";
	}

	// Nombres de shaders
	namespace ShaderNames {
		const std::string MAIN_SHADER = "main_shader";
		const std::string LIGHT_SHADER = "light_shader";
	}

	// Rutas de shaders
	namespace ShaderPaths {
		const std::string SHADER_PATH = "shaders/";
		const std::string VERTEX_SHADER = SHADER_PATH + "shader_light.vert";
		const std::string FRAGMENT_SHADER = SHADER_PATH + "shader_light.frag";
	}

	// Nombres de skybox

	namespace SkyboxNames {
		const std::string DAY = "dia";
		const std::string NIGHT = "noche";
	}

	// Rutas de texturas de skybox
	namespace SkyboxPaths {
		const std::string SKYBOX_PATH = "Textures/Skybox/";

		const std::string RIGHT = "right.png";
		const std::string LEFT = "left.png";
		const std::string TOP =  "top.png";
		const std::string BOTTOM = "bottom.png";
		const std::string BACK = "back.png";
		const std::string FRONT = "front.png";
	}

}