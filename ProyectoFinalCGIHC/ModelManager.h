#pragma once
#include "AssetConstants.h"
#include "Model.h"
#include <map>

// Clase para gestionar los modelos
class ModelManager
{
public:

	ModelManager();

	void renderModel(std::string);

	~ModelManager();

private:
	std::map<std::string, Model*> models;




};