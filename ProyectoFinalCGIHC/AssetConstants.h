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
	}


	// Nombres de modelos
	namespace ModelNames{
		const std::string CUPHEAD = "cuphead";
		const std::string ISAAC_CUERPO = "isaac_cuerpo";
		const std::string ISAAC_CABEZA = "isaac_cabeza";
		const std::string ISAAC_BRAZO_IZQUIERDO = "isaac_brazo_izquierdo";
		const std::string ISAAC_BRAZO_DERECHO = "isaac_brazo_derecho";
		const std::string ISAAC_PIERNA_IZQUIERDA = "isaac_pierna_izquierda";
		const std::string ISAAC_PIERNA_DERECHA = "isaac_pierna_derecha";

		// Modelos para Gojo (jerarquia)
		const std::string GOJO = "gojo"; // modelo principal
		const std::string GOJO_BRAZO_IZQ = "gojobrazoizq";
		const std::string GOJO_BRAZO_DER = "gojobrazoder";
		const std::string GOJO_PIERNA_IZQ = "gojopiernaizq";
		const std::string GOJO_PIERNA_DER = "gojopiernader";
		const std::string GOJO_RODILLA_IZQ = "gojorodillaizq";
		const std::string GOJO_RODILLA_DER = "gojorodillader";

		//ESCENARIO AZTECA
		const std::string ESCENARIOAZTECA = "escenarioazteca";

		//Modelos para el mercado
		const std::string CARPAVACIA = "carpavacia";
		const std::string CARPAYMESA = "carpaymesa";
		const std::string CARPABUENA = "carpabuena";
		const std::string PUESTOPESCADOS = "puestopescados";
		const std::string PUESTOKEKAS = "puestokekas";
	}

	// Nombres de meshes
	namespace MeshNames{
		const std::string PISO = "piso";
		const std::string PIRAMIDE = "piramide";
		const std::string VEGETACION = "vegetacion";
		const std::string ESFERA = "esfera";  // NUEVO
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
	}

	// Rutas de modelos
	namespace ModelPaths {
		const std::string MODEL_PATH = "Models/";
		const std::string CUPHEAD = MODEL_PATH + "cuphead.dae";

		const std::string ISAAC_CUERPO = MODEL_PATH + "isaac_cuerpo.obj";
		const std::string ISAAC_CABEZA = MODEL_PATH + "isaac_cabeza.obj";
		const std::string ISAAC_BRAZO_IZQUIERDO = MODEL_PATH + "isaac_brazo_izquierdo.obj";
		const std::string ISAAC_BRAZO_DERECHO = MODEL_PATH + "isaac_brazo_derecho.obj";
		const std::string ISAAC_PIERNA_IZQUIERDA = MODEL_PATH + "isaac_pierna_izquierda.obj";
		const std::string ISAAC_PIERNA_DERECHA = MODEL_PATH +"isaac_pierna_derecha.obj";

		// Rutas para modelos de Gojo (jerarquia)
		const std::string GOJO = MODEL_PATH + "gojo.obj";
		const std::string GOJOBRAZOIZQ = MODEL_PATH + "gojobrazoizq.obj";
		const std::string GOJOBRAZODER = MODEL_PATH + "gojobrazoder.obj";
		const std::string GOJOPIERNAIZQ = MODEL_PATH + "gojopiernaizq.obj";
		const std::string GOJOPIERNADER = MODEL_PATH + "gojopiernader.obj";
		const std::string GOJORODILLAIZQ = MODEL_PATH + "gojorodillaizq.obj";
		const std::string GOJORODILLADER = MODEL_PATH + "gojorodillader.obj";

		//ESCENARIO AZTECA
		const std::string ESCENARIOAZTECA = MODEL_PATH + "escenarioazteca.obj";

		//Modelos para el mercado
		const std::string CARPAVACIA = MODEL_PATH + "carpavacia.obj";
		const std::string CARPAYMESA = MODEL_PATH + "carpaymesa.obj";
		const std::string CARPABUENA = MODEL_PATH + "carpabuena.obj";
		const std::string PUESTOPESCADOS = MODEL_PATH + "puestopescados.obj";
		const std::string PUESTOKEKAS = MODEL_PATH + "puestokekas.obj";
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