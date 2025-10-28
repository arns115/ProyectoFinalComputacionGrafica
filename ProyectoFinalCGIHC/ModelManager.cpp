#include "ModelManager.h"

// Carga todos los modelos al inicializar el ModelManager
ModelManager::ModelManager()
{

}

// Renderiza un modelo por su nombre
void ModelManager::renderModel(std::string modelName)
{
	auto it = models.find(modelName);
	if (it != models.end()) {
		it->second->RenderModel();
	}

}

ModelManager::~ModelManager()
{
}
