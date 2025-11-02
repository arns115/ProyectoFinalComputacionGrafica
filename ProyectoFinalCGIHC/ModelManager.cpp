#include "ModelManager.h"

// Carga todos los modelos al inicializar el ModelManager
ModelManager::ModelManager()
{
	loadModel("cuphead", AssetConstants::ModelPaths::CUPHEAD);
}


void ModelManager::loadModel(const std::string& modelName, const std::string& modelPath)
{
	Model* model = new Model();
	model->LoadModel(modelPath);
	models[modelName] = model;
}

// Renderiza un modelo por su nombre
void ModelManager::renderModel(const std::string& modelName)
{
	auto it = models.find(modelName);
	if (it != models.end()) {
		it->second->RenderModel();
	}

}

ModelManager::~ModelManager()
{
}
