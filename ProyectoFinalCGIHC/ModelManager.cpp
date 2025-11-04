#include "ModelManager.h"

// Carga todos los modelos al inicializar el ModelManager
ModelManager::ModelManager()
{
	loadModel("cuphead", AssetConstants::ModelPaths::CUPHEAD);
	loadModel(AssetConstants::ModelNames::ISAAC_CUERPO, AssetConstants::ModelPaths::ISAAC_CUERPO);
	loadModel(AssetConstants::ModelNames::ISAAC_CABEZA, AssetConstants::ModelPaths::ISAAC_CABEZA);
	loadModel(AssetConstants::ModelNames::ISAAC_BRAZO_IZQUIERDO, AssetConstants::ModelPaths::ISAAC_BRAZO_IZQUIERDO);
	loadModel(AssetConstants::ModelNames::ISAAC_BRAZO_DERECHO, AssetConstants::ModelPaths::ISAAC_BRAZO_DERECHO);
	loadModel(AssetConstants::ModelNames::ISAAC_PIERNA_IZQUIERDA, AssetConstants::ModelPaths::ISAAC_PIERNA_IZQUIERDA);
	loadModel(AssetConstants::ModelNames::ISAAC_PIERNA_DERECHA, AssetConstants::ModelPaths::ISAAC_PIERNA_DERECHA);
}


// Carga un modelo en el manager
void ModelManager::loadModel(const std::string& modelName, const std::string& modelPath)
{
	Model* model = new Model();
	model->LoadModel(modelPath);
	models[modelName] = model;
}

// Obtiene un modelo por su nombre (retorna puntero)
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
