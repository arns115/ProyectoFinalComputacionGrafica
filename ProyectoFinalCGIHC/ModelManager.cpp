#include "ModelManager.h"

// Carga todos los modelos al inicializar el ModelManager
ModelManager::ModelManager()
{
	loadModel("cuphead", AssetConstants::ModelPaths::CUPHEAD);
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
