#pragma once
#include <string>
#include <vector>

// Namespace para guardar nombres y rutas de assets
namespace AssetConstants {
	
	// Nombres de texturas
	namespace TextureNames{
		const std::string AGAVE = "AGAVE";
		const std::string PASTO = "PASTO";
		const std::string LADRILLO = "BRICK";
		const std::string TIERRA = "DIRT";

		
	}


	// Nombres de modelos
	namespace ModelNames{

	}

	// Rutas de texturas
	namespace TexturePaths{
		const std::string TEXTURE_PATH = "Textures/";
		const std::string AGAVE_PATH = TEXTURE_PATH + "Agave.png";
		const std::string PASTO_PATH = TEXTURE_PATH + "textura_pasto.png";
		const std::string LADRILLO_PATH = TEXTURE_PATH + "brick.png";
		const std::string TIERRA_PATH = TEXTURE_PATH + "dirt.png";
	}

	// Rutas de modelos
	namespace ModelPaths {
		const std::string MODEL_PATH = "Models/";

	}

}