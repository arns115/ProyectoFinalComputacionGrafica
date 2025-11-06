#include "ModelManager.h"

// Carga todos los modelos al inicializar el ModelManager
ModelManager::ModelManager()
{
	
	loadModel(AssetConstants::ModelNames::CABEZA_OLMECA, AssetConstants::ModelPaths::CABEZA_OLMECA);
	loadModel(AssetConstants::ModelNames::PIRAMIDE, AssetConstants::ModelPaths::PIRAMIDE);

	loadModel(AssetConstants::ModelNames::CABEZA_HOLLOW, AssetConstants::ModelPaths::CABEZA_HOLLOW);
	loadModel(AssetConstants::ModelNames::CUERPO1_HOLLOW, AssetConstants::ModelPaths::CUERPO1_HOLLOW);
	loadModel(AssetConstants::ModelNames::CUERPO2_HOLLOW, AssetConstants::ModelPaths::CUERPO2_HOLLOW);
	loadModel(AssetConstants::ModelNames::CUERPO3_HOLLOW, AssetConstants::ModelPaths::CUERPO3_HOLLOW);


	loadModel("cuphead", AssetConstants::ModelPaths::CUPHEAD);


	// Cargar modelos de Cuphead (jerárquico)
	loadModel(AssetConstants::ModelNames::CUPHEAD_TORSO, AssetConstants::ModelPaths::CUPHEAD_TORSO);
	loadModel(AssetConstants::ModelNames::CUPHEAD_CABEZA, AssetConstants::ModelPaths::CUPHEAD_CABEZA);
	loadModel(AssetConstants::ModelNames::CUPHEAD_LECHE, AssetConstants::ModelPaths::CUPHEAD_LECHE);
	loadModel(AssetConstants::ModelNames::CUPHEAD_POPOTE, AssetConstants::ModelPaths::CUPHEAD_POPOTE);
	loadModel(AssetConstants::ModelNames::CUPHEAD_BRAZO_IZQUIERDO, AssetConstants::ModelPaths::CUPHEAD_BRAZO_IZQUIERDO);
	loadModel(AssetConstants::ModelNames::CUPHEAD_BRAZO_DERECHO, AssetConstants::ModelPaths::CUPHEAD_BRAZO_DERECHO);
	loadModel(AssetConstants::ModelNames::CUPHEAD_ANTEBRAZO_IZQUIERDO, AssetConstants::ModelPaths::CUPHEAD_ANTEBRAZO_IZQUIERDO);
	loadModel(AssetConstants::ModelNames::CUPHEAD_ANTEBRAZO_DERECHO, AssetConstants::ModelPaths::CUPHEAD_ANTEBRAZO_DERECHO);
	loadModel(AssetConstants::ModelNames::CUPHEAD_MUSLO_IZQUIERDO, AssetConstants::ModelPaths::CUPHEAD_MUSLO_IZQUIERDO);
	loadModel(AssetConstants::ModelNames::CUPHEAD_MUSLO_DERECHO, AssetConstants::ModelPaths::CUPHEAD_MUSLO_DERECHO);
	loadModel(AssetConstants::ModelNames::CUPHEAD_PIE_IZQUIERDO, AssetConstants::ModelPaths::CUPHEAD_PIE_IZQUIERDO);
	loadModel(AssetConstants::ModelNames::CUPHEAD_PIE_DERECHO, AssetConstants::ModelPaths::CUPHEAD_PIE_DERECHO);
	
	// Cargar modelos de Isaac (jerárquico)
	loadModel(AssetConstants::ModelNames::ISAAC_CUERPO, AssetConstants::ModelPaths::ISAAC_CUERPO);
	loadModel(AssetConstants::ModelNames::ISAAC_CABEZA, AssetConstants::ModelPaths::ISAAC_CABEZA);
	loadModel(AssetConstants::ModelNames::ISAAC_BRAZO_IZQUIERDO, AssetConstants::ModelPaths::ISAAC_BRAZO_IZQUIERDO);
	loadModel(AssetConstants::ModelNames::ISAAC_BRAZO_DERECHO, AssetConstants::ModelPaths::ISAAC_BRAZO_DERECHO);
	loadModel(AssetConstants::ModelNames::ISAAC_PIERNA_IZQUIERDA, AssetConstants::ModelPaths::ISAAC_PIERNA_IZQUIERDA);
	loadModel(AssetConstants::ModelNames::ISAAC_PIERNA_DERECHA, AssetConstants::ModelPaths::ISAAC_PIERNA_DERECHA);

	// Boss room
	loadModel(AssetConstants::ModelNames::BOSS_ROOM, AssetConstants::ModelPaths::BOSS_ROOM);
	loadModel(AssetConstants::ModelNames::SECRET_ROOM, AssetConstants::ModelPaths::SECRET_ROOM);
	loadModel(AssetConstants::ModelNames::R_KEY, AssetConstants::ModelPaths::R_KEY);

	// Fogatas
	loadModel(AssetConstants::ModelNames::FUEGO_ROJO, AssetConstants::ModelPaths::FUEGO_ROJO);
	loadModel(AssetConstants::ModelNames::FUEGO_MORADO, AssetConstants::ModelPaths::FUEGO_MORADO);
	loadModel(AssetConstants::ModelNames::FUEGO_AZUL, AssetConstants::ModelPaths::FUEGO_AZUL);

	// Comida de perro (Item en Binding of isaac)
	loadModel(AssetConstants::ModelNames::COMIDA_PERRO, AssetConstants::ModelPaths::COMIDA_PERRO);
}


// Carga un modelo en el manager
void ModelManager::loadModel(const std::string& modelName, const std::string& modelPath)
{
	Model* model = new Model();
	model->LoadModel(modelPath);
	models[modelName] = model;
}

// Obtiene un modelo por su nombre (retorna apuntador)
Model* ModelManager::getModel(const std::string& modelName)
{
	auto it = models.find(modelName);
	if (it != models.end()) {
		return it->second;
	}
	return nullptr;
}

ModelManager::~ModelManager()
{
}
