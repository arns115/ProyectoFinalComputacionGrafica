#pragma once
#include "AssetConstants.h"
#include "Model.h"
#include <map>

// Clase para gestionar los modelos
class ModelManager
{
public:

	ModelManager();

	void renderModel(const std::string& modelName);

	~ModelManager();

private:
	std::map<std::string, Model*> models;

	void loadModel(const std::string& modelName, const std::string& modelPath);


};